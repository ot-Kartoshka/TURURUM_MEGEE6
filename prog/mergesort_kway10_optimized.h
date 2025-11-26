#pragma once
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include "stats.h"

template<typename T, typename Compare>
void mergesort_kway10_optimized(std::vector<T>& a, Stats& st, Compare cmp) {
    const size_t K = 10;
    size_t n = a.size();
    if (n <= 1) return;

    std::vector<std::pair<size_t, size_t>> ranges;
    for (size_t i = 0; i < K; ++i) {
        size_t l = n * i / K;
        size_t r = n * (i + 1) / K;
        if (l < r) ranges.emplace_back(l, r);
    }

    for (auto& pr : ranges) {
        std::sort(a.begin() + pr.first, a.begin() + pr.second,
            [&](const T& x, const T& y) { Stats tmp; return cmp(x, y, tmp); });
        st.copies += pr.second - pr.first;
    }

    struct Node { T val; size_t part; size_t idx; };
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
    for (size_t i = 0; i < ranges.size(); ++i) {
        pos[i] = ranges[i].first;
        if (pos[i] < ranges[i].second) {
            pq.push({ a[pos[i]], i, pos[i] });
            pos[i]++;
            st.copies++;
        }
    }

    std::vector<T> out; out.reserve(n);
    while (!pq.empty()) {
        Node nd = pq.top(); pq.pop();
        out.push_back(nd.val); st.copies++;
        size_t part = nd.part;
        if (pos[part] < ranges[part].second) {
            pq.push({ a[pos[part]], part, pos[part] });
            pos[part]++; st.comparisons++;
        }
    }

    for (size_t i = 0; i < n; ++i) { a[i] = out[i]; st.copies++; }
    st.aux_bytes += n * sizeof(T);
}
