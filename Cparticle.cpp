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
Mat Cparticle::par_dis;


void Cparticle::calcModel(){
    for(int i = 0; i < m_logmCmat.size(); i++){
        m_logmCmat[i].setTo(0);
    }
    for(int i = 0; i < m_tmplib.size(); i++){
        for(int j = 0; j < m_tmplib[i].size(); j++){ 
            m_logmCmat[j] += m_tmplib[i][j];
        }
    }
    for(int i = 0; i < m_logmCmat.size(); i++){
        m_logmCmat[i] /= m_tmplib.size();
    }


}

void Cparticle::calccovmat(CovImage &cim, Parameter &para){
    if(para.nModes == 9){
        calc9covmat(cim,para.v9);
    }
    else if(para.nModes == 3){
        calc3covmat(cim,para.v3);
    }
}


void Cparticle::calc9covmat(CovImage &cim,vector<vector<int>> &v){

    m_cmat.resize(9);
    m_logmCmat.resize(9);
    Mat IIprod[4];
    Mat IIsum[4];

    int qx1[4], qy1[4], qx2[4], qy2[4];
    double Npixels[4];

    double *ptr = m_pos.ptr<double>(0);
    utils::getQuadrants(*(ptr), *(ptr+1), *(ptr+2), *(ptr+3), qx1, qy1, qx2, qy2);
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

void Cparticle::calc3covmat(CovImage &cim, vector<vector<int>> &v){
    m_cmat.resize(3);
    m_logmCmat.resize(3);
    Mat IIprod[2];
    Mat IIsum[2];

    int qx1[2], qy1[2], qx2[2], qy2[2];
    double Npixels[2];

    double *ptr = m_pos.ptr<double>(0);
    utils::getVerticalHalf(*(ptr), *(ptr+1), *(ptr+2), *(ptr+3), qx1, qy1, qx2, qy2);
    for (int i = 0; i < 2; i++) {
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

void Cparticle::logm(){
    for(int i = 0; i < m_cmat.size();i++){
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

void Cparticle::calcdis(Cparticle &tarpar,int k){
    double *p = par_dis.ptr<double>(k);
    for(int i = 0; i < m_logmCmat.size() ; i ++, p++){
        *p = norm(m_logmCmat[i] - tarpar.m_logmCmat[i]);
    }
}

// void Cparticle::calcwt(Mat &par_wt,int k){
//     double *pdis = m_distance.ptr<double>(0);
//     double *pwt  = par_wt.ptr<double>(k);
//     for(int i = 0 ; i < 9 ; ++i, ++pwt, ++pdis){
//         *pwt = 1/(*pdis);       
//     }
// }

void Cparticle::updateModel(CovImage &covimg, Parameter &para){
    calccovmat(covimg,para);
    logm();
    m_tmplib.pop_front();
    m_tmplib.push_back(m_logmCmat);
    for(int i = 0 ; i < para.nModes ; ++i){
        m_tmplib[para.templateNo-1][i] = m_logmCmat[i].clone();
    }
    calcModel();
    cerr<<"Model updated!"<<endl;
}