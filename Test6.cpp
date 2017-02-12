/* A small test program.

Chenghuan Liu , Du Huynh, Jan 2017.

*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <time.h>
#include <sstream>
#include <string>
#include <fstream>
#include <omp.h>

#include "covImage.h"
#include "debug.h"
#include "utils.h"
#include "Cparticle.h"
#include "SParater.h"


using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
    //load parameters from config.ini
    Parameter para;
    utils::InitPara(para);

    //initialization
    vector<string> filename(para.framelength);
    utils::GenImgName(filename,para);

    //---------------------------//
//     for(int i = 0; i < para.framelength ; i++){
//         Mat img = imread(filename[i],-1);
//         double *p =  pos_gt.ptr<double>(i);
//         Point a = Point(*p,*(p+1));
//         Point b = Point(*(p+2),*(p+3));
//         rectangle(img,a,b,Scalar(0,0,0));
//         imshow("flag",img);
//         waitKey(100);
//     }
    //---------------------------//

    //create target
    CovImage covimg(filename[para.startFrame-1]);
    Cparticle tarpar(covimg,para);

    //write results to file
    ofstream presults;
    presults.open(".//results//" + para.file + ".txt");
    presults<<para.file<<" "<<para.dataset<<endl;

    //load ground truth of position
    Mat pos_gt = utils::load_pos_gt(para);

    //tracking start
    for(int i = para.startFrame; i < para.endFrame ; i++){
        //load new frame 
        covimg.im = imread(filename[i],-1);
        covimg.process(); 
        cout<<filename[i]<<endl;

        //search
        Mat final_pos = utils::SearchParticle(covimg,tarpar,para,pos_gt.row(i));

        //position update
         tarpar.m_pos = final_pos.clone();
        
        //show results 
        utils::ShowResults(covimg,filename[i],tarpar.m_pos, para);

        //write results to file
        presults<<i+1<<" "<<para.currentMode+1<<" "<<tarpar.m_pos<<endl;
    }
    presults.close();
}
