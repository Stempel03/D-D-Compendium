#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>

namespace utils {

    std::string trim(const std::string& s) {
        const auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        const auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        return result;
    }

    int readInt(const std::string& prompt, int minValue, int maxValue) {
        while (true) {
            std::cout << prompt;
            std::string line;
            std::getline(std::cin, line);
            try {
                int value = std::stoi(trim(line));
                if (value < minValue || value > maxValue) {
                    std::cout << "Podaj wartosc z zakresu " << minValue << " - " << maxValue << ".\n";
                    continue;
                }
                return value;
            }
            catch (...) {
                std::cout << "Niepoprawna liczba.\n";
            }
        }
    }

    std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        return trim(line);
    }

    void pressEnterToContinue() {
        std::cout << "\nNacisnij Enter, aby kontynuowac...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

}
