#include "page_id.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
using namespace std;

#include <opencv2/imgproc.hpp>
#if CV_VERSION_MAJOR >= 5
#include <opencv2/geometry.hpp>
#endif
using namespace cv;

vector<int> readBits(const Mat& image)
{
    Rect area(1420,600, 840, 60);
    Mat roi = image(area);

    Mat gray;
    cvtColor(roi, gray, COLOR_BGR2GRAY);

    Mat binary;
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Find the outlines of white regions in the binary image.
    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

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

    return bits;
}

unsigned long long bitsToValue(const vector<int>& bits)
{
    unsigned long long value = 0;

    for(int bit : bits) {
        value = value * 2 + bit;
    }
    return value;
}

string readPageId(const Mat& straightPage)
{
    ostringstream id;
    id << setw(5) << setfill('0') << bitsToValue(readBits(straightPage));
    return id.str();
}
