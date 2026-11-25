#ifndef LABEL_MATCHING_HPP_
#define LABEL_MATCHING_HPP_

#include <array>
#include <string>
#include <vector>

#include "opencv2/core.hpp"

enum class Icon {
    Accident, Bomb, Car, Casualty, Electricity, Fire, FireBrigade,
    Flood, Gas, Injury, Paramedics, Person, Police, RoadBlock
};

enum class IconSize { None, Small, Medium, Large };

const std::array<Icon, 14> ALL_ICONS = {
    Icon::Accident, Icon::Bomb, Icon::Car, Icon::Casualty, Icon::Electricity, Icon::Fire, Icon::FireBrigade,
    Icon::Flood, Icon::Gas, Icon::Injury, Icon::Paramedics, Icon::Person, Icon::Police, Icon::RoadBlock
};

const std::array<IconSize, 3> WRITTEN_SIZES = { IconSize::Small, IconSize::Medium, IconSize::Large };

// Printed size words score above 0.9, rows without a word stay far below
const double SIZE_NONE_THRESHOLD = 0.7;

// Below this score the icon of a row is reported as uncertain
const double ICON_LOW_CONFIDENCE = 0.4;

struct Templates {
    std::vector<cv::Mat> icons;   // same order as ALL_ICONS
    std::vector<cv::Mat> sizes;   // same order as WRITTEN_SIZES
};

struct Match {
    int index = -1;
    double score = -1;
    double margin = 0;   // best score minus second best score
};

struct RowLabel {
    Icon icon = Icon::Accident;
    IconSize size = IconSize::None;
    double iconScore = 0;
    double sizeScore = 0;
};

std::string toString(Icon icon);
std::string toString(IconSize size);

// Reads <dir>/icons/<icon>.png and <dir>/sizes/<size>.png in grayscale.
// Throws std::runtime_error if a template is missing.
Templates loadTemplates(const std::string& dir);

// Best template inside the area, with TM_CCOEFF_NORMED. The area is padded with white
// when it is smaller than a template.
Match bestMatch(const cv::Mat& area, const std::vector<cv::Mat>& templates);

// Zone left of the boxes of a section, holding the printed icon and its size word.
// Coordinates are those of the working area, like the boxes.
cv::Rect labelArea(const std::vector<cv::Rect>& sectionBoxes);

RowLabel identifyRow(const cv::Mat& workingGray, const std::vector<cv::Rect>& sectionBoxes,
                     const Templates& templates);

#endif
