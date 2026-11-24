#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

#include <opencv2/imgproc.hpp>
#if CV_VERSION_MAJOR >= 5
#include <opencv2/geometry.hpp>
#endif
#include <opencv2/highgui.hpp>
using namespace cv;

#include "histogram.hpp"

int main(){
	string path = "../NicIcon/w002-scans/00214.png";
    Mat image = imread(path,cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Could not read " << path << '\n';
        return 1;
    }

    cout << "dimensions: " << image.cols << " x " << image.rows << '\n';

    Rect area(1420,600, 840, 60);
    Mat roi = image(area);

    Mat gray;
    cvtColor(roi, gray, COLOR_BGR2GRAY);

    Mat binary;
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Find the outlines of white regions in the binary image.
    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    cout << "Contours found: " << contours.size() << '\n';

    // Draw all contours in green on a copy of the original crop.
    Mat result = roi.clone();
    drawContours(result, contours, -1, Scalar(0, 255, 0), 2);

    // Save the result, since your display connection previously failed.
    imwrite("contours.png", result);

    const double firstPositionX = 27.5;
    const double spacing = 55.7;

    const int numberOfPositions = 15;

    std::vector<int> bits(numberOfPositions, 0);

    for (const auto& contour : contours) {
        cv::Rect box = cv::boundingRect(contour);
        double centerX = box.x + box.width / 2.0;

        int position = static_cast<int>(
            std::round((centerX - firstPositionX) / spacing)
        );

        if (position >= 0 && position < numberOfPositions) {
            bits[position] = 1;
        }
    }

    cout << "Bits: ";
    for(int bit : bits) {
        cout << bit;
    }
    cout << endl;

    unsigned long long value = 0;

    for(int bit : bits) {
        value = value * 2 + bit;
    }
    cout << value << endl;

    imshow("zaza", roi);
    waitKey(0);



}