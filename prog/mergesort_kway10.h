#pragma once
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include "stats.h"

template<typename T, typename Compare>
void mergesort_kway10(std::vector<T>& a, Stats& st, Compare cmp) {
    const size_t K = 10;
    size_t n = a.size();
    if (n <= 1) return;

    std::vector<T> aux(n);
    st.aux_bytes += n * sizeof(T);

    std::vector<T>* src = &a;
    std::vector<T>* dst = &aux;

    for (size_t width = 1; width < n; width *= K) {

        for (size_t i = 0; i < n; i += K * width) {

            std::vector<std::pair<size_t, size_t>> ranges;
            for (size_t j = 0; j < K; ++j) {
                size_t l = i + j * width;
                size_t r = std::min(l + width, n);
                if (l < r) {
                    ranges.emplace_back(l, r);
                }
            }

            if (ranges.size() <= 1) {
                if (!ranges.empty()) {
                    std::copy(src->begin() + ranges[0].first, src->begin() + ranges[0].second, dst->begin() + ranges[0].first);
                    st.copies += ranges[0].second - ranges[0].first;
                }
                continue;
            }

            struct Node { T val; size_t part_index; }; 

            struct CmpNode {
                Compare cmp;
                CmpNode(Compare c) : cmp(c) {}
                bool operator()(const Node& A, const Node& B) const {
                    Stats tmp; 
                    return cmp(B.val, A.val, tmp);
                }
            };

            std::priority_queue<Node, std::vector<Node>, CmpNode> pq((CmpNode(cmp)));

            std::vector<size_t> pos(ranges.size());

            for (size_t part = 0; part < ranges.size(); ++part) {
                size_t l = ranges[part].first;
                pos[part] = l + 1;
                pq.push({ (*src)[l], part });
                st.copies++;
            }

            size_t k = i; 
            while (!pq.empty()) {
                Node nd = pq.top();
                pq.pop();

                (*dst)[k++] = nd.val;
                st.copies++;

                size_t part = nd.part_index;

                if (pos[part] < ranges[part].second) {
                    pq.push({ (*src)[pos[part]], part });
                    st.copies++; 
                    pos[part]++;
                }

                if (!pq.empty()) {
                    st.comparisons++;
                }
            }
        }
        std::swap(src, dst);
    }

    if (src != &a) {
        std::copy(src->begin(), src->end(), a.begin());
        st.copies += n;
    }
}