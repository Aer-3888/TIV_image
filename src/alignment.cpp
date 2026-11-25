#include "alignment.hpp"

#include <cmath>
#include "opencv2/imgproc.hpp"
#if CV_VERSION_MAJOR >= 5
#include "opencv2/geometry.hpp"   // getRotationMatrix2D moved here in OpenCV 5
#endif

using namespace cv;

static const double MIN_SCORE = 0.7;   // minimum score to accept a cross

Point2f detectCross(const Mat& gray, const Mat& templ, const Rect& zone, double& score)
{
    Mat result;
    matchTemplate(gray(zone), templ, result, TM_CCOEFF_NORMED);

    Point best;
    minMaxLoc(result, nullptr, &score, nullptr, &best);

    return Point2f(zone.x + best.x + templ.cols / 2.f,
                   zone.y + best.y + templ.rows / 2.f);
}


PageAlignment alignPage(const Mat& image, const Mat& templ)
{
    PageAlignment a;

    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // 1) Look for the crosses in the top-right quarter and the bottom-left quarter
    int W = gray.cols, H = gray.rows;
    a.crossTR = detectCross(gray, templ, Rect(W / 2, 0, W / 2, H / 2), a.scoreTR);
    a.crossBL = detectCross(gray, templ, Rect(0, H / 2, W / 2, H / 2), a.scoreBL);

    // Case 2: a cross is missing
    if (a.scoreTR < MIN_SCORE || a.scoreBL < MIN_SCORE) return a;

    // 2) Angle and scale: compare the segment BL cross -> TR cross
    //    of the scan with the one of the reference page
    Point2f vScan = a.crossTR - a.crossBL;
    Point2f vRef  = REF_CROSS_TR - REF_CROSS_BL;
    a.angleDeg = (std::atan2(vRef.y, vRef.x) - std::atan2(vScan.y, vScan.x)) * 180.0 / CV_PI;
    a.scale    = norm(vRef) / norm(vScan);

    // 3) Rotation around the BL cross, then translation that brings it to its place
    a.matrix = getRotationMatrix2D(a.crossBL, -a.angleDeg, a.scale);
    a.matrix.at<double>(0, 2) += REF_CROSS_BL.x - a.crossBL.x;
    a.matrix.at<double>(1, 2) += REF_CROSS_BL.y - a.crossBL.y;

    a.processable = true;
    return a;
}


Mat straightenPage(const Mat& image, const PageAlignment& alignment)
{
    Mat straight;
    warpAffine(image, straight, alignment.matrix, image.size(),
               INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));   // fill the missing empty corners with white
    return straight;
}
