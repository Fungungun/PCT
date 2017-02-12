/* A small test program.
 *
 * Du Huynh, Jan 2017.
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

using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
    // To test the covImage class on a colour or greyscale image in jpg or png
    // format.
    // Using version 1 of the CovImage constructor.

    /*
    if (argc != 2) {
        cerr << "Usage: Test imageFileName" << endl;
        cerr << "Example:  Test yellowlily.jpg" << endl;
        exit(1);
    }
    string filename = string(argv[1]);
    CovImage cim(filename);
    */


    // To test the covImage class on a small text file whose contents
    // represent a colour or greyscale image.  Using version 2 of the CovImage
    // constructor.

    /*
    if (argc != 2) {
        cerr << "Usage: Test imageFileName" << endl;
        cerr << "Example:  Test data1.txt" << endl;
        exit(1);
    }
    string file = string(argv[1]);
    CovImage cim(file, 0);
    */

    // assuming that the feature image is already stored in a file, we want to
    // bypass the computation of Ix, Iy, Ixx, Iyy, etc. We just want to test
    // the integral image and covariance matrix computation. Note that the
    // file must store a 7-channel or 17-channel image of type double.
    // Using version 3 of the CovImage constructor.

    string featfilename = "featimageData1.txt";
    Mat fim = debug::readFeatureImage(featfilename);
    
    for(int i = 0; i < 7; i++){
        cout<<"ttfeat(:,:,"<<i+1<<") = [\n";
        debug::printDoubleMat(fim,i);
        cout<<"];"<<endl;    
    }
    CovImage cim(fim); 

    Mat IIprod[4];
    Mat IIsum[4];

    // Suppose that we want to compute the covariance matrix for mode 1 (the
    // entire image patch) and mode 2 (the image patch with the top-left
    // quadrant missing), we need to compute the component matrices for each
    // of the 4 quadrants separately.
    int qx1[4], qy1[4], qx2[4], qy2[4];
    int Npixels[4];

    // since we have a very small image in this example, let's assume that the
    // target to be tracked occupies the entire image.
    
    utils::getQuadrants(0, 0, fim.cols-1, fim.rows-1, qx1, qy1, qx2, qy2);

    for (int i=0; i < 4; i++) {
        cerr << "quadrant " << i << endl;
        cerr << qx1[i] << ", " << qy1[i] << ", "
             << qx2[i] << ", " << qy2[i] << endl;
        cim.covComponentMatrices(qx1[i], qy1[i], qx2[i], qy2[i],
                                 IIprod[i], IIsum[i], Npixels[i]);
        cerr << "Npixels[i] = " << Npixels[i] << endl;
        cerr << "IIprod[i] = " << endl;
        debug::printDoubleMat(IIprod[i], 0);
        cerr << "IIsum[i] = " << endl;
        debug::printDoubleMat(IIsum[i], 0);
    }

    
    vector<vector<int>> v; 
    utils::CovmatQuadrantRef(v,9);
    for(int i = 0; i < v.size() ; i++){

        Mat iiprod = Mat::zeros(IIprod[0].rows,IIprod[0].cols,CV_64F); 
        Mat iisum  =  Mat::zeros(IIsum[0].rows,IIsum[0].cols,CV_64F);
        int N      = 0;

        for(int j = 0; j < v[i].size(); j++){
            cout<<"i =  "<<i<< "  j =  "<< v[i][j] <<endl;
            iiprod += IIprod[v[i][j]];
            iisum  += IIsum[v[i][j]];
            N      += Npixels[v[i][j]];
        }
        Mat cmat = iiprod / (N-1) - iisum*iisum.t() / (N*(N-1));
        cerr << endl;
        cerr << "Covariance matrix for mode "<< i+1 <<" is:"<<endl;
        debug::printDoubleMat(cmat, 0);
    }  
    
    system("pause");
}
