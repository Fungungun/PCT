#ifndef __COV_PARTICLE_H__
#define __COV_PARTICLE_H__


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

#include <assert.h>
#include "debug.h"
#include "covImage.h"
#include "SParater.h"
#include <queue>

#pragma warning(disable : 4244 4996)

class Cparticle{

public:
    /* position of all particles*/
    static Mat par_pos;
    /* store the distance between particle with target*/
    static Mat par_dis;
    /* probability */
    static Mat par_prob,sum_prob;
    /* standard deviation*/
    static Mat stddevm;
    /* position of particle. 1*4 matrix. [x1,y1,x2,y2] */
    Mat m_pos;
    /* covariance matrices */
    vector<Mat> m_cmat; 
    /* logm of covariance matrices*/
    vector<Mat> m_logmCmat;
    /* library of templates*/
    /* store the covariance matrices in nine modes
    previous para.templateNo frames*/
    deque<vector<Mat>> m_tmplib;
    /* image patch of templates*/
    vector<Mat> templatePatch;

    /* constructor 1: candidate particles*/
    Cparticle(CovImage &cim,Mat pos,Parameter& para){
        m_pos = pos.clone();
        calccovmat(cim,para);
        logm();  
    }
/* constructor 2:  initialization of the target in 1st frame*/
//     Cparticle(CovImage &cim, Parameter para){
//         int nModes = (int)(para.v.size());
//         m_pos = (Mat_<double>(1,4)<<para.x1, para.y1, para.x2, para.y2);
//         calccovmat(cim,para.v,nModes);
//         logm();
//     }

    /* constructor 3: create target with template library*/
    /*this part assume that more than one frame can be used for training which is not 
        allowed. This part needs to be modified later. Currently we only use one frame
        for training. */
    Cparticle(vector<string> filename, Parameter& para, Mat pos_gt){
        cerr<<"Begin creating target with first "<<para.startFrame-1<<" frames..."<<endl;
        m_tmplib.resize(para.templateNo);
        templatePatch.resize(para.templateNo);
        for(int i = 0; i < para.templateNo ; ++i){
            cerr<<"Processing frame "<<i+1<<"  "<<filename[i]<<endl;
            CovImage covimg_init(filename[i]);
            m_pos = pos_gt.row(i);
            templatePatch[i] = covimg_init.im_in.colRange(m_pos.at<double>(0),
                                                          m_pos.at<double>(2)).rowRange(
                                                          m_pos.at<double>(1),
                                                          m_pos.at<double>(3));
            calccovmat(covimg_init,para);
            logm();
            m_tmplib[i].resize(para.nModes);
            for(int j = 0 ; j < para.nModes ; j++){
                m_tmplib[i][j] = m_logmCmat[j].clone();
            }
        }
        cerr<<"Target Created!"<<endl;
        //draw sample
        updateStddev(para);
        GenParticlePostion(para);
    }

public:
   
    /*  calculate the covariance matrices 
    */
    void calccovmat(CovImage &cim, Parameter &para);
    /*  calculate the covariance matrices of nine modes
    */
    void calc9covmat(CovImage &cim, vector<vector<int>> &v);
    /*  calculate the covariance matrices of three modes
    */
    void calc3covmat(CovImage &cim, vector<vector<int>> &v);
    /*  same as Matlab::logm but this function has only been tested on covariance matrix
    */
    void logm();
    /* calculate the distance between particles
    */
    void ParticleProcess(Cparticle &tarpar, int k);
    /* calculate the probability
    */
    void NormProb();
    /* update model
    */
    void updateModel(CovImage &covimg,Parameter &para, int frameNo);
    /*update standard deviation
     */
    void updateStddev(const Parameter &para);
    /*draw sample
     */
    void GenParticlePostion(const Parameter &para);
    /*resampling
     */
    void ResampleParticle(Parameter &para);
    
};


#endif