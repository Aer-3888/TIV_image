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
#include "page_id.hpp"

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

    vector<int> bits = readBits(image);

    cout << "Bits: ";
    for(int bit : bits) {
        cout << bit;
    }
    cout << endl;

    unsigned long long value = bitsToValue(bits);
    cout << value << endl;

    imshow("zaza", roi);
    waitKey(0);



}