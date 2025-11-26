#pragma once
#include <cstdint>

struct Stats {
    uint64_t comparisons = 0;
    uint64_t copies = 0;
    uint64_t aux_bytes = 0;

    void reset() {
        comparisons = copies = aux_bytes = 0;
    }
};
