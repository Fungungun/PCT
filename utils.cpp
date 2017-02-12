/** File name: utils.cpp
*
* This file contains some small useful utility functions.
*
* Authors: Chenghuan Liu, Du Huynh
*/

#include "utils.h"
#include <fstream>

void utils::InitPara(Parameter &para){
    INIReader reader("config.ini");
    if (reader.ParseError() < 0){
        cerr<<"Config Failed!"<<endl;
    }

    para.file             = reader.Get("video_test","file","UNKNOWN");
    para.fext             = reader.Get("video_test","fext","UNKNOWN");
    para.route            = reader.Get("video_route","route","UNKNOWN");

    para.x1               = reader.GetInteger(para.file,"x1",0);
    para.y1               = reader.GetInteger(para.file,"y1",0);
    para.x2               = reader.GetInteger(para.file,"x2",0);
    para.y2               = reader.GetInteger(para.file,"y2",0);
    para.startFrame       = reader.GetInteger(para.file,"startframe",1);
    para.endFrame         = reader.GetInteger(para.file,"endframe",100);
    para.nParticles       = reader.GetInteger(para.file,"nParticles",100);
    para.nModes           = reader.GetInteger(para.file,"nModes",9);
    para.dataset          = reader.GetInteger(para.file,"dataset",0);
    para.gt_sep           = reader.GetInteger(para.file,"gt_sep",0);

    para.std_x            = reader.GetReal(para.file,"std_x",1);
    para.std_y            = reader.GetReal(para.file,"std_y",1);
    para.std_gain_w       = reader.GetReal(para.file,"std_gain_w",0.1);
    para.std_gain_h       = reader.GetReal(para.file,"std_gain_h",0.1);
    para.diffratio        = reader.GetReal(para.file,"diffratio",0.1);

    para.framelength      = para.endFrame - para.startFrame + 1;

    para.v                = utils::CovmatQuadrantRef(para.nModes);
    para.v_draw           = utils::ModeQuadrantRef(para.nModes);
    para.tran_matrix      = utils::InitModeTranMat(para.nModes);

    para.previousMode     = 0;
    para.currentMode      = 0;
}

/* ------------------------------------------------------------ */

Mat utils::InitModeTranMat(int nModes){
    /*

     1  2  3  4  5  6  7  8  9              1   2  3
    1                                      1 
    2                                      2
    3                                      3
    4
    5
    6
    7
    8
    9
    */  
    if(nModes == 9){
        Mat tran_matrix = (Mat_<double>(nModes,nModes)<<0.2 ,0.1 ,0.1 ,0.1 ,0.1 ,0.1 ,0.1 ,0.1 ,0.1 ,
                                                        0.25,0.25,0   ,0   ,0   ,0.25,0   ,0.25,0   ,
                                                        0.25,0   ,0.25,0   ,0   ,0   ,0.25,0.25,0   ,
                                                        0.25,0   ,0   ,0.25,0   ,0.25,0   ,0   ,0.25,
                                                        0.25,0   ,0   ,0   ,0.25,0   ,0.25,0   ,0.25,
                                                        0.4 ,0.1 ,0   ,0.1 ,0   ,0.4 ,0   ,0   ,0   ,
                                                        0.4 ,0   ,0.1 ,0   ,0.1 ,0   ,0.4 ,0   ,0   ,
                                                        0.4 ,0.1 ,0.1 ,0   ,0   ,0   ,0   ,0.4 ,0   ,
                                                        0.4 ,0   ,0   ,0.1 ,0.1 ,0   ,0   ,0   ,0.4  );
    return tran_matrix;
    }
    
}

/* ------------------------------------------------------------ */

