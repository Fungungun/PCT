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
/***********************************************************/
Mat Cparticle::par_pos;
Mat Cparticle::par_dis;
Mat Cparticle::par_prob;
Mat Cparticle::sum_prob;
Mat Cparticle::stddevm;

/***********************************************************/
void Cparticle::calccovmat(CovImage &cim, Parameter &para){
    if(para.nModes == 9){
        calc9covmat(cim,para.v9);
    }
    else if(para.nModes == 3){
        calc3covmat(cim,para.v3);
    }
}
/***********************************************************/
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
/***********************************************************/
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
/***********************************************************/
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
/***********************************************************/
void Cparticle::ParticleProcess(Cparticle &tarpar,int k){
    double *pdis  = Cparticle::par_dis.ptr<double>(k);
    double *pprob = Cparticle::par_prob.ptr<double>(k);
    double *psum  = Cparticle::sum_prob.ptr<double>(0);
    for(int i = 0; i < m_logmCmat.size() ; ++i, ++pdis, ++pprob, ++psum){
        *pdis   = norm(m_logmCmat[i] - tarpar.m_logmCmat[i]);
        *pprob  = 1/(*pdis);
        *psum  += *pprob;
    }
}
/***********************************************************/
void Cparticle::NormProb(){
    for (int i = 0; i < Cparticle::par_prob.rows; ++i){
        double *pprob = Cparticle::par_prob.ptr<double>(i);
        double *psum  = Cparticle::sum_prob.ptr<double>(0);
        for (int j = 0; j < Cparticle::par_prob.cols; ++j, ++pprob, ++psum){
            *pprob /= *psum;
        }
    }
}
/***********************************************************/
void Cparticle::updateStddev(const Parameter &para){
    double *ptarpos = m_pos.ptr<double>(0);
    double width    = *(ptarpos+2) - *ptarpos;
    double height   = *(ptarpos+3) - *(ptarpos+1);
    Cparticle::stddevm = (Mat_<double>(1,4)<<para.std_x, para.std_y, 
        para.std_gain_w*width/3 , para.std_gain_h*height/3);
}
/***********************************************************/
void Cparticle::updateModel(CovImage &covimg,Parameter &para, int frameNo){
    if(para.updateFreq != 0 && para.currentMode == 0 && frameNo % para.updateFreq == 0){
        calccovmat(covimg,para);
        logm();
        m_tmplib.pop_front();
        m_tmplib.push_back(m_logmCmat);
        for(int i = 0 ; i < para.nModes ; ++i){
            m_tmplib[para.templateNo-1][i] = m_logmCmat[i].clone();
        }
        cerr<<"Model updated!"<<endl;
    } 
}
/***********************************************************/
void Cparticle::GenParticlePostion(const Parameter &para){
    double* pstddev = Cparticle::stddevm.ptr<double>(0);
    double* ptarpos = m_pos.ptr<double>(0);
    Mat tarpos_wh   = Mat::zeros(1,4,CV_64F);
    double* ptarpos_wh  = tarpos_wh.ptr<double>(0);
    *(ptarpos_wh)       = *(ptarpos);
    *(ptarpos_wh + 1)   = *(ptarpos + 1);
    *(ptarpos_wh + 2)   = *(ptarpos + 2) - *(ptarpos);
    *(ptarpos_wh + 3)   = *(ptarpos + 3) - *(ptarpos + 1);
    
    Mat ParPos_wh       = Mat(para.nParticles,4,CV_64F); 
    Cparticle::par_pos  = Mat(para.nParticles,4,CV_64F); 

    for(int i = 0; i < 4; ++i, ++ptarpos_wh, ++pstddev){
        theRNG().state = getTickCount();
        randn(ParPos_wh.col(i),(double)(*ptarpos_wh),(double)(*pstddev));
    }
    Cparticle::par_pos.col(0) = ParPos_wh.col(0) * 1.0;
    Cparticle::par_pos.col(1) = ParPos_wh.col(1) * 1.0;
    Cparticle::par_pos.col(2) = ParPos_wh.col(2) + ParPos_wh.col(0);
    Cparticle::par_pos.col(3) = ParPos_wh.col(3) + ParPos_wh.col(1);
}
/***********************************************************/
void Cparticle::ResampleParticle(Parameter &para){
    //update standard deviation
    updateStddev(para);
    //sort probability
   
    NormProb();
    
    Mat current_prob = Cparticle::par_prob.col(para.currentMode).clone();
    Mat current_prob_idx;
    cv::sortIdx(current_prob,
        current_prob_idx,
        CV_SORT_DESCENDING + CV_SORT_EVERY_COLUMN);
    //resampling
    Vector<int> CountPar(Cparticle::par_prob.rows);
    
    for (int i = 0; i < CountPar.size(); ++i){
        CountPar[i] = ceil(current_prob.at<double>(i) * para.nParticles); 
     
    }

    
    Mat ParPos_wh       = Mat::zeros(para.nParticles,4,CV_64F); 
    for(int i = 0, k = 0; i < para.nParticles ; ++k){
        int par_no = current_prob_idx.at<__int32>(k);
        int count = CountPar[par_no];
        for(int j = 0 ; j < count && i < para.nParticles; ++j, ++i){
            double *pParPosTmp = ParPos_wh.ptr<double>(i);
            double *pParPos    = Cparticle::par_pos.ptr<double>(par_no);
            *pParPosTmp = *pParPos;
            *(pParPosTmp+1) = *(pParPos+1);
            *(pParPosTmp+2) = *(pParPos+2) - *pParPos;
            *(pParPosTmp+3) = *(pParPos+3) - *(pParPos+1);
        }
    }
    double* pstddev = Cparticle::stddevm.ptr<double>(0);
    for(int i = 0; i < 4; ++i, ++pstddev){
        Mat noise = Mat::zeros(ParPos_wh.rows,1,CV_64F);
        theRNG().state = getTickCount();
        randn(noise,0,(double)(*pstddev));
        ParPos_wh.col(i) += noise;
    }

    Cparticle::par_pos = Mat::zeros(ParPos_wh.rows,ParPos_wh.cols,CV_64F);
    Cparticle::par_pos.col(0) = ParPos_wh.col(0) * 1.0;
    Cparticle::par_pos.col(1) = ParPos_wh.col(1) * 1.0;
    Cparticle::par_pos.col(2) = ParPos_wh.col(2) + ParPos_wh.col(0);
    Cparticle::par_pos.col(3) = ParPos_wh.col(3) + ParPos_wh.col(1);  
}


