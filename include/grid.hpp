#ifndef GRID_HPP_
#define GRID_HPP_

#include <map>
#include <vector>

#include <opencv2/core.hpp>

const cv::Rect WORKING_AREA(180, 700, 2200, 2400);

//boxes of the working area sorted by section (key = y coordinate of the section)
std::map<int, std::vector<cv::Rect>> sortBoxesBySection(const cv::Mat& workingArea);

#endif
