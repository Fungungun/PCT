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


class Cparticle{

public:
    /* store the position of all particles*/
    static Mat par_pos;
    /* store the distance between particle with target*/
    static Mat par_dis;
    /* position of particle. 1*4 matrix. [x1,y1,x2,y2] */
    Mat m_pos;
    /* store the distance between the candidate with target*/
    //Mat m_distance;
    /* covariance matrices */
    vector<Mat> m_cmat; 
    /* logm of covariance matrices*/
    vector<Mat> m_logmCmat;
    /* library of templates*/
    /* store the covariance matrices in nine modes
    previous para.templateNo frames*/
    deque<vector<Mat>> m_tmplib;

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
    Cparticle(vector<string> filename, Parameter& para, Mat pos_gt){
        cerr<<"Begin creating target with first "<<para.startFrame-1<<" frames..."<<endl;
        m_tmplib.resize(para.templateNo);
        for(int i = 0; i < para.templateNo ; ++i){
            cerr<<"Processing frame "<<i+1<<"  "<<filename[i]<<endl;
            CovImage covimg_init(filename[i]);
            m_pos = pos_gt.row(i);
            calccovmat(covimg_init,para);
            logm();

            ofstream ptestfile;
            ptestfile.close();

            m_tmplib[i].resize(para.nModes);
            for(int j = 0 ; j < para.nModes ; j++){
                m_tmplib[i][j] = m_logmCmat[j].clone();
            }
        }
        calcModel();
        cerr<<"Target Created!"<<endl;
    }

public:
    /* initialize target with template library
    */
    void calcModel();
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
    void calcdis(Cparticle &tarpar, int k);
    /* calculate the weight of particle
    */
    /*void calcwt(Mat &par_wt,int i);*/
    /* update model
    */
    void updateModel(CovImage &covimg, Parameter &para);
};


#endif