#ifndef __COV_PARAMETER_H__
#define __COV_PARAMETER_H__

#include <fstream>

struct Parameter{
    string file;
    string route;
    string fext;

    int templateNo;
    int startFrame,endFrame;
    int nParticles;
    int nModes;
    int dataset;

    double std_x;
    double std_y;
    double std_gain_w;
    double std_gain_h;
    double diffratio;

    int framelength;

    vector<vector<int>>    v9;
    vector<vector<int>>    v3;
    vector<vector<int>>    v_draw9;
    vector<vector<int>>    v_draw3;
    Mat                    tran_matrix9;
    Mat                    tran_matrix3;

    int previousMode;
    int currentMode;

    int updateFreq;

};



#endif