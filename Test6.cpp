/* A small test program.
*
* Chenghuan , Du Huynh, Jan 2017.
*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <time.h>
#include <omp.h>

#include "covImage.h"
#include "debug.h"
#include "utils.h"

#include <sstream>
#include <string>
#include <fstream>
#include "Cparticle.h"
using namespace std;
using namespace cv;

void usage()
{
    cerr << "Usage:\n";
    cerr << "covTracker [-i imprefix] [-s startFrame] [-e endFrame] [-b x1 y1 x2 y2] [-sigma value]\n";
    cerr << "  imprefix should be the image file name prefix (default='car4')\n";
    cerr << "  startFrame should be the starting frame number (default=1)\n";
    cerr << "  endframe should be the ending frame number (default=100)\n";
    cerr << "  x1,y1 should be the coordinates of the top-left corner of the target in startFrame (default=71 177)\n";
    cerr << "  x2,y2 should be the coordinates of the bottom-right corner of the target in startFrame (default=51 138)\n";
    cerr << "  value should be the standard deviation for particle sampling (default=???)\n\n";
    cerr << "Example: To track frames 1 to 100 of the car4 sequence with\n";
    cerr << "         bounding box given by (x1,y1) (top-left) and (x2,y2) (bottom-right)\n";
    cerr << "   covTracker -i car4 -s 1 -e 300 -p 100 -t 3 -b 70 51 177 138\n";
    exit(1);
};
/*
    -i basketball -s 1 -e 300 -p 100 -t 4 -b  198  214  232 295
    -i Girl       -s 1 -e 150 -p 300 -t 1 -b  57   21   88  66  
    -i car4       -s 1 -e 300 -p 600 -t 1 -b  70   51   177 138
    -i Tiger1     -s 1 -e 100 -p 100 -t 10 -b 232  88   308 172

    -i Woman      -s 1 -e 300 -p 200 -t 3 -b  213  121  234 216 
    -i Skating    -s 1 -e 300 -p 100 -t 3 -b  592  133  675 340 
    -i Spiderman  -s 1 -e 300 -p 100 -t 3 -b  527  86   663 471 
    -i surfing    -s 1 -e 282 -p 100 -t 3 -b  40   64   59  111  -d 1 
    -i girl_vot   -s 1 -e 350 -p 100 -t 2 -b  328  306  293 133  -d 1 

    -i FaceOcc1   -s 1 -e 892 -p 100 -t 2 -b  118  69   232 231
    -i FaceOcc2   -s 1 -e 812 -p 300 -t 2 -b  118  57   200 155 
    -i David3     -s 1 -e 252 -p 100 -t 5 -b  83   200  118 331

    if the particle is out of frame there will be a error
*/
int main( int argc, char** argv )
{
    // default: track "car4" with the following bounding box from frames 1 to 100
    // default: 600 particles standard deviation is 0.1
    // default: 9 modes
    string file = "car4";
    int x1=70, y1=51, x2=177, y2=138;
    int startFrame = 1, endFrame = 100;
    int nParticles = 600;
    int nModes     = 9;
    int stddev = 1;
    int dataset = 0; // 0 four digits   1 eight digits
    // extract parameters from command line arguments
    for (int i=1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'i')
                if (++i < argc) file = string(argv[i]);
                else usage();
            else if (argv[i][1] == 's')
                if (++i < argc) startFrame = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 'e')
                if (++i < argc) endFrame = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 'p')
                if (++i < argc) nParticles = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 't')
                if (++i < argc) stddev = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 'm')
                if (++i < argc) nModes = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 'd')
                if (++i < argc) dataset = atoi(argv[i]);
                else usage();
            else if (argv[i][1] == 'b') {
                if (++i < argc) x1 = atoi(argv[i]);
                else usage();
                if (++i < argc) y1 = atoi(argv[i]);
                else usage();
                if (++i < argc) x2 = atoi(argv[i]);
                else usage();
                if (++i < argc) y2 = atoi(argv[i]);
                else usage();
            }
        }
        else usage();
    }
    //generate all the file name 
    int framelength = endFrame - startFrame + 1;
    string *filename = new string [framelength];
    utils::GenImgName(filename,file,framelength,dataset);
    //Initialization
    Mat initpos = (Mat_<double>(1,4)<<x1,y1,x2,y2);
    Cparticle::stddevm = (Mat_<double>(1,4)<<stddev,stddev,0.01*stddev,0.01*stddev);
    cout<<Cparticle::stddevm<<endl;
    vector<vector<int>> v = utils::CovmatQuadrantRef(nModes);
    vector<vector<int>> v_draw = utils::ModeQuadrantRef(nModes);
    //default: template generated from frame[startFrame-1]
    //create target
    CovImage covimg(filename[startFrame-1]);
    Cparticle tarpar(covimg,initpos,v);
    //write results to file
    ofstream presults;
    presults.open(file+"_results.txt");
    presults<<file<<" "<<dataset<<endl;
    //tracking start
    for(int i = startFrame; i < endFrame ; i++){
        //load new frame 
        covimg.im = imread(filename[i],-1);
 
        cout<<filename[i]<<endl;
        covimg.process(); 
        //search
        int mode = 0;
        Mat final_pos = utils::SearchParticle(covimg,tarpar,nParticles,nModes,v,mode);
        //position update
        tarpar.m_pos = final_pos.clone();
        //show results 
        utils::ShowResults(covimg,filename[i],final_pos,file,nParticles ,mode, v_draw);
        //write results to file
        presults<<i+1<<" "<<mode+1<<" "<<tarpar.m_pos<<endl;
    }
    presults.close();
    system("pause");
}
