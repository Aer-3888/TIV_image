#include <iostream>
#include <string>

#include <opencv2/imgcodecs.hpp>

int main()
{
    const std::string projectDir = TIV_IMAGE_SOURCE_DIR;
    const std::string inputPath = projectDir + "/straight_00715.png";
    const std::string outputDir = projectDir + "/output";
    const cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Could not read " << inputPath << '\n';
        return 1;
    }

    int y = 715;
    for (int i = 0; i < 7; ++i) {
        const cv::Rect crop(0, y, 2480, 260);
        if (crop.x + crop.width > image.cols || crop.y + crop.height > image.rows) {
            std::cerr << "Crop " << i << " is outside image bounds\n";
            return 1;
        }

        const std::string outputPath =
            outputDir + "/img_" + std::to_string(i) + ".png";
        if (!cv::imwrite(outputPath, image(crop))) {
            std::cerr << "Could not write " << outputPath << '\n';
            return 1;
        }
        
        std::cout << "Saved " << outputPath << '\n';

        y+=348;
    }
}
