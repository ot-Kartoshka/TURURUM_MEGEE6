#pragma once
#include <vector>
#include <algorithm>
#include "stats.h"

static const size_t INSERTION_THRESHOLD = 32;

template<typename T, typename Compare>
void insertion_sort(std::vector<T>& a, size_t left, size_t right,
    Stats& st, Compare cmp)
{
    for (size_t i = left + 1; i <= right; ++i) {
        T key = a[i];
        st.copies++;
        size_t j = i;
        while (j > left && cmp(key, a[j - 1], st)) {
            a[j] = a[j - 1];
            st.copies++;
            --j;
        }
        a[j] = key;
        st.copies++;
    }
}

template<typename T, typename Compare>
void merge_left_copy(const std::vector<T>& src,
    std::vector<T>& dst,
    std::vector<T>& aux_left,
    size_t left, size_t mid, size_t right,
    Stats& st, Compare cmp)
{
    size_t left_len = mid - left + 1;
    for (size_t i = 0; i < left_len; ++i) {
        aux_left[i] = src[left + i];
        st.copies++;
    }

    size_t i = 0;            
    size_t j = mid + 1;      
    size_t k = left;        

    while (i < left_len && j <= right) {
        if (!cmp(src[j], aux_left[i], st)) {
            dst[k++] = aux_left[i++];
        }
        else {
            dst[k++] = src[j++];
        }
        st.copies++;
    }

    while (i < left_len) {
        dst[k++] = aux_left[i++];
        st.copies++;
    }
    while (j <= right) {
        dst[k++] = src[j++];
        st.copies++;
    }
}

template<typename T, typename Compare>
void mergesort_optimized(std::vector<T>& a, Stats& st, Compare cmp)
{
    size_t n = a.size();
    if (n <= 1) return;

    std::vector<T> aux_left(n);
    std::vector<T> buffer(n);
    st.aux_bytes += n * sizeof(T) * 2;

    for (size_t start = 0; start < n; start += INSERTION_THRESHOLD) {
        size_t end = std::min(start + INSERTION_THRESHOLD - 1, n - 1);
        insertion_sort(a, start, end, st, cmp);
    }

    std::vector<T>* src = &a;
    std::vector<T>* dst = &buffer;

    for (size_t width = INSERTION_THRESHOLD; width < n; width *= 2) {
        for (size_t left = 0; left < n; left += 2 * width) {
            size_t mid = std::min(left + width - 1, n - 1);
            size_t right = std::min(left + 2 * width - 1, n - 1);

            if (mid >= right) {
                for (size_t i = left; i <= right; ++i) {
                    (*dst)[i] = (*src)[i];
                    st.copies++;
                }
                continue;
            }

            if (!cmp((*src)[mid + 1], (*src)[mid], st)) {
                for (size_t i = left; i <= right; ++i) {
                    (*dst)[i] = (*src)[i];
                    st.copies++;
                }
                continue;
            }

            merge_left_copy(*src, *dst, aux_left, left, mid, right, st, cmp);
        }
        std::swap(src, dst);
    }

    if (src != &a) {
        for (size_t i = 0; i < n; ++i) a[i] = (*src)[i];
        st.copies += n;
    }
}
