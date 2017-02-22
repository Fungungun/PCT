
/** File name: utils.cpp
*
* Chenghuan Liu, Du Huynh
*/

#include "utils.h"
#include <fstream>


void utils::LoadVideoList(vector<string> &video_list){
    ifstream inf;
    inf.open("video_list.txt", ifstream::in);
    string line;
    while (!inf.eof()) {
        getline(inf,line);
        video_list.push_back(line);
    }
}

/* ------------------------------------------------------------ */

void utils::InitPara(Parameter &para){
    cerr<<"Loading parameters...";
    INIReader reader("config.ini");
    if (reader.ParseError() < 0){
       ERROR_OUT__;
    }
    para.fext             = reader.Get("video_type","fext","UNKNOWN");
    para.route            = reader.Get("video_route","route","UNKNOWN");

    para.startFrame       = reader.GetInteger(para.file,"startframe",1);
    para.endFrame         = reader.GetInteger(para.file,"endframe",100);
    para.nParticles       = reader.GetInteger(para.file,"nParticles",100);
    para.dataset          = reader.GetInteger(para.file,"dataset",0);
    para.gt_sep           = reader.GetInteger(para.file,"gt_sep",0);
    para.std_x            = reader.GetReal(para.file,"std_x",1);
    para.std_y            = reader.GetReal(para.file,"std_y",1);
    para.std_gain_w       = reader.GetReal(para.file,"std_gain_w",0.1);
    para.std_gain_h       = reader.GetReal(para.file,"std_gain_h",0.1);
    para.diffratio        = reader.GetReal(para.file,"diffratio",0.1);

    para.framelength      = para.endFrame - para.startFrame + 1;
    para.templateNo       = para.startFrame - 1;

    para.v9               = utils::CovmatQuadrantRef(9);
    para.v3               = utils::CovmatQuadrantRef(3);
    para.v_draw9          = utils::ModeQuadrantRef(9);
    para.v_draw3          = utils::ModeQuadrantRef(3);
    para.tran_matrix9     = utils::InitModeTranMat(9);
    para.tran_matrix3     = utils::InitModeTranMat(3);

    para.previousMode     = 0;
    para.currentMode      = 0;

    para.updateFreq       = reader.GetInteger(para.file,"updateFreq",0);    

    cerr<<"Done!";
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
    assert(nModes == 9 || nModes == 3);

    if(nModes == 9){
        Mat tran_matrix = (Mat_<double>(nModes,nModes)<<
            0.2 ,0.15,0.15,0.15,0.15,0.05,0.05,0.05,0.05,
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
    else{
        Mat tran_matrix = (Mat_<double>(nModes,nModes)<<
            0.4 ,0.3 ,0.3,
            0.5 ,0.5 ,0  ,
            0.5 ,0   ,0.5);
        return tran_matrix;
    }
}

/* ------------------------------------------------------------ */

Mat utils::LoadPosGT(Parameter para){

    cerr<<endl<<"Loading ground truth of target position...";
    const int coorcnt = 4; //number of coordinates
    Mat pos_gt    = Mat::zeros(para.endFrame, coorcnt, CV_64F);

    ifstream inf;
    inf.open(para.route + para.file + "//gt.txt", ifstream::in);
    if(inf == NULL) ERROR_OUT__;

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
    for(int i = 0; i < para.endFrame; i++){
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
    pos_gt -= 1;

    cerr<<"Done!"<<endl;
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
     qx1[0] = x1;  qy1[0] = y1;  qx2[0] = xhalf;  qy2[0] = y2;
    // right half
     qx1[0] = xhalf+1;  qy1[0] = y1;  qx2[0] = x2;  qy2[0] = y2;
}

/* ------------------------------------------------------------ */

void utils::GenImgName(vector<string> &filename, Parameter para){

    cerr<<"Generating file names of images...";
    for(int i = 0; i < para.endFrame; ++i){
        stringstream sstr; 
        sstr<<i+1;
        string frameNoStr;
        sstr>>frameNoStr;
        filename[i] = "00000000";
        filename[i] += (frameNoStr+para.fext);
        filename[i].assign(filename[i],filename[i].length() - (4*(para.dataset == 0)+8*(para.dataset == 1)+para.fext.length()),
            (4*(para.dataset == 0)+8*(para.dataset == 1)+para.fext.length()));
        filename[i] = para.route + para.file + "//" + filename[i];
    }
    cerr<<"Done!"<<endl;
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

    for(int i = 0; i < 4; ++i, ++pmean_wh, ++pstddev)
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

void utils::ModeTran(Mat sum_dis, Parameter &para){
    //finite state machine
    double *ptran_matrix = para.nModes == 9 ? 
        para.tran_matrix9.ptr<double>(para.previousMode):para.tran_matrix3.ptr<double>(para.previousMode) ;
    double *psum         = sum_dis.ptr<double>(0);
    for(int i = 0; i < sum_dis.cols; ++i, ++psum, ++ptran_matrix){
        *psum  /= (*ptran_matrix+0.001);
    }
    psum = sum_dis.ptr<double>(0);

    double sum_min = 0;
    for(int i = 0; i < sum_dis.cols ; ++i, ++psum){
        if(sum_min > *psum){
            sum_min = *psum;
            para.currentMode = i;
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
    else if(nmodes == 3){
        v.resize(3);
        v[0].resize(2);v[0][0] = 0;v[0][1] = 1;
        v[1].resize(1);v[1][0] = 0;
        v[2].resize(1);v[2][0] = 1;
    }
    return v;
}

/* ------------------------------------------------------------ */

Mat utils::SearchParticle(CovImage &covimg, Cparticle &tarpar, Parameter &para, Mat pos_gt){
//     ofstream ptestfile;
//     ptestfile.open(".//experiments&results//test_metric_dis_data.m",ios::trunc);
    //update standard deviation
    Mat stddevm = Mat::zeros(1,4,CV_64F);
    utils::updateStddev(stddevm, para, tarpar.m_pos);
    Cparticle::par_pos = utils::GenParticlePostion(tarpar.m_pos, stddevm, para.nParticles);
    Cparticle::par_dis = Mat::zeros(para.nParticles, para.nModes,CV_64F);

    Mat sum_dis  =  Mat::zeros(1, para.nModes,CV_64F); 
    Mat min_dis  =  Mat::zeros(1, para.nModes,CV_64F); 
    Mat min_index = Mat::zeros(1, para.nModes,CV_64F);

    for(int j = 0; j < para.nParticles; ++j){
        if (utils::IsParticleOutFrame(Cparticle::par_pos.row(j))) 
            continue;
        Cparticle canpar(covimg,Cparticle::par_pos.row(j),para);
        ptestfile<<"canpar_model(:,:,"<<j+1<<")="<<canpar.m_logmCmat[0]<<";"<<endl;
        canpar.calcdis(tarpar,j);
        sum_dis  += Cparticle::par_dis.row(j);
        double *p            = Cparticle::par_dis.ptr<double>(j);
        double *pmin         = min_dis.ptr<double>(0);
        double *pmin_index   = min_index.ptr<double>(0);
        for(int i = 0; i < para.nModes; ++i, ++p, ++pmin, ++pmin_index){
            if(*pmin == 0){
                *pmin = *p;
            }
            else if(*pmin > *p){
                *pmin = *p; *pmin_index = j;
            }
        }
    }
//     ptestfile<<"min_dis = "<<min_dis<<";"<<endl;
//     ptestfile<<"min_index = "<<min_index<<";"<<endl;
//     ptestfile<<"tarpar_model = "<<tarpar.m_logmCmat[0]<<";"<<endl;
//     ptestfile<<"pos_gt = "<<pos_gt<<";"<<endl;
//     ptestfile<<"par_pos = "<<Cparticle::par_pos<<";"<<endl;
//     ptestfile<<"par_dis = "<<Cparticle::par_dis<<";"<<endl;
//     ptestfile.close();
    utils::ModeTran(sum_dis, para);
    Mat final_pos = Cparticle::par_pos.row(min_index.at<double>(0,para.currentMode));
    return final_pos;
}

/* ------------------------------------------------------------ */

void utils::ShowResults(CovImage covimg, string filename ,  Mat final_pos, Parameter &para, Mat pos_gt){

    stringstream ss;
    ss<<"mode :"<<para.currentMode+1;
    putText(covimg.im_in,ss.str(),Point(5,15),CV_FONT_NORMAL,0.7,Scalar(0,0,255)); 
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
    if(para.nModes == 9){
        point_draw.resize(9);
        double *pfinal_pos = final_pos.ptr<double>(0);
        double x1 = *pfinal_pos;double y1 = *(pfinal_pos+1);
        double x2 = *(pfinal_pos+2);double y2 = *(pfinal_pos+3);
        double xhalf = (x1 + x2)/2;double yhalf = (y1 + y2)/2;
        point_draw[0] = Point(x1,y1);   point_draw[1] = Point(xhalf,y1);   point_draw[2] = Point(x2,y1);
        point_draw[3] = Point(x1,yhalf);point_draw[4] = Point(xhalf,yhalf);point_draw[5] = Point(x2,yhalf);
        point_draw[6] = Point(x1,y2);   point_draw[7] = Point(xhalf,y2);   point_draw[8] = Point(x2,y2);
        for(int i = 0; i < para.v_draw9[para.currentMode].size() - 1; i++){
            line(covimg.im_in, 
                point_draw[para.v_draw9[para.currentMode][i]],
                point_draw[para.v_draw9[para.currentMode][i+1]],
                Scalar(255,255,255));
        }
    }
    else if(para.nModes == 3){
        point_draw.resize(6);
        double *pfinal_pos = final_pos.ptr<double>(0);
        double x1 = *pfinal_pos;double y1 = *(pfinal_pos+1);
        double x2 = *(pfinal_pos+2);double y2 = *(pfinal_pos+3);
        double yhalf = (y1 + y2)/2;
        point_draw[0] = Point(x1,y1);   point_draw[1] = Point(x2,y1);      
        point_draw[2] = Point(x1,yhalf);point_draw[3] = Point(x2,yhalf);
        point_draw[4] = Point(x1,y2);   point_draw[5] = Point(x2,y2);
        for(int i = 0; i < para.v_draw3[para.currentMode].size() - 1; i++){
            line(covimg.im_in, 
                point_draw[para.v_draw3[para.currentMode][i]],
                point_draw[para.v_draw3[para.currentMode][i+1]],
                Scalar(255,255,255));
        }
    }
    double *p =  pos_gt.ptr<double>(0);
    Point  a  = Point(*p,*(p+1));
    Point  b  = Point(*(p+2),*(p+3));
    rectangle(covimg.im_in,a,b,Scalar(0,0,0));
    double deltaX = ((pos_gt.at<double>(0) + pos_gt.at<double>(2)) 
                   -(final_pos.at<double>(0) + final_pos.at<double>(2)))/2;
    double deltaY = ((pos_gt.at<double>(1) + pos_gt.at<double>(3)) 
                   -(final_pos.at<double>(1) + final_pos.at<double>(3)))/2;
    
    //cout<<"deltaX = "<<deltaX<<"; deltaY = "<<deltaY<<endl;
    imshow(para.file,covimg.im_in);
    waitKey(1);
}

/*  draw mode
points definition
0----1----2
|    |    |
3----4----5     
|    |    |
6----7----8 

0----1
|    |      
2----3     
|    |      
4----5
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
    else if(nmodes == 3){
        //only for pedestrian
        v.resize(3);
        v[0].resize(5);v[0][0] = 0;v[0][1] = 1; v[0][2] = 5;v[0][3] = 4;v[0][4] = 0;
        v[1].resize(5);v[1][0] = 0;v[1][1] = 1; v[1][2] = 3;v[1][3] = 2;v[1][4] = 0;
        v[2].resize(5);v[2][0] = 2;v[2][1] = 3; v[2][2] = 5;v[2][3] = 4;v[2][4] = 2;
    }
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

/* ------------------------------------------------------------ */
bool utils::IsParticleOutFrame(Mat single_par_pos){
    double *p = single_par_pos.ptr<double>(0);
    for (int i = 0; i < single_par_pos.cols; ++i){
        if (*(p++) < 0) return true;
    }
    return false;
}

/* ------------------------------------------------------------ */
int utils::updateModeNum(Mat pos){
    double *p = pos.ptr<double>(0);
    double width  = *(p+2) - *p;
    double height = *(p+3) - *(p+1);
    double halfPixelNum = width * height;
    //3 times of the dimensions of covariance matrices
    //4 quadrants
    return ( (halfPixelNum < FEAT_DIM3 * 3 * 4) ? 3 : 9);
}