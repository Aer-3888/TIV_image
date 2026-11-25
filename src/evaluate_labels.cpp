// Compares the labels found by label_pages with a hand-made ground truth.
//
// Usage: ./evaluate_labels [labels.csv] [ground truth.json]
//   default: output/labels.csv  assets/ground_truth_bdd_test.json
//
// Ground truth format: { "00005" : [{"casualty" : "small"}, {"bomb" : "medium"}, ...], ... }

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "opencv2/core.hpp"

namespace {

struct Label {
    std::string icon;
    std::string size;
};

using PageLabels = std::map<std::string, std::vector<Label>>;

struct Score {
    int correct = 0;
    int total = 0;
};

struct Report {
    Score icon, size, row;
    std::map<std::string, Score> perIcon;
    std::map<std::string, std::map<std::string, int>> sizeConfusion;   // expected -> found -> count
    std::vector<std::string> mismatches;
    std::vector<std::string> missingPages;
    std::vector<std::string> rowCountMismatches;
};

const Label MISSING{"(missing)", "(missing)"};

PageLabels readGroundTruth(const std::string& path)
{
    cv::FileStorage fs(path, cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
    if (!fs.isOpened())
        throw std::runtime_error("Could not read " + path);

    PageLabels truth;
    for (const cv::FileNode& page : fs.root()) {
        std::vector<Label>& rows = truth[page.name()];
        for (const cv::FileNode& row : page)
            for (const cv::FileNode& entry : row)
                rows.push_back({entry.name(), static_cast<std::string>(entry)});
    }
    if (truth.empty())
        throw std::runtime_error("No page in " + path);
    return truth;
}

PageLabels readFoundLabels(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not read " + path);

    PageLabels found;
    std::string line;
    std::getline(file, line);   // header
    while (std::getline(file, line)) {
        std::vector<std::string> fields;
        std::stringstream stream(line);
        for (std::string field; std::getline(stream, field, ',');)
            fields.push_back(field);
        if (fields.size() < 4)
            throw std::runtime_error("Malformed line in " + path + ": " + line);

        std::vector<Label>& rows = found[fields[0]];
        const size_t row = std::stoul(fields[1]);
        if (rows.size() <= row)
            rows.resize(row + 1, MISSING);
        rows[row] = {fields[2], fields[3]};
    }
    return found;
}

void add(Score& score, bool correct)
{
    score.correct += correct ? 1 : 0;
    score.total += 1;
}

void compareRow(const std::string& page, size_t row, const Label& expected, const Label& got, Report& report)
{
    const bool iconOk = expected.icon == got.icon;
    const bool sizeOk = expected.size == got.size;
    add(report.icon, iconOk);
    add(report.size, sizeOk);
    add(report.row, iconOk && sizeOk);
    add(report.perIcon[expected.icon], iconOk);
    report.sizeConfusion[expected.size][got.size]++;
    if (!iconOk || !sizeOk)
        report.mismatches.push_back(page + " row " + std::to_string(row) + ": expected "
                                    + expected.icon + "/" + expected.size + ", got " + got.icon + "/" + got.size);
}

Report evaluate(const PageLabels& truth, const PageLabels& found)
{
    Report report;
    for (const auto& [page, expectedRows] : truth) {
        const auto it = found.find(page);
        const std::vector<Label> noRows;
        const std::vector<Label>& gotRows = it == found.end() ? noRows : it->second;
        if (it == found.end())
            report.missingPages.push_back(page);
        else if (gotRows.size() != expectedRows.size())
            report.rowCountMismatches.push_back(page + " (" + std::to_string(gotRows.size()) + " rows)");

        for (size_t row = 0; row < expectedRows.size(); ++row)
            compareRow(page, row, expectedRows[row], row < gotRows.size() ? gotRows[row] : MISSING, report);
    }
    return report;
}

std::string percent(const Score& s)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << (s.total ? 100.0 * s.correct / s.total : 0.0) << " % ("
        << s.correct << "/" << s.total << ")";
    return out.str();
}

void printList(const std::string& title, const std::vector<std::string>& items)
{
    std::cout << "\n" << title << ": " << items.size() << "\n";
    for (const std::string& item : items)
        std::cout << "  " << item << "\n";
}

void printReport(const Report& report, size_t pages)
{
    std::cout << "Pages in ground truth: " << pages << "\n\n"
              << "Icon accuracy: " << percent(report.icon) << "\n"
              << "Size accuracy: " << percent(report.size) << "\n"
              << "Row accuracy (icon and size): " << percent(report.row) << "\n";

    std::cout << "\nIcon accuracy per class:\n";
    for (const auto& [icon, score] : report.perIcon)
        std::cout << "  " << std::left << std::setw(13) << icon << percent(score) << "\n";

    std::cout << "\nSize confusion (expected -> found):\n";
    for (const auto& [expected, row] : report.sizeConfusion)
        for (const auto& [got, count] : row)
            std::cout << "  " << std::left << std::setw(8) << expected << " -> " << std::setw(10) << got << count << "\n";

    printList("Pages not labelled", report.missingPages);
    printList("Pages with a wrong number of rows", report.rowCountMismatches);
    printList("Mismatches", report.mismatches);
}

}

int main(int argc, char** argv)
{
    const std::string projectDir = TIV_IMAGE_SOURCE_DIR;
    const std::string labelsPath = argc > 1 ? argv[1] : projectDir + "/output/labels.csv";
    const std::string truthPath = argc > 2 ? argv[2] : projectDir + "/assets/ground_truth_bdd_test.json";

    try {
        const PageLabels truth = readGroundTruth(truthPath);
        const PageLabels found = readFoundLabels(labelsPath);
        printReport(evaluate(truth, found), truth.size());
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
