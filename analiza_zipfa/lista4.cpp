#include "matplotlibcpp.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace plt = matplotlibcpp;

class ZipfAnalyzer {
private:
    std::string baseName;
    std::vector<std::pair<int, std::string>> wordFrequencyList;
    std::vector<int> ranks;
    std::vector<int> frequencies;

public:
    ZipfAnalyzer(const std::string& fileBaseName) : baseName(fileBaseName) {
        std::string fullName = baseName + ".txt";
        cleanTextFile(fullName);
        std::ifstream input(fullName);
        if (!input.is_open()) {
            std::cerr << "Nie można otworzyć pliku: " << fullName << "\n";
            return;
        }

        auto frequencyMap = countWordFrequencies(input);
        for (const auto& entry : frequencyMap) {
            wordFrequencyList.emplace_back(entry.second, entry.first);
        }

        std::sort(wordFrequencyList.begin(), wordFrequencyList.end(),
            [](const auto& a, const auto& b) { return a.first > b.first; });

        input.close();
    }

    void exportFrequencies(const std::string& outputName) const {
        std::ofstream outFile(outputName + "_data.txt");
        for (const auto& [count, word] : wordFrequencyList) {
            outFile << count << " " << word << "\n";
        }
    }

    void displayPlot() {
        for (size_t i = 0; i < wordFrequencyList.size(); ++i) {
            ranks.push_back(static_cast<int>(i + 1));
            frequencies.push_back(wordFrequencyList[i].first);
        }

        double exponent = estimateZipfExponent();

        plt::clf();
        plt::loglog(ranks, frequencies);
        plt::named_plot("Zipf's Law", ranks, frequencies, "b-");
        plt::title("Zipf - " + baseName + " (s = " + std::to_string(exponent) + ")");
        plt::xlabel("Rank");
        plt::ylabel("Frequency");
        plt::legend();
        plt::grid(true);
        plt::save("plot_" + baseName + ".png");
    }

private:
    static void toLowerCase(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char ch) { return std::tolower(ch); });
    }

    static void removePunctuation(std::string& str) {
        str.erase(std::remove_if(str.begin(), str.end(),
            [](unsigned char ch) { return std::ispunct(ch); }),
            str.end());
    }

    void cleanTextFile(const std::string& filePath) {
        std::ifstream inFile(filePath);
        std::ofstream outFile("cleaned_temp.txt");
        std::string line;

        while (std::getline(inFile, line)) {
            removePunctuation(line);
            toLowerCase(line);
            outFile << line << "\n";
        }

        inFile.close();
        outFile.close();

        std::remove(filePath.c_str());
        std::rename("cleaned_temp.txt", filePath.c_str());
    }

    static std::map<std::string, int> countWordFrequencies(std::istream& input) {
        std::map<std::string, int> freqMap;
        std::string word;
        while (input >> word) {
            freqMap[word]++;
        }
        input.clear();
        return freqMap;
    }

    double estimateZipfExponent() {
        std::vector<double> logRanks;
        std::vector<double> logFrequencies;

        for (size_t i = 0; i < wordFrequencyList.size(); ++i) {
            logRanks.push_back(std::log(i + 1));
            logFrequencies.push_back(std::log(wordFrequencyList[i].first));
        }

        double avgLogRank = std::accumulate(logRanks.begin(), logRanks.end(), 0.0) / logRanks.size();
        double avgLogFreq = std::accumulate(logFrequencies.begin(), logFrequencies.end(), 0.0) / logFrequencies.size();

        double numerator = 0.0;
        double denominator = 0.0;

        for (size_t i = 0; i < logRanks.size(); ++i) {
            double deltaX = logRanks[i] - avgLogRank;
            double deltaY = logFrequencies[i] - avgLogFreq;
            numerator += deltaX * deltaY;
            denominator += deltaX * deltaX;
        }

        return -(numerator / denominator);
    }
};

int main() {
    std::vector<std::string> texts = { "Pan_Tadeusz", "Boska_Komedia", "RomeoJulia" };

    for (const auto& name : texts) {
        ZipfAnalyzer analyzer(name);
        analyzer.exportFrequencies(name);
        analyzer.displayPlot();
    }

    return 0;
}
