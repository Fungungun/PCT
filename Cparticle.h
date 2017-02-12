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

class Cparticle{

public:
    /* store the position of all particles*/
    static Mat par_pos;
    /* store the weight of all particles*/
    static Mat par_weight;
    /* position of particle. 1*4 matrix. [x1,y1,x2,y2] */
    Mat m_pos;
    /* store the distance between the candidate with target*/
    Mat distance;
    /* covariance matrices */
    Mat *m_cmat; 
    /* logm of covariance matrices*/
    Mat *m_logmCmat;


    /* constructor 1*/
    Cparticle(CovImage &cim, Mat pos,vector<vector<int>> v){

        int nModes = (int)(v.size());
        m_pos = pos.clone();
        distance = Mat::zeros(1,nModes,CV_64F);
        m_cmat = new Mat[nModes];
        m_logmCmat = new Mat[nModes];
        calccovmat(cim,v,nModes);
        logm();  
    }

    /* constructor 2:  initialization of the target in 1st frame*/
    Cparticle(CovImage &cim, Parameter para){

        int nModes = (int)(para.v.size());
        m_pos = (Mat_<double>(1,4)<<para.x1, para.y1, para.x2, para.y2);
        distance = Mat::zeros(1,nModes,CV_64F);
        m_cmat = new Mat[nModes];
        m_logmCmat = new Mat[nModes];
        calccovmat(cim,para.v,nModes);
        logm();
    }

public:
    /*  calculate the covariance matrices 
    */
    void calccovmat(CovImage &cim, vector<vector<int>> v,int nModes);
    /*  calculate the covariance matrices of nine modes
    */
    void calc9covmat(CovImage &cim, vector<vector<int>> v);
    /*  calculate the covariance matrices of three modes
    */
    void calc3covmat(CovImage &cim, vector<vector<int>> v);
    /*  same as Matlab::logm but this function has only been tested on 
    covariance matrix
    */
    void logm();

    /* calculate the distance between particles*/
    void calcdis(Cparticle tarpar);

    /* calculate the weight of particle*/
    void calcwt(Mat &par_wt,int i);

};


#endif