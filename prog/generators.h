#pragma once
#include <vector>
#include <random>
#include <string>

enum class DataType { RANDOM , SORTED , REVERSED , NEARLY_SORTED , FEW_UNIQUE };

inline const char* data_type_name(DataType t) {
    switch (t) {
    case DataType::RANDOM: return "random";
    case DataType::SORTED: return "sorted";
    case DataType::REVERSED: return "reversed";
    case DataType::NEARLY_SORTED: return "nearly_sorted";
    case DataType::FEW_UNIQUE: return "few_unique";
        return "unknown";
    }
}

inline std::vector<int> generate_data(size_t n, DataType kind, std::mt19937& rng) {
    std::vector<int> a(n);
    if (kind == DataType::RANDOM) {
        std::uniform_int_distribution<int> d(0, (int)n);
        for (size_t i = 0; i < n; ++i) a[i] = d(rng);
    }
    else if (kind == DataType::SORTED) {
        for (size_t i = 0; i < n; ++i) a[i] = (int)i;
    }
    else if (kind == DataType::REVERSED) {
        for (size_t i = 0; i < n; ++i) a[i] = (int)(n - i);
    }
    else if (kind == DataType::NEARLY_SORTED) {
        for (size_t i = 0; i < n; ++i) a[i] = (int)i;
        size_t swaps = n / 100 + 1;
        std::uniform_int_distribution<size_t> d(0, n - 1);
        for (size_t s = 0; s < swaps; ++s) std::swap(a[d(rng)], a[d(rng)]);
    }
    else if (kind == DataType::FEW_UNIQUE) {
        std::uniform_int_distribution<int> d(0, 5);
        for (size_t i = 0; i < n; ++i) a[i] = d(rng);
    }
    return a;
}
