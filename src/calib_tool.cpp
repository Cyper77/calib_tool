// Program illustrate single camera calibration
// Author: Samarth Manoj Brahmbhatt, Georgia Tech

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <boost/filesystem.hpp>
#include "Config.h"

using namespace cv;
using namespace std;
using namespace boost::filesystem;

class calibrator {
    private:
        string path; //path of folder containing chessboard images
        vector<Mat> images; //chessboard images
        Mat cameraMatrix, distCoeffs; //camera matrix and distortion coefficients
        bool show_chess_corners; //visualize the extracted chessboard corners?
        float side_length; //side length of a chessboard square in mm
        int width, height; //number of internal corners of the chessboard along width and height
        vector<vector<Point2f> > image_points; //2D image points
        vector<vector<Point3f> > object_points; //3D object points

    public:
        calibrator(string, float, int, int); //constructor, reads in the images
        void calibrate(); //function to calibrate the camera
        Mat get_cameraMatrix(); //access the camera matrix
        Mat get_distCoeffs(); //access the distortion coefficients
        void calc_image_points(bool); //calculate internal corners of the chessboard image
};

calibrator::calibrator(string _path, float _side_length, int _width, int _height) {
    side_length = _side_length;
    width = _width;
    height = _height;

    path = _path;
    cout << path << endl;

    // Read images    
    for(directory_iterator i(path), end_iter; i != end_iter; i++) {
        string filename = path + i->path().filename().string();
        images.push_back(imread(filename));
    }
}

void calibrator::calc_image_points(bool show) {
    // Calculate the object points in the object co-ordinate system (origin at top left corner)
    vector<Point3f> ob_p;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            ob_p.push_back(Point3f(j * side_length, i * side_length, 0.f));
        }
    }

    if(show) namedWindow("Chessboard corners");

    for(int i = 0; i < images.size(); i++) {
        Mat im = images[i];
        vector<Point2f> im_p;
        //find corners in the chessboard image
        bool pattern_found = findChessboardCorners(im, Size(width, height), im_p, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK);
        if(pattern_found) {
            object_points.push_back(ob_p);
            Mat gray;
            cvtColor(im, gray, CV_BGR2GRAY);
            cornerSubPix(gray, im_p, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            image_points.push_back(im_p);
            if(show) {
                Mat im_show = im.clone();
                drawChessboardCorners(im_show, Size(width, height), im_p, true);
                imshow("Chessboard corners", im_show);
                while(char(waitKey(1)) != ' ') {}
            }
        }
        //if a valid pattern was not found, delete the entry from vector of images
        else images.erase(images.begin() + i);
    }
}

void calibrator::calibrate() {
    vector<Mat> rvecs, tvecs;
    cameraMatrix = initCameraMatrix2D(object_points, image_points, images[0].size(), 0);
    distCoeffs = Mat::zeros(1, 5, CV_32F);
    float rms_error = calibrateCamera(object_points, image_points, images[0].size(), cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_ZERO_TANGENT_DIST | CV_CALIB_FIX_ASPECT_RATIO);
    // cout << "RMS reprojection error " << rms_error << endl;
    rms_error = calibrateCamera(object_points, image_points, images[0].size(), cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS);
    cout << "RMS reprojection error " << rms_error << endl;
}

Mat calibrator::get_cameraMatrix() {
    return cameraMatrix;
}

Mat calibrator::get_distCoeffs() {
    return distCoeffs;
}

int main() {
    calibrator calib(IMAGE_FOLDER, 22.5, 6, 8);
    
    calib.calc_image_points(true);
    cout << "Caibrating camera..." << endl;
    calib.calibrate();
    //save the calibration for future use
    Mat intrinsics, distortion;
    calib.get_cameraMatrix().convertTo(intrinsics, CV_32F);
    calib.get_distCoeffs().convertTo(distortion, CV_32F);
    string filename_i = DATA_FOLDER + string("/Intrinsics.xml"), filename_d = DATA_FOLDER + string("/Distortion.xml");
    FileStorage fs_i(filename_i, FileStorage::WRITE), fs_d(filename_d, FileStorage::WRITE);
    fs_i << "Intrinsics" << intrinsics;
    fs_d << "Distortion" << distortion;
    fs_i.release();
    fs_d.release();
    cout << "Saved calibration matrices." << endl;

    return 0;
}
