#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <csignal>
#include <ctime>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

bool terminationrequested;

inline string IntToStr(unsigned int x)
{
  string result;
  ostringstream convert;
  convert << x;
  result = convert.str();
  return result;
}

void sig_term(int param)
{
    terminationrequested = true;
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        cerr << "Usage: " << argv[0] << " cameraid width height interval outputfilesprefix" << endl;
        cerr << "Parameters:" << endl;
        cerr << "cameraid - Cam to use, counting from 0 or -1 for first cam that is found." << endl; 
        cerr << "width - Width of the image" << endl;
        cerr << "height - Height of the image" << endl;
        cerr << "interval - Interval in seconds at which images should be saved" << endl;
        cerr << "outputfilesprefix - Prefix for the output files" << endl;
        cerr << "Output files will be created with the format PREFIX-TIMESTAMP.JPG" << endl;
        return 1;
    }

    cout << "OpenCV Partycam, version 2 by darkkilla" << endl;

    string outputprefix = string(argv[5]);
    int interval = atoi(argv[4]);
    terminationrequested = false;
    time_t curTime = time(NULL);
    time_t lastSnapshot = time(NULL);

    // Setup signal handlers
    void (*prev_fn)(int);
    prev_fn = signal(SIGTERM, sig_term);
    if (prev_fn == SIG_IGN) signal(SIGTERM, SIG_IGN);
    prev_fn = signal(SIGABRT, sig_term);
    if (prev_fn == SIG_IGN) signal(SIGABRT, SIG_IGN);
    prev_fn = signal(SIGINT, sig_term);
    if (prev_fn == SIG_IGN) signal(SIGINT, SIG_IGN);

    cout << "Initializing camera...";

    // Create a new movie capture object.
    VideoCapture input1(atoi(argv[1]));

    if (!input1.isOpened())
    {
        cerr << endl << "Error: Can't open camera!" << endl;
        return 1;
    }
    cout << "done." << endl;

    input1.set(CV_CAP_PROP_FRAME_WIDTH, atof(argv[2]));
    input1.set(CV_CAP_PROP_FRAME_HEIGHT, atof(argv[3]));

    cout << "Creating window...";
    Mat colourImage1;
    string jpegfilename1;

    namedWindow("Partycam 2.0");
    cout << "done." << endl;
    cout << "Entering main loop..." << endl;

    while (!terminationrequested)
    {
       if (!input1.grab())
       {
         cerr << "Error: Cannot grab frame from video capture device... exiting." << endl;
         break;
       }

       input1.retrieve(colourImage1);
       imshow("Partycam 2.0", colourImage1);

       curTime = time(NULL);
       if (curTime - lastSnapshot >= interval)
       {
         jpegfilename1 = outputprefix + "-" + IntToStr((unsigned int) curTime) + ".jpg";
         imwrite(jpegfilename1, colourImage1);
         lastSnapshot = time(NULL);
       }

       if ((char) waitKey(33) == 'q') break;
    }

    // End
    cout << "Termination requested, exiting..." << endl;

    // Destroy the image, capture and window
    destroyAllWindows();
    return 0;
}
