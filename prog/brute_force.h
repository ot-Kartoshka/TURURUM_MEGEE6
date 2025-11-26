#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>
#include <tuple>
#include <functional>
#include "stats.h"

inline uint64_t bf_count(const std::vector<int>& perm) {
    std::vector<int> a = perm, aux(perm.size());
    uint64_t cmp = 0;
    std::function<void(size_t, size_t)> rec;
    rec = [&](size_t l, size_t r) {
        if (r - l <= 1) return;
        size_t m = (l + r) / 2;
        rec(l, m);
        rec(m, r);
        size_t i = l, j = m, k = l;
        while (i < m && j < r) {
            ++cmp;
            if (a[i] <= a[j]) aux[k++] = a[i++];
            else aux[k++] = a[j++];
        }
        while (i < m) aux[k++] = a[i++];
        while (j < r) aux[k++] = a[j++];
        for (size_t t = l; t < r; ++t) a[t] = aux[t];
        };
    rec(0, perm.size());
    return cmp;
}

inline std::pair<uint64_t, uint64_t> brute_force_minmax(size_t n) {
    std::vector<int> p(n);
    for (size_t i = 0; i < n; ++i) p[i] = (int)i;
    uint64_t mn = UINT64_MAX, mx = 0;
    do {
        uint64_t c = bf_count(p);
        if (c < mn) mn = c;
        if (c > mx) mx = c;
    } while (std::next_permutation(p.begin(), p.end()));
    return { mn, mx };
}

inline std::tuple<uint64_t, std::vector<int>, uint64_t, std::vector<int>>
bruteforce_full(size_t n) {
    std::vector<int> p(n);
    for (size_t i = 0; i < n; ++i) p[i] = (int)i;
    uint64_t mn = UINT64_MAX, mx = 0;
    std::vector<int> pmn, pmx;
    do {
        uint64_t c = bf_count(p);
        if (c < mn) { mn = c; pmn = p; }
        if (c > mx) { mx = c; pmx = p; }
    } while (std::next_permutation(p.begin(), p.end()));
    return { mn, pmn, mx, pmx };
}
