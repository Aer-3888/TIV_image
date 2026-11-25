#ifndef PAGE_ID_HPP_
#define PAGE_ID_HPP_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

// Bits of the binary code in the dark header bar, leftmost bit first
std::vector<int> readBits(const cv::Mat& image);

unsigned long long bitsToValue(const std::vector<int>& bits);

// Page id of a straightened page, read from its binary code, on 5 digits ("00715")
std::string readPageId(const cv::Mat& straightPage);

#endif
