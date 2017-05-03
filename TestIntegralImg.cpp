/* This program test the construction of partly integral image
  Chenghuan Liu, May 2017.
*/

#include "Test6.h"

int mainii(int argc, char** argv)
{
    Mat tarpos = (Mat_<double>(1,4)<< 69,50,176,137);
    cout<<tarpos<<endl;
    CovImage testimg(".//experiments&results//gray.jpg",20.0,tarpos);
    return 0;
}
