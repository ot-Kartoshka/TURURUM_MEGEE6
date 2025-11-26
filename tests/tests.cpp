#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <random>
#include <functional>
#include <string>

#include "../prog/mergesort_topdown.h"
#include "../prog/mergesort_bottomup.h"
#include "../prog/mergesort_optimized.h"
#include "../prog/mergesort_kway10.h"
#include "../prog/mergesort_kway10_optimized.h"
#include "../prog/stats.h"

auto cmp_int = [](const int& a, const int& b, Stats& st) -> bool {
    st.comparisons++;
    return a < b;
};

auto cmp_gensort = [](const std::string& A, const std::string& B, Stats& st) -> bool {
    st.comparisons++;
    size_t la = std::min<size_t>(10, A.size());
    size_t lb = std::min<size_t>(10, B.size());
    size_t lm = std::min(la, lb);
    int cmp = A.compare(0, lm, B, 0, lm);
    if (cmp != 0) return cmp < 0;
    return la < lb;
};


template <typename SortFunc>
void check_sort(SortFunc sorter)
{
    Stats st;

    {
        std::vector<int> a = { 5, 1, 4, 2, 8 };
        sorter(a, st);
        EXPECT_TRUE(std::is_sorted(a.begin(), a.end()));
    }
    {
        std::vector<int> a = { 1, 2, 3, 4, 5 };
        sorter(a, st);
        EXPECT_TRUE(std::is_sorted(a.begin(), a.end()));
    }
    {
        std::vector<int> a = { 5, 4, 3, 2, 1 };
        sorter(a, st);
        EXPECT_TRUE(std::is_sorted(a.begin(), a.end()));
    }
    {
        std::vector<int> a = {};
        sorter(a, st);
        EXPECT_TRUE(std::is_sorted(a.begin(), a.end()));
    }
}

TEST(SortTest, Topdown)
{
    check_sort([](auto& a, auto& st) { mergesort_topdown<int>(a, st, cmp_int); });
}

TEST(SortTest, Bottomup)
{
    check_sort([](auto& a, auto& st) { mergesort_bottomup<int>(a, st, cmp_int); });
}

TEST(SortTest, Optimized)
{
    check_sort([](auto& a, auto& st) { mergesort_optimized<int>(a, st, cmp_int); });
}

TEST(SortTest, KWay10)
{
    check_sort([](auto& a, auto& st) { mergesort_kway10<int>(a, st, cmp_int); });
}

TEST(SortTest, KWay10_Optimized)
{
    check_sort([](auto& a, auto& st) { mergesort_kway10_optimized<int>(a, st, cmp_int); });
}


template <typename Compare>
bool is_sorted_by_cmp(const std::vector<std::string>& arr, Compare cmp)
{
    Stats tmp;
    for (size_t i = 1; i < arr.size(); ++i) {
        if (cmp(arr[i], arr[i - 1], tmp)) return false;
    }
    return true;
}

template <typename SortFunc>
void check_sort_string(SortFunc sorter)
{
    Stats st;

    {
        std::vector<std::string> a = { "hello", "hel", "helloo", "he", "world", "hell" };
        sorter(a, st);
        EXPECT_TRUE(is_sorted_by_cmp(a, cmp_gensort));
    }
    {
        std::vector<std::string> a = { "aaaaaaa", "aaaaaaaa", "aaaa", "aa", "a" };
        sorter(a, st);
        EXPECT_TRUE(is_sorted_by_cmp(a, cmp_gensort));
    }
    {
        std::vector<std::string> a = {};
        sorter(a, st);
        EXPECT_TRUE(is_sorted_by_cmp(a, cmp_gensort));
    }
}

TEST(SortString, Topdown)
{
    check_sort_string([](auto& a, auto& st) { mergesort_topdown<std::string>(a, st, cmp_gensort); });
}

TEST(SortString, Bottomup)
{
    check_sort_string([](auto& a, auto& st) { mergesort_bottomup<std::string>(a, st, cmp_gensort); });
}

