#pragma once
#include <string>
#include <vector>

namespace utils {
    std::string trim(const std::string& s);
    std::vector<std::string> split(const std::string& s, char delimiter);
    int readInt(const std::string& prompt, int minValue, int maxValue);
    std::string readLine(const std::string& prompt);
    int readInt(const std::string& prompt, int minValue, int maxValue = 9999);
    void pressEnterToContinue();
}
