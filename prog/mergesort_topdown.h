#pragma once
#include <vector>
#include "stats.h"

template<typename T, typename Compare>
void merge_td_rec(std::vector<T>& src, std::vector<T>& aux, size_t l, size_t r, Stats& st, Compare cmp) {
    if (r - l <= 1) return;
    size_t m = l + (r - l) / 2;
    merge_td_rec(src, aux, l, m, st, cmp);
    merge_td_rec(src, aux, m, r, st, cmp);

    size_t i = l, j = m, k = l;
    while (i < m && j < r) {
        bool left_is_less = cmp(src[i], src[j], st);
        if (left_is_less) aux[k++] = src[i++], st.copies++;
        else aux[k++] = src[j++], st.copies++;
    }
    while (i < m) aux[k++] = src[i++], st.copies++;
    while (j < r) aux[k++] = src[j++], st.copies++;

    for (size_t t = l; t < r; ++t) { src[t] = aux[t]; st.copies++; }
}

template<typename T, typename Compare>
void mergesort_topdown(std::vector<T>& a, Stats& st, Compare cmp) {
    if (a.size() <= 1) return;
    std::vector<T> aux(a.size());
    st.aux_bytes += a.size() * sizeof(T);
    merge_td_rec(a, aux, 0, a.size(), st, cmp);
}