TEST(SortString, Optimized)
{
    check_sort_string([](auto& a, auto& st) { mergesort_optimized<std::string>(a, st, cmp_gensort); });
}

TEST(SortString, KWay10)
{
    check_sort_string([](auto& a, auto& st) { mergesort_kway10<std::string>(a, st, cmp_gensort); });
}

TEST(SortString, KWay10_Optimized)
{
    check_sort_string([](auto& a, auto& st) { mergesort_kway10_optimized<std::string>(a, st, cmp_gensort); });
}

TEST(SortExtended, AllEqualElements)
{
    std::vector<int> a(200, 42);
    auto expected = a;
    std::sort(expected.begin(), expected.end());
    Stats st;
    mergesort_optimized<int>(a, st, cmp_int);
    EXPECT_EQ(expected, a);
}

TEST(SortExtended, FewUniqueValues)
{
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> d(0, 5);
    size_t n = 1000;
    std::vector<int> base(n);
    for (size_t i = 0; i < n; ++i) base[i] = d(rng);
    auto expected = base;
    std::sort(expected.begin(), expected.end());

    std::vector<std::pair<std::string, std::function<void(std::vector<int>&, Stats&)>>> algos = {
        {"topdown", [](std::vector<int>& arr, Stats& st) { mergesort_topdown<int>(arr, st, cmp_int); }},
        {"bottomup", [](std::vector<int>& arr, Stats& st) { mergesort_bottomup<int>(arr, st, cmp_int); }},
        {"optimized", [](std::vector<int>& arr, Stats& st) { mergesort_optimized<int>(arr, st, cmp_int); }},
        {"kway10", [](std::vector<int>& arr, Stats& st) { mergesort_kway10<int>(arr, st, cmp_int); }},
        {"kway10_optimized", [](std::vector<int>& arr, Stats& st) { mergesort_kway10_optimized<int>(arr, st, cmp_int); }}
    };

    for (auto& p : algos) {
        auto arr = base;
        Stats st;
        p.second(arr, st);
        EXPECT_EQ(expected, arr) << "Algorithm: " << p.first;
    }
}

TEST(SortExtended, RandomVariousSizes)
{
    std::vector<std::pair<std::string, std::function<void(std::vector<int>&, Stats&)>>> algos = {
        {"topdown", [](std::vector<int>& arr, Stats& st) { mergesort_topdown<int>(arr, st, cmp_int); }},
        {"bottomup", [](std::vector<int>& arr, Stats& st) { mergesort_bottomup<int>(arr, st, cmp_int); }},
        {"optimized", [](std::vector<int>& arr, Stats& st) { mergesort_optimized<int>(arr, st, cmp_int); }},
        {"kway10", [](std::vector<int>& arr, Stats& st) { mergesort_kway10<int>(arr, st, cmp_int); }},
        {"kway10_optimized", [](std::vector<int>& arr, Stats& st) { mergesort_kway10_optimized<int>(arr, st, cmp_int); }}
    };

    std::mt19937 rng(2021);
    std::vector<size_t> sizes = { 1,2,3,31,32,33,63,64,65,100,1000,10000 };
    for (size_t n : sizes) {
        std::vector<int> base(n);
        std::uniform_int_distribution<int> d(0, (int)std::max<size_t>(1, n));
        for (size_t i = 0; i < n; ++i) base[i] = d(rng);

        auto expected = base;
        std::sort(expected.begin(), expected.end());

        for (auto& p : algos) {
            auto arr = base;
            Stats st;
            p.second(arr, st);
            EXPECT_EQ(expected, arr) << "Algorithm: " << p.first << " n=" << n;
        }
    }
}

TEST(SortExtended, LargeRandom50000)
{
    std::mt19937 rng(1234);
    size_t n = 50000;
    std::vector<int> base(n);
    std::uniform_int_distribution<int> d(0, (int)n);
    for (size_t i = 0; i < n; ++i) base[i] = d(rng);

    auto expected = base;
    std::sort(expected.begin(), expected.end());

    Stats st;
    auto arr = base;
    mergesort_optimized<int>(arr, st, cmp_int);
    EXPECT_EQ(expected, arr);
}