/* A small test program.
*
* Chenghuan , Du Huynh, Jan 2017.
*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <time.h>

#include <sstream>
#include <string>

#include <omp.h>

#include "covImage.h"
#include "debug.h"
#include "utils.h"

#include <fstream>
#include "Cparticle.h"
using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
    //This program test the covariance matrices computation of ROI in nine modes
    //color1 is a 100*100*3 image, this is generated by matlab::randn(100,100,3)
    //we can also use any other color image to test.
    // top-left      (9,14)  ((10,15) in Matlab)  
    // bottom-right  (89,94) ((90,95) in Matlab)

    if (argc != 2) {
        cerr << "Usage: Test imageFileName" << endl;
        cerr << "Example:  Test5 color1.jpg" << endl;
        exit(1);
    }
    cout<<"filename = " << argv[1]<<endl;
    string filename = string(argv[1]);
    CovImage cim(filename);
    
    Cparticle cpa;
    double pos[4] = {9,14,89,94};
    cpa.m_pos = Mat(1,4,CV_64F,pos);
    cout<<cpa.m_pos<<endl;
    cpa.calc9covmat(cim);
    

    ofstream pCresults;
    pCresults.open("Cresults.txt");
    for(int i = 0; i < 9; i++){
        pCresults<< "Covariance matrix for mode "<< i+1<<" is:"<<endl;
        pCresults<<cpa.m_cmat[i]<<endl;
    }
   pCresults.close();

    system("pause");
}
