#include "grid.hpp"

#include <opencv2/opencv.hpp>

std::map<int, std::vector<cv::Rect>> sortBoxesBySection(const cv::Mat& workingArea)
{
    cv::Mat grayImage; //gray image for thresholding
    cv::Mat binaryImage; //binary image after thresholding
    cv::Mat hsvImage; //hsv image for color filtering (easier than BGR because it isolates hue from lighting and brightness)
    cv::Mat blueInkMask; //mask for blue color (ink color)

    cv::cvtColor(workingArea, grayImage, cv::COLOR_BGR2GRAY);
    cv::cvtColor(workingArea, hsvImage, cv::COLOR_BGR2HSV);

    //filter blue color (ink color)
    cv::inRange(hsvImage, 
        cv::Scalar(75, 80, 90), //hue between 75 and 105
        cv::Scalar(105, 255, 255), // sat > 80 and Value > 90
        blueInkMask); //put it in the mask (mask is just white pixels when its in range)

    //thresholding to get binary image from the grayImage
    cv::adaptiveThreshold(
        grayImage, binaryImage, 255,
        cv::ADAPTIVE_THRESH_MEAN_C, //adapt to local light differences
        cv::THRESH_BINARY_INV, //so lines and symbols become white
        15, 5 //mean of pixel in a 15x15 area and we remove 5 from the result
    );

    binaryImage.setTo(0, blueInkMask); //remove blue form the binary image

    int horizontal_size = binaryImage.cols * 0.065; //a box is 6.5% of the image width
    int vertical_size = binaryImage.rows * 0.045; //a box is 4.5% of the image height

    //extract horizontal lines
    cv::Mat horizontal = binaryImage.clone();
    cv::Mat horizontalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));
    cv::erode(horizontal, horizontal, horizontalStructure);
    cv::dilate(horizontal, horizontal, horizontalStructure);

    //extract vertical lines
    cv::Mat vertical = binaryImage.clone();
    cv::Mat verticalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, vertical_size));
    cv::erode(vertical, vertical, verticalStructure);
    cv::dilate(vertical, vertical, verticalStructure);

    //line fusion (of the 2 images)
    cv::Mat grid = horizontal + vertical;

    // detect contours (boxes)
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(grid, contours, 
        cv::RETR_EXTERNAL,  //only external contours
        cv::CHAIN_APPROX_SIMPLE); //approx a line with 2 points only

    std::vector<cv::Rect> contourBoxes;  //all boxes
    std::map<int, std::vector<cv::Rect>> contourBoxesSortBySection; //sort boxes by y coordinate (categs)

    //categorize boxes by y coordinate (categs)
    for (size_t i = 0; i < contours.size(); ++i){
        const cv::Rect box = cv::boundingRect(contours[i]);
        if (box.width < 200 || box.width > 330 || box.height < 200 || box.height > 330) {
            continue; //not a box (a box is about 265x265), so a page without boxes gives no section
        }
        bool isNewSection = true;
        for (auto& section : contourBoxesSortBySection){
            if (abs(box.y - section.first) < 20){ //boxes of a row can be a few pixels apart after straightening
                section.second.push_back(box);
                isNewSection = false;
                break;
            }
        }

        if (isNewSection) {
            contourBoxesSortBySection[box.y].push_back(box);
        }

        contourBoxes.push_back(box);
    }

    return contourBoxesSortBySection;
}
