#ifndef ALIGNMENT_HPP_
#define ALIGNMENT_HPP_

#include "opencv2/core.hpp"

/*
 * Detection of the 2 "+" crosses (template assets/image_plus.png) and computation of the
 * transformation matrix that puts the page back straight.
 *
 * Case 1: both crosses are found -> we compute the transformation
 *         (rotation + scale + translation) that puts the page back straight.
 * Case 2: a cross is missing -> page marked as not processable.
 */

// Where the cross centers should be on a straight page
const cv::Point2f REF_CROSS_TR(2204.f, 471.f);   // top-right
const cv::Point2f REF_CROSS_BL(256.f, 3234.f);   // bottom-left

struct PageAlignment {
    bool processable = false;
    cv::Point2f crossTR, crossBL;      // centers of the crosses found in the scan
    double scoreTR = 0, scoreBL = 0;   // matchTemplate scores of the crosses
    double angleDeg = 0;               // rotation
    double scale = 1;
    cv::Mat matrix;                    // 2x3 matrix: scan to straight page
};


cv::Point2f detectCross(const cv::Mat& gray, const cv::Mat& templ, const cv::Rect& zone, double& score);

PageAlignment alignPage(const cv::Mat& image, const cv::Mat& templ);

cv::Mat straightenPage(const cv::Mat& image, const PageAlignment& alignment);

#endif
