#include <iostream>
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
using namespace cv;



Point detectPlus(const Mat& gray, const Mat& tpl, Rect zone, double& score)
{
    Mat result;
    matchTemplate(gray(zone), tpl, result, TM_CCOEFF_NORMED);

    Point bestLoc;
    minMaxLoc(result, nullptr, &score, nullptr, &bestLoc); 

    
    return Point(zone.x + bestLoc.x + tpl.cols / 2,
                 zone.y + bestLoc.y + tpl.rows / 2);
}

int main()
{
    Mat img = imread("../NicIcon/all-scans/00715.png");
    Mat tpl = imread("../image_plus.png", IMREAD_GRAYSCALE);
    if (img.empty() || tpl.empty()) { cout << "probleme lecture image" << endl; return 1; }

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    int W = img.cols, H = img.rows;
    Rect zoneTR(int(0.70 * W), int(0.10 * H), int(0.30 * W), int(0.08 * H));
    Rect zoneBL(0,             int(0.85 * H), int(0.20 * W), int(0.10 * H));

    double scoreTR, scoreBL;
    Point tr = detectPlus(gray, tpl, zoneTR, scoreTR);
    Point bl = detectPlus(gray, tpl, zoneBL, scoreBL);
    cout << "Top-right + : " << tr << "  score " << scoreTR << endl;
    cout << "Bottom-left +: " << bl << "  score " << scoreBL << endl;

    
    rectangle(img, zoneTR, Scalar(255, 0, 0), 3);
    rectangle(img, zoneBL, Scalar(255, 0, 0), 3);
    circle(img, tr, 25, Scalar(0, 0, 255), 4);
    circle(img, bl, 25, Scalar(0, 0, 255), 4);

    Mat small;
    resize(img, small, Size(), 0.25, 0.25);
    imshow("plus detection", small);
    waitKey(0);
    return 0;
}