#include "label_matching.hpp"

#include <algorithm>
#include <stdexcept>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

namespace {

const int MATCH_PADDING = 8;

Mat loadGray(const std::string& path)
{
    Mat image = imread(path, IMREAD_GRAYSCALE);
    if (image.empty())
        throw std::runtime_error("Could not read template " + path);
    return image;
}

Mat padToFit(const Mat& area, const Mat& templ)
{
    const int extraX = std::max(0, templ.cols - area.cols);
    const int extraY = std::max(0, templ.rows - area.rows);
    Mat padded;
    copyMakeBorder(area, padded,
                   MATCH_PADDING + extraY / 2, MATCH_PADDING + extraY - extraY / 2,
                   MATCH_PADDING + extraX / 2, MATCH_PADDING + extraX - extraX / 2,
                   BORDER_CONSTANT, Scalar(255));
    return padded;
}

}

std::string toString(Icon icon)
{
    switch (icon) {
        case Icon::Accident: return "accident";
        case Icon::Bomb: return "bomb";
        case Icon::Car: return "car";
        case Icon::Casualty: return "casualty";
        case Icon::Electricity: return "electricity";
        case Icon::Fire: return "fire";
        case Icon::FireBrigade: return "fire_brigade";
        case Icon::Flood: return "flood";
        case Icon::Gas: return "gas";
        case Icon::Injury: return "injury";
        case Icon::Paramedics: return "paramedics";
        case Icon::Person: return "person";
        case Icon::Police: return "police";
        case Icon::RoadBlock: return "road_block";
    }
    throw std::invalid_argument("Unknown icon");
}

std::string toString(IconSize size)
{
    switch (size) {
        case IconSize::None: return "none";
        case IconSize::Small: return "small";
        case IconSize::Medium: return "medium";
        case IconSize::Large: return "large";
    }
    throw std::invalid_argument("Unknown size");
}

Templates loadTemplates(const std::string& dir)
{
    Templates templates;
    for (Icon icon : ALL_ICONS)
        templates.icons.push_back(loadGray(dir + "/icons/" + toString(icon) + ".png"));
    for (IconSize size : WRITTEN_SIZES)
        templates.sizes.push_back(loadGray(dir + "/sizes/" + toString(size) + ".png"));
    return templates;
}

Match bestMatch(const Mat& area, const std::vector<Mat>& templates)
{
    Match best;
    double second = -1;
    for (size_t i = 0; i < templates.size(); ++i) {
        Mat result;
        matchTemplate(padToFit(area, templates[i]), templates[i], result, TM_CCOEFF_NORMED);
        double score = 0;
        minMaxLoc(result, nullptr, &score);
        if (score > best.score) {
            second = best.score;
            best.score = score;
            best.index = int(i);
        } else if (score > second) {
            second = score;
        }
    }
    best.margin = best.score - second;
    return best;
}

Rect labelArea(const std::vector<Rect>& sectionBoxes)
{
    if (sectionBoxes.empty())
        return Rect();
    Rect section = sectionBoxes.front();
    for (const Rect& box : sectionBoxes)
        section |= box;
    return Rect(0, section.y, section.x, section.height);
}

RowLabel identifyRow(const Mat& workingGray, const std::vector<Rect>& sectionBoxes, const Templates& templates)
{
    const Rect area = labelArea(sectionBoxes) & Rect(0, 0, workingGray.cols, workingGray.rows);
    if (area.empty())
        throw std::invalid_argument("Section has no label area");

    const Mat label = workingGray(area);
    const Match icon = bestMatch(label, templates.icons);
    const Match size = bestMatch(label, templates.sizes);

    RowLabel result;
    result.icon = ALL_ICONS.at(icon.index);
    result.iconScore = icon.score;
    result.sizeScore = size.score;
    result.size = size.score >= SIZE_NONE_THRESHOLD ? WRITTEN_SIZES.at(size.index) : IconSize::None;
    return result;
}
