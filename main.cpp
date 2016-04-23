
/**
 *  Project     : Calibration
 *  Author      : Iñigo Cirauqui Viloria
 *  Version     : v1.0
 *  Copyright   : Your copyright notice
 *  Description : Get Camera matrix and distortion coefficients from image sequence.
 *  ===========================================================================================
 */


#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



using namespace std;
using namespace cv;

void Settings(int &sqH, int &sqV, int &nCornersH, int &nCornersV, int &opt);
bool LoadFrames(Size &imageSize, String &path, vector<Mat> &inputFrames);
vector<Point2f> GetCorners(Mat &image, bool &ch, int cH, int cV);
void DisplayImages(vector<Mat> &selectedFrames, const int &imageDelay);
void PrintAndSave(Mat &K, Mat &DC);



int main () {

    cout << " - CALIBRATION - " << endl << endl;

    Size imageSize;
    const int imageDelay = 200;    // time to show the image

    int opt = 0;
    int sqH, sqV, nCornersH, nCornersV;
    Settings (sqH, sqV, nCornersH, nCornersV, opt);

    String path = "inputFrames/*";
    vector<Mat> inputFrames;
    bool Loading = LoadFrames(imageSize, path, inputFrames);
    if (!Loading) return -1;


    cout << " Processing frames..." << endl;


    vector<vector<Point2f> > corners;
    vector<Mat> selectedFrames;
    bool check;
    for (int i=0; i<inputFrames.size(); i++) {
        vector<Point2f> cornerCheck;
        cornerCheck = GetCorners(inputFrames[i],check, nCornersH, nCornersV);
        if (check) {
            corners.push_back(cornerCheck);
            selectedFrames.push_back(inputFrames[i]);
        }
        else
            continue;
    }

    // Set vector with the physical position of the corners (planar, z=0)
    int nFrames = selectedFrames.size();
    int nSquares = nCornersH * nCornersV;
    vector<Point3f> obj;
    for(int i=0; i<nSquares; i++)
        obj.push_back(Point3f(i/nCornersH, i%nCornersH, 0.0f));

    vector<vector<Point3f> > cornersBoard;
    for(int i=0; i<nFrames; i++)
        cornersBoard.push_back(obj);

    // Print corners over images
    DisplayImages(selectedFrames, imageDelay);


    // Define storage for calibration and calibrate
    Mat cameraMatrix = Mat::eye(3,3,CV_64F);
    Mat distCoeffs = Mat::zeros(8,1,CV_64F);
    vector<Mat> rvecs, tvecs;

    if (opt = 0)      calibrateCamera(cornersBoard, corners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    else if (opt = 1) calibrateCamera(cornersBoard, corners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_K3);
    else if (opt = 2) calibrateCamera(cornersBoard, corners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_ZERO_TANGENT_DIST);
    else if (opt = 3) calibrateCamera(cornersBoard, corners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_K3 + CV_CALIB_ZERO_TANGENT_DIST);
    else cout << " Something's wrong... ";

    cout << " Complete!"
         << endl << endl << endl;

    // Print results on screen and save to a file
    PrintAndSave(cameraMatrix, distCoeffs);

    return 0;
}


void Settings(int &sqH, int &sqV, int &nCornersH, int &nCornersV, int &opt) {
    cout << " Type pattern dimensions" << endl
         << "     Horizontal squares: "; cin >> sqH;
    cout << "     Vertical squares:   "; cin >> sqV;
    cout << " ---------------------------------------------"
         << endl << endl
         << " Choose calibration output (default 5 distortion coefficients " << endl
         << endl
         << "   0 - Default " << endl
         << "   1 - k3 = 0 " << endl
         << "   2 - p1 & p2 = 0 " << endl
         << "   3 - k3 & p1 & p2 = 0 " << endl
         << endl
         << " Option: "; cin >> opt;

         while ((opt<0) || (opt>3)) {
            system("clear");
            cout << endl
                 << " Invalid option " << endl << endl
                 << "   0 - Default " << endl
                 << "   1 - k3 = 0 " << endl
                 << "   2 - p1 & p2 = 0 " << endl
                 << "   3 - k3 & p1 & p2 = 0 " << endl
                 << endl
                 << " Option: "; cin >> opt;
         }

    nCornersH = sqH-1;
    nCornersV = sqV-1;

    system("clear");
    cout << endl
         << " Horizontal squares: " << sqH << endl
         << " Vertical squares:   " << sqV << endl
         << " Distorion coefficients: " << (5-opt) << " non-zero" << endl
         << endl << endl;

    cout << " Make sure you have at least 6 valid images inside inputFrames folder" << endl
         << " Then press ENTER to continue" << endl;
    cin.ignore().get();
}


bool LoadFrames(Size &imageSize, String &path, vector<Mat> &inputFrames) {
    vector<String> fn;
    glob(path,fn,true);
    if (fn.size() == 0) {
        cout << " No images found" << endl
             << " Please place at least 6 valid images inside inputFrames folder and restart the program" << endl;
        return false;
    }

    for (size_t k=0; k<fn.size(); ++k) {
        Mat im = imread(fn[k]);
        if (im.empty())
            continue;
        inputFrames.push_back(im);
        if (k==0) imageSize = im.size();
    }

    if (inputFrames.size() < 6) {
        cout << " Proper images found: " << inputFrames.size() << endl
             << " Not  enought proper images for calibration have been found inside inputImages folder" << endl
             << " Please place at least 6 valid images inside inputFrames folder and restart the program" << endl;
        return false;
    }

    return true;
}


vector<Point2f> GetCorners(Mat &image, bool &ch, int cH, int cV) {
    Mat imageGS;
    vector<Point2f> corners;

    cvtColor(image,imageGS,CV_BGR2GRAY);

    Size pattern_size = cvSize(cH,cV);

    bool patternfound = findChessboardCorners(imageGS,pattern_size,corners,
                                               CV_CALIB_CB_ADAPTIVE_THRESH +
                                               CV_CALIB_CB_FILTER_QUADS);

    if (patternfound) {
        cornerSubPix(imageGS,corners,Size(11,11),Size(-1,-1),
                     TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
        ch = true;
    }
    else {
        ch = false;
    }

    drawChessboardCorners(image,pattern_size,Mat(corners),patternfound);
    return corners;
}


void DisplayImages(vector<Mat> &selectedFrames, const int &imageDelay) {
    namedWindow("Frame",CV_WINDOW_AUTOSIZE);
    for (unsigned int i=0; i<(selectedFrames.size()); i++) {
        imshow("Frame",selectedFrames[i]);
        waitKey(imageDelay);
    }
    destroyWindow("Frame");
}


void PrintAndSave(Mat &K, Mat &DC) {
    cout << "Calibration result: " << endl << endl;
    cout << " CAMERA MATRIX " << endl
         << " " << K << endl << endl;
    cout << " DISTORTION COEFFICIENTS " << endl
         << " " << DC << endl
         << endl
         << endl;

    char question;
    cout << " Save results [y/n]: "; cin >> question;
    if (question == 'y') {
        ofstream results;
        results.open("Results.txt");
        results << endl
                << " - CALIBRATION RESULTS - "
                << endl
                << endl
                << " Camera matrix"
                << endl
                << " " << K
                << endl
                << endl
                << " Distortion coefficients"
                << endl
                << " " << DC
                << endl;
        results.close();
    }
    else
        return;
}
