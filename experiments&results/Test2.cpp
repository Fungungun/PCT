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
    // To test the covImage class on a colour image.
    // Using version 1 of the CovImage constructor.

    string filename = "data3.txt";
    // a small text file that simulates a colour image (3 channels)
    CovImage cim(filename,0);
    int Npixels;
    Mat C = cim.covMatrix(0, 0, 7, 9, Npixels);
    cerr << "Note that the covariance matrix would not be correct until\n";
    cerr << "all the functions for computing Ix, Iy, etc. have been coded\n";
    cerr << "The covariance matrix of the entire image is:\n";
    debug::printDoubleMat(C,0);

    // I suggest that we print the featimage to a file, load the file
    // into Matlab, and compute the covariance matrix in Matlab.


    // ***** save the following output to a Matlab file and then run it in
    // ***** Matlab *****

    int nRows = cim.getnRows();
    int nCols = cim.getnCols();
    Mat featimage = cim.getfeatimage();

    cerr << "featimage = zeros(" << nRows << "," << nCols << ","
         << FEAT_DIM3 << ");\n";
    for (int i=0; i < FEAT_DIM3; i++) {
        cerr << "featimage(:,:," << i+1 << ") = [\n";
        debug::printDoubleMat(featimage,i);
        cerr << "];\n";
    }
    cerr << "G = reshape(featimage,[]," << FEAT_DIM3 << ");" << endl;
    cerr << "C = cov(G)\n" << endl;

	system("pause");

}
