#ifndef __COV_PARAMETER_H__
#define __COV_PARAMETER_H__

struct Parameter{
    string file;
    string route;
    string fext;

    int x1,y1,x2,y2; //(x1,y1)--top left (x2,y2)--bottom right
    int startFrame,endFrame;
    int nParticles;
    int nModes;
    int dataset; // 0 four digits   1 eight digits
    int gt_sep; // 0 for space, 1 for comma

    double std_x;
    double std_y;
    double std_gain_w;
    double std_gain_h;
    double diffratio;

    int framelength;

    //
    vector<vector<int>>    v;
    vector<vector<int>>    v_draw;

    Mat tran_matrix;
    int previousMode;
    int currentMode;
};

#endif