Mat utils::load_pos_gt(Parameter para){

    const int coorcnt = 4; //number of coordinates
    Mat pos_gt    = Mat::zeros(para.framelength, coorcnt, CV_64F);

    ifstream inf;
    inf.open(para.route + para.file + "//gt.txt", ifstream::in);

    char gt_sep;
    if(para.gt_sep == 0){
        gt_sep = ' ';
    }
    else if(para.gt_sep == 1){
        gt_sep = ',';
    }
    else if(para.gt_sep == 2){
        gt_sep = '\t';  //tab
    }

    string line;
    size_t sep, sep2;
    for(int i = 0; i < para.framelength; i++){
        double *ppos_gt = pos_gt.ptr<double>(i);
        getline(inf,line);
        sep = line.find(gt_sep,0);
        *(ppos_gt++) = atof(line.substr(0,sep).c_str()); 
        for(int j = 0; sep < line.size() && j != coorcnt - 1 ; j++){
            sep2 = line.find(gt_sep, sep + 1);
            *(ppos_gt++) = atof(line.substr(sep+1,sep2-sep-1).c_str());
            sep = sep2;
        }
    }

    pos_gt.col(2) = pos_gt.col(0) + pos_gt.col(2);
    pos_gt.col(3) = pos_gt.col(1) + pos_gt.col(3);

    return pos_gt;
}

/* ------------------------------------------------------------ */

void utils::getQuadrants(int x1, int y1, int x2, int y2,
    int *qx1, int *qy1, int *qx2, int *qy2)
{
    int xhalf = (x1+x2) / 2;
    int yhalf = (y1+y2) / 2;

    // top-left quadrant
    qx1[0] = x1;  qy1[0] = y1;  qx2[0] = xhalf;  qy2[0] = yhalf;
    // top-right quadrant
    qx1[1] = xhalf+1;  qy1[1] = y1;  qx2[1] = x2;  qy2[1] = yhalf;
    // bottom-left quadrant
    qx1[2] = x1;  qy1[2] = yhalf+1;  qx2[2] = xhalf; qy2[2] = y2;
    // bottom-right quadrant
    qx1[3] = xhalf+1;  qy1[3] = yhalf+1;  qx2[3] = x2; qy2[3] = y2;
}

/* ------------------------------------------------------------ */

void utils::getVerticalHalf(int x1, int y1, int x2, int y2,
    int *qx1, int *qy1, int *qx2, int *qy2)
{
    int yhalf = (y1+y2) / 2;

    // top half
    qx1[0] = x1;  qy1[0] = y1;  qx2[0] = x2;  qy2[0] = yhalf;
    // bottom half
    qx1[1] = x1;  qy1[1] = yhalf+1;  qx2[1] = x2;  qy2[1] = y2;
}

/* ------------------------------------------------------------ */

void utils::getHorizontalHalf(int x1, int y1, int x2, int y2,
    int *qx1, int *qy1, int *qx2, int *qy2)
{
    int xhalf = (x1+x2) / 2;

    // left half

    // right half
}

/* ------------------------------------------------------------ */

void utils::GenImgName(vector<string> &filename, Parameter para)
{
    for(int i = 0; i < para.framelength; i++){
        stringstream sstr; 
        sstr<<i+1;
        string frameNoStr;
        sstr>>frameNoStr;
        filename[i] = "00000000";
        filename[i] += (frameNoStr+para.fext);
        filename[i].assign(filename[i],filename[i].length() - (4*(para.dataset == 0)+8*(para.dataset == 1)+para.fext.length()),
            (4*(para.dataset == 0)+8*(para.dataset == 1)+para.fext.length()));
        filename[i] = para.route + para.file + "\\" + filename[i];
    }
}

/* ------------------------------------------------------------ */

Mat utils::GenParticlePostion(Mat meanm, Mat stddevm, int nParticles)
{
    double* pstddev = stddevm.ptr<double>(0);
    double* pmean = meanm.ptr<double>(0);
    Mat ParPos    = Mat(nParticles,4,CV_64F);
    Mat ParPos_wh = Mat(nParticles,4,CV_64F); 


    Mat meanm_wh = Mat::zeros(1,4,CV_64F); 
    double * pmean_wh = meanm_wh.ptr<double>(0);
    *(pmean_wh)      = *(pmean);
    *(pmean_wh + 1)  = *(pmean + 1);
    *(pmean_wh + 2)  = *(pmean + 2) - *(pmean);
    *(pmean_wh + 3)  = *(pmean + 3) - *(pmean + 1);

    for(int i = 0; i < 4; i++, pmean_wh++, pstddev++)
    {
        theRNG().state = getTickCount();
        randn(ParPos_wh.col(i),(double)(*pmean_wh),(double)(*pstddev));
    }
    ParPos.col(0) = ParPos_wh.col(0) * 1.0;
    ParPos.col(1) = ParPos_wh.col(1) * 1.0;
    ParPos.col(2) = ParPos_wh.col(2) + ParPos_wh.col(0);
    ParPos.col(3) = ParPos_wh.col(3) + ParPos_wh.col(1);

    return ParPos;
}

