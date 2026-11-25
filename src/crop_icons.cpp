#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "grid.hpp"

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

    const cv::Rect area = WORKING_AREA;
    const cv::Mat workingArea = image(area); //crop img to where we work

    std::map<int, std::vector<cv::Rect>> contourBoxesSortBySection = sortBoxesBySection(workingArea);

    cv::Mat debugImage = workingArea.clone(); //debug image to draw boxes on

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
