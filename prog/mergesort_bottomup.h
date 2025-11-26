#pragma once
#include <vector>
#include "stats.h"
#include <algorithm>

template<typename T, typename Compare>
void mergesort_bottomup(std::vector<T>& a, Stats& st, Compare cmp) {
    size_t n = a.size();
    if (n <= 1) return;
    std::vector<T> aux(n);
    st.aux_bytes += n * sizeof(T);

    for (size_t width = 1; width < n; width <<= 1) {
        for (size_t i = 0; i < n; i += 2 * width) {
            size_t l = i;
            size_t m = std::min(i + width, n);
            size_t r = std::min(i + 2 * width, n);

            size_t p = l, q = m, k = l;
            while (p < m && q < r) {
                if (cmp(a[p], a[q], st)) aux[k++] = a[p++], st.copies++;
                else aux[k++] = a[q++], st.copies++;
            }
            while (p < m) aux[k++] = a[p++], st.copies++;
            while (q < r) aux[k++] = a[q++], st.copies++;
            for (size_t t = l; t < r; ++t) { a[t] = aux[t]; st.copies++; }
        }
    }
}
