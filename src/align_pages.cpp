// Detects the crosses and straightens each page.
//
// Usage: ./align_pages [folder] [--show]
//   default: ../NicIcon/all-scans
//   --show : shows each straightened page (press a key for the next one)

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
using namespace cv;

#include "alignment.hpp"

int main(int argc, char** argv)
{
    string folder = "../NicIcon/all-scans";
    bool show = false;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--show") show = true;
        else folder = arg;
    }

    Mat templ = imread("../assets/image_plus.png", IMREAD_GRAYSCALE);
    vector<String> files;
    glob(folder + "/*.png", files);

    ofstream rejects("not_processable.txt");
    int nbProcessable = 0;

    for (const String& file : files) {
        Mat image = imread(file);
        PageAlignment alignment = alignPage(image, templ);

        cout << file << "  scores " << alignment.scoreTR << " / " << alignment.scoreBL;
        if (!alignment.processable) {
            cout << "  NOT PROCESSABLE" << endl;
            rejects << file << endl;
            continue;
        }
        nbProcessable++;
        cout << "  angle " << alignment.angleDeg << " deg  scale " << alignment.scale << endl;

        Mat straight = straightenPage(image, alignment);
        // the rest of the processing chain should go here

        if (show) {
            Mat small;
            resize(straight, small, Size(), 0.25, 0.25);

            // Blue circles: where the detected crosses ended up after straightening
            vector<Point2f> crosses = {alignment.crossTR, alignment.crossBL};
            transform(crosses, crosses, alignment.matrix);
            for (const Point2f& c : crosses)
                circle(small, c * 0.25, 20, Scalar(255, 0, 0), 2);   // BGR: (255,0,0) = blue

            imshow("straightened page", small);
            waitKey(0);
        }
    }

    cout << nbProcessable << " / " << files.size() << " pages processable" << endl;
    return 0;
}
