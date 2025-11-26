#pragma once
#include <chrono>

using namespace std;

using steady_clock_t = chrono::steady_clock;

inline double ms_between(steady_clock_t::time_point a, steady_clock_t::time_point b) {
    return chrono::duration<double, std::milli>(b - a).count();
}
