/** File name: utils.cpp
*
* This file contains some small useful utility functions.
*
* Authors: Chenghuan Liu, Du Huynh
*/

#include "utils.h"
#include <fstream>

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

void utils::GenImgName(string *filename, string file,int framelength, int dataset)
{
    string fext = ".jpg";

    for(int i = 0; i < framelength; i++){
        stringstream sstr; 
        sstr<<i+1;
        string frameNoStr;
        sstr>>frameNoStr;
        filename[i] = "00000000";
        filename[i] += (frameNoStr+fext);
        filename[i].assign(filename[i],filename[i].length() - (4*(dataset == 0)+8*(dataset == 1)+fext.length()),
            (4*(dataset == 0)+8*(dataset == 1)+fext.length()));
        filename[i] = ".//" + file + "//" + filename[i];
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

void utils::normWt(Mat &par_wt, Mat sum_wt, int &finalmode){

    double sum_max = 0;
    double *psum = sum_wt.ptr<double>(0);
    for(int i = 0; i < sum_wt.cols ; i++, psum++){
        if(sum_max < *psum){
            sum_max = *psum;
            finalmode = i;
        }
    }
    for(int i = 0; i < par_wt.rows ; i++){
        double *p = par_wt.ptr<double>(i);
        double *psum = sum_wt.ptr<double>(0);
        for(int j = 0; j < par_wt.cols; j++, p++, psum++){
            *p /= *psum;
        }
    }
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

Mat utils::SearchParticle(CovImage covimg, Cparticle tarpar, int nParticles, int nModes, vector<vector<int>> v,int &mode){

    Cparticle::par_pos = utils::GenParticlePostion(tarpar.m_pos,Cparticle::stddevm,nParticles);
    Cparticle::par_weight = Mat(nParticles,nModes,CV_64F);

    Mat sum_wt = Mat::zeros(1,nModes,CV_64F); 
    Mat max_wt = Mat::zeros(1,nModes,CV_64F);
    Mat max_index = Mat::zeros(1,nModes,CV_64F);
    for(int j = 0; j < nParticles; j++){
        Cparticle canpar(covimg,Cparticle::par_pos.row(j),v);
        canpar.calcdis(tarpar);
        canpar.calcwt(Cparticle::par_weight,j);
        sum_wt += Cparticle::par_weight.row(j);
        double *p          = Cparticle::par_weight.ptr<double>(j);
        double *pmax       = max_wt.ptr<double>(0);
        double *pmax_index = max_index.ptr<double>(0);
        for(int i = 0; i < nModes ; i++,p++, pmax++, pmax_index++){
            if (*pmax < *p){

                *pmax = *p;*pmax_index = j;
            }
        }
    }
    utils::normWt(Cparticle::par_weight, sum_wt, mode);
    cout<<"mode = " <<mode+1<< " max_index = "<<max_index.at<double>(0,mode)<<"  ";
    Mat final_pos = Cparticle::par_pos.row(max_index.at<double>(0,mode));
    cout<<" final_pos = "<<final_pos<<endl;
    // Mat final_pos = Cparticle::par_weight.col(0).t() * Cparticle::par_pos;

    return final_pos;
}

/* ------------------------------------------------------------ */

void utils::ShowResults(CovImage covimg, string filename , Mat final_pos, string file, int nParticles, int mode, vector<vector<int>> v_draw){



    stringstream ss;
    //     ss<<filename;
    //     putText(covimg.im,ss.str(),Point(5,20),CV_FONT_NORMAL,0.7,Scalar(0,0,255)); 
    //     ss.str("");
    ss<<"mode :"<<mode+1;
    putText(covimg.im,ss.str(),Point(5,15),CV_FONT_NORMAL,0.7,Scalar(0,0,255)); 
    /*
    show all the particles
    */
    //     for (int i = 0; i <nParticles; i++){
    //         double *p =  Cparticle::par_pos.ptr<double>(i);
    //         Point a = Point(*p,*(p+1));
    //         Point b = Point(*(p+2),*(p+3));
    //         rectangle(covimg.im,a,b,Scalar(255,255,255));
    //     }
    /*
    show the weighted sum of particles' positions
    */
    //     double *pfinal_pos = final_pos.ptr<double>(0);
    //     Point a = Point(*pfinal_pos,*(pfinal_pos+1));
    //     Point b = Point(*(pfinal_pos+2),*(pfinal_pos+3));
    //     rectangle(covimg.im,a,b,Scalar(255,0,0));

    /*
    show mode
    */
    vector<Point> point_draw;
    if(v_draw.size() == 9){
        point_draw.resize(9);
        double *pfinal_pos = final_pos.ptr<double>(0);
        double x1 = *pfinal_pos;double y1 = *(pfinal_pos+1);
        double x2 = *(pfinal_pos+2);double y2 = *(pfinal_pos+3);
        double xhalf = (x1 + x2)/2;double yhalf = (y1 + y2)/2;
        point_draw[0] = Point(x1,y1);   point_draw[1] = Point(xhalf,y1);   point_draw[2] = Point(x2,y1);
        point_draw[3] = Point(x1,yhalf);point_draw[4] = Point(xhalf,yhalf);point_draw[5] = Point(x2,yhalf);
        point_draw[6] = Point(x1,y2);   point_draw[7] = Point(xhalf,y2);   point_draw[8] = Point(x2,y2);

        for(int i = 0; i < v_draw[mode].size() - 1; i++){
            line(covimg.im, point_draw[v_draw[mode][i]],point_draw[v_draw[mode][i+1]],Scalar(255,255,255));
        }
    }
    else if(v_draw.size() == 3){}

    imshow(file,covimg.im);
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