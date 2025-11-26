#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>


inline std::vector<std::string> read_gensort_file(const std::string& path) {
    std::ifstream fin(path, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open file: " + path);
    std::vector<std::string> data;
    std::string line;
    while (std::getline(fin, line)) {
        if (!line.empty()) data.push_back(line);
    }
    return data;
}

inline void save_gensort_file(const std::string& path, const std::vector<std::string>& data) {
    std::ofstream fout(path, std::ios::binary);
    if (!fout) throw std::runtime_error("Cannot open for writing: " + path);
    for (const auto& line : data) {
        fout << line << "\n";
    }
}

inline void save_int_file(const std::string& path, const std::vector<int>& data) {
    std::ofstream fout(path);
    if (!fout) throw std::runtime_error("Cannot open for writing: " + path);
    for (const auto& val : data) {
        fout << val << "\n";
    }
}