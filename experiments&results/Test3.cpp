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

using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
    // To test the covImage class on a colour or greyscale image in jpg or png
    // format.
    // Using version 1 of the CovImage constructor.

    if (argc != 2) {
        cerr << "Usage: Test imageFileName" << endl;
        cerr << "Example:  Test yellowlily.jpg" << endl;
        exit(1);
    }
	cout<<argv[1]<<endl;
    string filename = string(argv[1]);
    CovImage cim(filename);

    int nRows = cim.getnRows();
    int nCols = cim.getnCols();
    int Npixels;

    Mat C = cim.covMatrix(0, 0, nCols-1, nRows-1, Npixels);
    cerr << "covariance matrix of the entire image = \n";
    debug::printDoubleMat(C, 0);
    cerr << "Npixels = " << Npixels << "\n";

    // unfortunately it is difficult to compare the covariance matrix with
    // Matlab as the image is very large.
    // I think we can assume that the code works correctly if the for a small
    // colour image the output covariance matrices are the same.

    system("pause");
}
