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

using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
    // To test the covImage class on a colour or greyscale image in jpg or png
    // format.  Using version 1 of the CovImage constructor.

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
    cerr << "Using the 3rd constructor.\n";
    cerr << "The input should be a feature file name\n";
    if (argc != 2) {
        cerr << "Usage: Test featImageFileName" << endl;
        cerr << "Example:  Test featimageData1.txt" << endl;
        exit(1);
    }
    string featfilename = string(argv[1]);
    Mat fim = debug::readFeatureImage(featfilename);

    CovImage cim(fim); 

    double Npixels;
    Mat IIprod, IIsum;
    // compute the component matrices and covariance matrix of the entire
    // image
    cerr << "Test 1: (0.0, 0.0) - (" << fim.cols-1 << ", "
         << fim.rows-1 << ")\n";
    cerr << "fim.rows = " << fim.rows << ", fim.cols = " << fim.cols << endl;
    cim.covComponentMatrices(0.0, 0.0,
                             (double)(fim.cols-1), (double)(fim.rows-1), 
                             IIprod, IIsum, Npixels);
    cerr << "Npixels = " << Npixels << "\n";
    cerr << "IIprod = \n";
    debug::printDoubleMat(IIprod,0);
    cerr << "IIsum = \n";
    debug::printDoubleMat(IIsum,0);

    Mat C = cim.covMatrix(0.0, 0.0, 8.0, 7.0, Npixels);
    cerr << "covariance matrix = \n";
    debug::printDoubleMat(C, 0);

    // compare the output above with the output from Matlab

    // compute the component matrices and covariance matrix of a small patch
    double x1 = 2.0, y1 = 1.0, x2 = 7.0, y2 = 7.0;
    cerr << "\n\nTest 2: (2.0, 1.0) - (7.0, 7.0)\n";
    cim.covComponentMatrices(x1, y1, x2, y2, IIprod, IIsum, Npixels);

    cerr << "Npixels = " << Npixels << "\n";
    cerr << "IIprod = \n";
    debug::printDoubleMat(IIprod,0);
    cerr << "IIsum = \n";
    debug::printDoubleMat(IIsum,0);

    C = cim.covMatrix(x1, y1, x2, y2, Npixels);
    cerr << "covariance matrix = \n";
    debug::printDoubleMat(C,0);

    cerr << "\n\nTest 3: (1.8, 2.3) - (6.6, 6.8)\n";
    C = cim.covMatrix(1.8, 2.3, 6.6, 6.8, Npixels);
    cerr << "covariance matrix at (1.8,2.3) - (6.6,6.8) =\n";
    debug::printDoubleMat(C,0);
    cerr << endl;

    C = cim.covMatrix(1.0, 2.0, 6.0, 6.0, Npixels);
    cerr << "covariance matrix at (1,2) - (6,6) =\n";
    debug::printDoubleMat(C,0);
    cerr << endl;

    C = cim.covMatrix(2.0, 3.0, 7.0, 7.0, Npixels);
    cerr << "covariance matrix at (2,3) - (7,7) =\n";
    debug::printDoubleMat(C,0);
    cerr << endl;


    system("pause");
}
