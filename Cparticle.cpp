#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <time.h>

#include <sstream>
#include <string>

#include <omp.h>

#include <assert.h>

#include "Cparticle.h"
#include "debug.h"
#include "utils.h"
#include <fstream>


Mat Cparticle::par_pos;
Mat Cparticle::par_weight;
Mat Cparticle::stddevm;

void Cparticle::calccovmat(CovImage &cim, vector<vector<int>> v, int nModes){
        if(nModes == 9){
            calc9covmat(cim,v);
        }
        else if(nModes == 3){
            calc3covmat(cim,v);
        }
}


void Cparticle::calc9covmat(CovImage &cim,vector<vector<int>> v){

     Mat IIprod[4];
     Mat IIsum[4];

     int qx1[4], qy1[4], qx2[4], qy2[4];
     double Npixels[4];

     double *ptr = m_pos.ptr<double>(0);
     utils::getQuadrants(*(ptr), *(ptr+1), *(ptr+2), *(ptr+3), qx1, qy1, qx2, qy2);
   //utils::getQuadrants((int)(ptr[0]+0.5), (int)(ptr[1]+0.5), (int)(ptr[2]+0.5), (int)(ptr[3]+0.5), qx1, qy1, qx2, qy2);
     for (int i=0; i < 4; i++) {
         cim.covComponentMatrices(qx1[i], qy1[i], qx2[i], qy2[i],
             IIprod[i], IIsum[i], Npixels[i]);
     }
     for(int i = 0; i < v.size() ; i++){
        Mat iiprod = Mat::zeros(IIprod[0].rows,IIprod[0].cols,CV_64F); 
        Mat iisum  =  Mat::zeros(IIsum[0].rows,IIsum[0].cols,CV_64F);
        int N      = 0;
        for(int j = 0; j < v[i].size(); j++){
                iiprod += IIprod[v[i][j]];
                iisum  += IIsum[v[i][j]];
                N      += Npixels[v[i][j]];
        }
        m_cmat[i] = iiprod / (N-1) - iisum*iisum.t() / (N*(N-1));
     }
}
void Cparticle::calc3covmat(CovImage &cim, vector<vector<int>> v){

}
void Cparticle::logm(){
    for(int i = 0; i < 9;i++){
        Mat U, W, V;
        SVD::compute(m_cmat[i], W, U, V);
        for(int j = 0; j < m_cmat[i].cols ;j++)
        {
            U(Range(0,m_cmat[i].cols),Range(j,j+1)) *= log(W.at<double>(j)); 
        }
        m_logmCmat[i] = U*V; 
        m_logmCmat[i] = (m_logmCmat[i] + m_logmCmat[i].t())/2;
    }
}

void Cparticle::calcdis(Cparticle tarpar){
    double *p = distance.ptr<double>(0);
    for(int i = 0; i < 9 ; i ++, p++){
        *p = norm(m_logmCmat[i] - tarpar.m_logmCmat[i]);
    }
}

void Cparticle::calcwt(Mat &par_wt,int k){
    double *pdis = distance.ptr<double>(0);
    double *pwt  = par_wt.ptr<double>(k);
    for(int i = 0 ; i < 9 ; i ++, pwt++, pdis++){
        *pwt = 1/((*pdis)*(*pdis));       
    }
}