/* ------------------------------------------------------------ */

void utils::mode_tran(Mat &par_wt, Mat sum_wt, Parameter &para){
    
    double sum_max = 0;
    double *psum = sum_wt.ptr<double>(0);
    for(int i = 0; i < sum_wt.cols ; i++, psum++){
        if(sum_max < *psum){
            sum_max = *psum;
            para.currentMode = i;
        }
    }

    //control the best mode -- mode 1
    double diff = abs(sum_wt.at<double>(0,0) - 
        sum_wt.at<double>(0,para.currentMode))/sum_wt.at<double>(0,0);
    cout<<"diff = "<<diff<<endl;
    if( diff < para.diffratio){
        para.currentMode = 0;
    }

    //finite state machine
    if(para.currentMode != 0){
        srand(getTickCount()); double probability = (double)rand()/RAND_MAX;
        if(probability > para.tran_matrix.at<double>(para.previousMode,para.currentMode)){
            para.currentMode = para.previousMode;
        }
    }
    
    
    para.previousMode = para.currentMode;
}

/* ------------------------------------------------------------ */

vector<vector<int>> utils::CovmatQuadrantRef(int nmodes){

    vector<vector<int>> v; 
    if(nmodes == 9){
        v.resize(9);
        v[0].resize(4);v[0][0] = 0;v[0][1] = 1; v[0][2] = 2;v[0][3] = 3;
        v[1].resize(3);v[1][0] = 1;v[1][1] = 2; v[1][2] = 3;
        v[2].resize(3);v[2][0] = 0;v[2][1] = 2; v[2][2] = 3;
        v[3].resize(3);v[3][0] = 0;v[3][1] = 1; v[3][2] = 3;
        v[4].resize(3);v[4][0] = 0;v[4][1] = 1; v[4][2] = 2;
        v[5].resize(2);v[5][0] = 1;v[5][1] = 3; 
        v[6].resize(2);v[6][0] = 0;v[6][1] = 2; 
        v[7].resize(2);v[7][0] = 2;v[7][1] = 3; 
        v[8].resize(2);v[8][0] = 0;v[8][1] = 1; 
    }
    else if(nmodes == 3){}
    return v;
}

/* ------------------------------------------------------------ */

Mat utils::SearchParticle(CovImage covimg, Cparticle tarpar, Parameter &para, Mat pos_gt){

    Mat stddevm = Mat::zeros(1,4,CV_64F);
    utils::updateStddev(stddevm, para, tarpar.m_pos);
    cout<<"stddev = "<<stddevm<<endl;

    Cparticle::par_pos = utils::GenParticlePostion(tarpar.m_pos, stddevm, para.nParticles);
    Cparticle::par_weight = Mat::zeros(para.nParticles, para.nModes,CV_64F);

    Mat sum_wt    = Mat::zeros(1, para.nModes,CV_64F); 
    Mat max_wt    = Mat::zeros(1, para.nModes,CV_64F);
    Mat max_index = Mat::zeros(1, para.nModes,CV_64F);

    for(int j = 0; j < para.nParticles; j++){
        Cparticle canpar(covimg,Cparticle::par_pos.row(j),para.v);
        canpar.calcdis(tarpar);
        canpar.calcwt(Cparticle::par_weight,j);
        sum_wt += Cparticle::par_weight.row(j); 
        double *p          = Cparticle::par_weight.ptr<double>(j);
        double *pmax       = max_wt.ptr<double>(0);
        double *pmax_index = max_index.ptr<double>(0);
        for(int i = 0; i < para.nModes ; i++,p++, pmax++, pmax_index++){
            if (*pmax < *p){
                *pmax = *p;*pmax_index = j;
            }
        }
    }
    utils::mode_tran(Cparticle::par_weight, sum_wt, para);
   
    

    cout<<"mode = " <<para.currentMode+1<<endl;
    Mat final_pos = Cparticle::par_pos.row(max_index.at<double>(0,para.currentMode));
    cout<<" final_pos = "<<final_pos<<endl;
    return final_pos;
}

/* ------------------------------------------------------------ */

