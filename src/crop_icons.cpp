#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

int main()
{
    const std::vector<std::string> categories = {
        "accident",
        "bomb",
        "car",
        "casualty",
        "electricity",
        "fire",
        "fire_brigade",
        "gas",
        "injury",
        "paramedics",
        "person",
        "police",
        "road_block",
        "flood"
    };

    const std::vector<std::string> size = {
        "none",
        "small",
        "medium",
        "large"
    };

    const std::string projectDir = TIV_IMAGE_SOURCE_DIR;
    const std::string inputPath = projectDir + "/straight_00715.png";
    const std::string outputDir = projectDir + "/output";
    
    const cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Could not read " << inputPath << '\n';
        return 1;
    }

    const cv::Rect area(180, 700, 2200, 2400);
    const cv::Mat workingArea = image(area); //crop img to where we work

    cv::Mat grayImage; //gray image for thresholding
    cv::Mat binaryImage; //binary image after thresholding
    cv::Mat hsvImage; //hsv image for color filtering
    cv::Mat blueInkMask; //mask for blue color (ink color)

    cv::cvtColor(workingArea, grayImage, cv::COLOR_BGR2GRAY);
    cv::cvtColor(workingArea, hsvImage, cv::COLOR_BGR2HSV);

    //filter blue color (ink color)
    cv::inRange(hsvImage, cv::Scalar(75, 80, 90),
                cv::Scalar(105, 255, 255), blueInkMask);

    //thresholding to get binary image
    cv::adaptiveThreshold(
        grayImage, binaryImage, 255,
        cv::ADAPTIVE_THRESH_MEAN_C,
        cv::THRESH_BINARY_INV,
        15, 5
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

    //line fusion
    cv::Mat grid = horizontal + vertical;

    // detect contours (boxes)
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(grid, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat debugImage = workingArea.clone(); //debug image to draw boxes on

    std::vector<cv::Rect> contourBoxes;  //all boxes
    std::map<int, std::vector<cv::Rect>> contourBoxesSortBySection; //sort boxes by y coordinate (categs)

    //categorize boxes by y coordinate (categs)
    for (size_t i = 0; i < contours.size(); ++i){
        const cv::Rect box = cv::boundingRect(contours[i]);
        bool isNewSection = true;
        for (auto& section : contourBoxesSortBySection){
            if (abs(box.y - section.first) < 3){
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

    //output each category as a separate image
    for (int i = 0; i < contourBoxesSortBySection.size(); ++i) {

        auto section = std::next(contourBoxesSortBySection.begin(), i);

        cv::Rect crop(0, section->first, workingArea.cols, section->second[0].height);
        cv::rectangle(debugImage, crop, cv::Scalar(0, 0, 255), 2); //draw red rectangle around the section on the debug image

        const std::string outputPath = outputDir + "/img_" + std::to_string(i) + "full.png";
        if (!cv::imwrite(outputPath, workingArea(crop))) {
            std::cerr << "Could not write " << outputPath << '\n';
            return 1;
        }
        for (int j = 0; j < section->second.size(); ++j) {
            cv::Rect box = section->second[j];
            std::string outputPath2 = outputDir + "/img_" + std::to_string(i) + "_" + std::to_string(j) + ".png";
            if (!cv::imwrite(outputPath2, workingArea(box))) {
                std::cerr << "Could not write " << outputPath << '\n';
                return 1;
            }
            cv::rectangle(debugImage, box, cv::Scalar(0, 255, 0), 2); //draw green rectangle around the box on the debug image
        }
        
        std::cout << "Saved " << std::to_string(i) << '\n';
    }

    
    //WHATS NEXT:
    //1. For each section image (inside contourBoxesSortBySection), detect the pictogram to sort them into their own folders.
    //2. Send every boxes (inside contourBoxes) within this section into the same folder.
    
    //Note : For now, sections and boxes are saved in /output folder but it's just a proof of work, images aren't sorted at all.
    
    
    //show debug image
    cv::namedWindow("debug", cv::WINDOW_NORMAL);
    cv::resizeWindow("debug", 1920, 1080);
    cv::imshow("debug", debugImage);
    cv::waitKey(0);
}
