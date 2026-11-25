// Straightens each scan, finds its rows and labels them with their icon and size.
//
// Usage: ./label_pages [scans folder] [--out folder] [--export]
//   default: dataset/NicIcon/all-scans  --out output
//   --export : also saves every box in <out>/<icon>/<size>/<page>_<row>_<column>.png
//
// Writes <out>/labels.csv, <out>/low_confidence.txt and <out>/not_processable.txt

#include <filesystem>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "alignment.hpp"
#include "grid.hpp"
#include "label_matching.hpp"
#include "page_id.hpp"

namespace fs = std::filesystem;

namespace {

struct Options {
    std::string scansDir;
    std::string outDir;
    bool exportBoxes = false;
};

struct Outputs {
    std::ofstream labels;
    std::ofstream lowConfidence;
    std::ofstream notProcessable;
};

Options parseArguments(int argc, char** argv)
{
    const std::string projectDir = TIV_IMAGE_SOURCE_DIR;
    Options options{projectDir + "/dataset/NicIcon/all-scans", projectDir + "/output", false};
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--export")
            options.exportBoxes = true;
        else if (arg == "--out" && i + 1 < argc)
            options.outDir = argv[++i];
        else if (arg.rfind("--", 0) == 0)
            throw std::invalid_argument("Unknown option " + arg);
        else
            options.scansDir = arg;
    }
    return options;
}

std::ofstream openOutput(const std::string& path)
{
    std::ofstream file(path);
    if (!file)
        throw std::runtime_error("Could not write " + path);
    return file;
}

void exportBoxes(const cv::Mat& workingArea, std::vector<cv::Rect> boxes, const RowLabel& label,
                 const std::string& pageId, size_t rowIndex, const std::string& outDir)
{
    std::sort(boxes.begin(), boxes.end(), [](const cv::Rect& a, const cv::Rect& b) { return a.x < b.x; });
    const fs::path dir = fs::path(outDir) / toString(label.icon) / toString(label.size);
    fs::create_directories(dir);
    for (size_t col = 0; col < boxes.size(); ++col) {
        const fs::path path = dir / (pageId + "_" + std::to_string(rowIndex) + "_" + std::to_string(col) + ".png");
        if (!cv::imwrite(path.string(), workingArea(boxes[col])))
            throw std::runtime_error("Could not write " + path.string());
    }
}

void labelPage(const cv::Mat& page, const std::string& pageId, const Templates& templates,
               const Options& options, Outputs& out)
{
    const cv::Mat workingArea = page(WORKING_AREA);
    cv::Mat gray;
    cv::cvtColor(workingArea, gray, cv::COLOR_BGR2GRAY);

    size_t i = 0;
    for (const auto& [sectionY, boxes] : sortBoxesBySection(workingArea)) {
        const RowLabel label = identifyRow(gray, boxes, templates);
        out.labels << pageId << ',' << i << ',' << toString(label.icon) << ',' << toString(label.size)
                   << ',' << label.iconScore << ',' << label.sizeScore << '\n';
        if (label.iconScore < ICON_LOW_CONFIDENCE)
            out.lowConfidence << pageId << ',' << i << ',' << toString(label.icon) << ',' << label.iconScore << '\n';
        if (options.exportBoxes)
            exportBoxes(workingArea, boxes, label, pageId, i, options.outDir);
        ++i;
    }
}

}

int main(int argc, char** argv)
{
    try {
        const Options options = parseArguments(argc, argv);
        const std::string projectDir = TIV_IMAGE_SOURCE_DIR;
        const Templates templates = loadTemplates(projectDir + "/assets/templates");
        const cv::Mat cross = cv::imread(projectDir + "/assets/image_plus.png", cv::IMREAD_GRAYSCALE);
        if (cross.empty())
            throw std::runtime_error("Could not read assets/image_plus.png");

        std::vector<cv::String> files;
        cv::glob(options.scansDir + "/*.png", files);
        if (files.empty())
            throw std::runtime_error("No scan found in " + options.scansDir);

        fs::create_directories(options.outDir);
        Outputs out{openOutput(options.outDir + "/labels.csv"),
                    openOutput(options.outDir + "/low_confidence.txt"),
                    openOutput(options.outDir + "/not_processable.txt")};
        out.labels << "page,row,icon,size,icon_score,size_score\n";

        for (const cv::String& file : files) {
            const cv::Mat scan = cv::imread(file, cv::IMREAD_COLOR);
            const PageAlignment alignment = scan.empty() ? PageAlignment() : alignPage(scan, cross);
            if (!alignment.processable) {
                out.notProcessable << fs::path(file).filename().string() << '\n';
                continue;
            }
            const cv::Mat straight = straightenPage(scan, alignment);
            labelPage(straight, readPageId(straight), templates, options, out);
        }
        std::cout << files.size() << " scans processed, results in " << options.outDir << '\n';
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