void utils::ShowResults(CovImage covimg, string filename ,  Mat final_pos, Parameter &para){

    stringstream ss;
    ss<<"mode :"<<para.currentMode+1;
    putText(covimg.im,ss.str(),Point(5,15),CV_FONT_NORMAL,0.7,Scalar(0,0,255)); 
    /*
    show all the particles
    */
    //     for (int i = 0; i <nParticles; i++){
    //         double *p =  Cparticle::par_pos.ptr<double>(i);
    //         Point a = Point(*p,*(p+1));
    //         Point b = Point(*(p+2),*(p+3));
    //         rectangle(covimg.im,a,b,Scalar(255,i,i));
    //     }
    /*
    show mode
    */
    vector<Point> point_draw;
    if(para.v_draw.size() == 9){
        point_draw.resize(9);
        double *pfinal_pos = final_pos.ptr<double>(0);
        double x1 = *pfinal_pos;double y1 = *(pfinal_pos+1);
        double x2 = *(pfinal_pos+2);double y2 = *(pfinal_pos+3);
        double xhalf = (x1 + x2)/2;double yhalf = (y1 + y2)/2;
        point_draw[0] = Point(x1,y1);   point_draw[1] = Point(xhalf,y1);   point_draw[2] = Point(x2,y1);
        point_draw[3] = Point(x1,yhalf);point_draw[4] = Point(xhalf,yhalf);point_draw[5] = Point(x2,yhalf);
        point_draw[6] = Point(x1,y2);   point_draw[7] = Point(xhalf,y2);   point_draw[8] = Point(x2,y2);
        for(int i = 0; i < para.v_draw[para.currentMode].size() - 1; i++){
            line(covimg.im, 
                 point_draw[para.v_draw[para.currentMode][i]],
                 point_draw[para.v_draw[para.currentMode][i+1]],
                 Scalar(255,255,255));
        }
    }
    else if(para.v_draw.size() == 3){}
    imshow(para.file,covimg.im);
    waitKey(1);
}

/*  draw mode
points definition
0------1------2
|      |      |
3------4------5     
|      |      |
6------7------8 
*/

vector<vector<int>> utils::ModeQuadrantRef(int nmodes){

    vector<vector<int>> v; 
    if(nmodes == 9){
        v.resize(9);
        v[0].resize(5);v[0][0] = 0;v[0][1] = 2; v[0][2] = 8;v[0][3] = 6;v[0][4] = 0;
        v[1].resize(7);v[1][0] = 1;v[1][1] = 2; v[1][2] = 8;v[1][3] = 6;v[1][4] = 3;v[1][5] = 4;v[1][6] = 1;
        v[2].resize(7);v[2][0] = 0;v[2][1] = 1; v[2][2] = 4;v[2][3] = 5;v[2][4] = 8;v[2][5] = 6;v[2][6] = 0;
        v[3].resize(7);v[3][0] = 0;v[3][1] = 2; v[3][2] = 8;v[3][3] = 7;v[3][4] = 4;v[3][5] = 3;v[3][6] = 0;
        v[4].resize(7);v[4][0] = 0;v[4][1] = 2; v[4][2] = 5;v[4][3] = 4;v[4][4] = 7;v[4][5] = 6;v[4][6] = 0;
        v[5].resize(5);v[5][0] = 1;v[5][1] = 2; v[5][2] = 8;v[5][3] = 7;v[5][4] = 1;
        v[6].resize(5);v[6][0] = 0;v[6][1] = 1; v[6][2] = 7;v[6][3] = 6;v[6][4] = 0;
        v[7].resize(5);v[7][0] = 3;v[7][1] = 5; v[7][2] = 8;v[7][3] = 6;v[7][4] = 3; 
        v[8].resize(5);v[8][0] = 0;v[8][1] = 2; v[8][2] = 5;v[8][3] = 3;v[8][4] = 0; 
    }
    else if(nmodes == 3){}
    return v;
}


/* ------------------------------------------------------------ */
void utils::updateStddev(Mat &stddevm, Parameter &para, Mat tarpos ){

    double *ptarpos = tarpos.ptr<double>(0);
    double width    = *(ptarpos+2) - *ptarpos;
    double height   = *(ptarpos+3) - *(ptarpos+1);
    stddevm = (Mat_<double>(1,4)<<para.std_x, para.std_y, 
        para.std_gain_w*width/3 , para.std_gain_h*height/3);

}