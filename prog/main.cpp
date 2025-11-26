#include <iostream>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

#include "cli.h"
#include "stats.h"
#include "timer.h"
#include "generators.h"
#include "file_input.h"

#include "mergesort_topdown.h"
#include "mergesort_bottomup.h"
#include "mergesort_optimized.h"
#include "mergesort_kway10.h"
#include "mergesort_kway10_optimized.h"
#include "brute_force.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char** argv) {
    Config cfg = parse_arguments(argc, argv);

    std::mt19937 rng;
    if (cfg.seed_provided) rng.seed((unsigned)cfg.seed);
    else rng.seed(std::random_device{}());

    fs::create_directories("benchmarks");
    ofstream fout(cfg.out);
    if (!fout) {
        cerr << "Cannot open output CSV: " << cfg.out << "\n";
        return 1;
    }
    fout << "variant,size,kind,rep,elapsed_ms,comparisons,copies,aux_bytes\n";

    if (cfg.run_bruteforce) {
        auto [mn, pmin, mx, pmax] = bruteforce_full(cfg.bf_n);
        cout << "Bruteforce n=" << cfg.bf_n << "\n";
        cout << "Min comparisons: " << mn << "\nPermutation: ";
        for (int x : pmin) cout << x << " ";
        cout << "\nMax comparisons: " << mx << "\nPermutation: ";
        for (int x : pmax) cout << x << " ";
        cout << "\n";
        return 0;
    }

    if (cfg.use_file) {
        vector<string> base = read_gensort_file(cfg.input_file);
        cout << "Loaded " << base.size() << " records from file: " << cfg.input_file << "\n";

        auto cmp_gensort = [](const string& A, const string& B, Stats& st) -> bool {
            st.comparisons++;
            size_t la = std::min<size_t>(10, A.size());
            size_t lb = std::min<size_t>(10, B.size());
            int cmp = A.compare(0, min(la, lb), B, 0, min(la, lb));
            if (cmp != 0) return cmp < 0;
            return la < lb;
            };

        auto run_variant_string = [&](
            const function<void(vector<string>&, Stats&)>& fn,
            const string& name
            ) {
                vector<string> arr = base;
                Stats st; st.reset();

                auto t0 = steady_clock_t::now();
                fn(arr, st);
                auto t1 = steady_clock_t::now();
                double ms = ms_between(t0, t1);

                bool ok = true;
                for (size_t i = 1; i < arr.size(); ++i) {
                    Stats tmp;
                    if (cmp_gensort(arr[i], arr[i - 1], tmp)) { ok = false; break; }
                }
                if (!ok) cerr << "ERROR: not sorted (file): " << name << "\n";

                fout << name << "," << arr.size() << ",gensort,0," << ms << "," << st.comparisons << "," << st.copies << "," << st.aux_bytes << "\n";
                cout << name << ": " << ms << " ms, comps=" << st.comparisons << ", copies=" << st.copies << "\n";

                if (cfg.save_sorted_file) {
                    string outname = cfg.sorted_out;
                    if (base.size() > 1) { 
                        size_t p = outname.find_last_of('.');
                        string newname;

                        if (p == string::npos) newname = outname + "_" + name;

                        else newname = outname.substr(0, p) + "_" + name + outname.substr(p);
                        save_gensort_file(newname, arr);
                        cout << "Saved sorted file: " << newname << "\n";
                    }
                    else {
                        save_gensort_file(outname, arr);
                        cout << "Saved sorted file: " << outname << "\n";
                    }
                }
            };

        run_variant_string([&](vector<string>& arr, Stats& st) { mergesort_topdown<string>(arr, st, cmp_gensort); }, "topdown");
        run_variant_string([&](vector<string>& arr, Stats& st) { mergesort_bottomup<string>(arr, st, cmp_gensort); }, "bottomup");
        run_variant_string([&](vector<string>& arr, Stats& st) { mergesort_optimized<string>(arr, st, cmp_gensort); }, "optimized");
        run_variant_string([&](vector<string>& arr, Stats& st) { mergesort_kway10<string>(arr, st, cmp_gensort); }, "kway10");
        run_variant_string([&](vector<string>& arr, Stats& st) { mergesort_kway10_optimized<string>(arr, st, cmp_gensort); }, "kway10_optimized");


        cout << "File sorting results saved to " << cfg.out << "\n";
        return 0;
    }

    vector<size_t> sizes;
    try {
  
        size_t custom_size = std::stoull(cfg.mode);

        sizes.push_back(custom_size);
    }
    catch (const std::invalid_argument&) {
        sizes = (cfg.mode == "smoke") ? vector<size_t>{100, 1000} : vector<size_t>{ 100, 1000, 10000, 50000 };
    }

    vector<DataType> kinds;
    bool bad_kind_found = false;

    if (cfg.kinds.empty()) {
        kinds = { DataType::RANDOM, DataType::SORTED, DataType::REVERSED, DataType::NEARLY_SORTED, DataType::FEW_UNIQUE };
    }
    else {
        for (const string& k_str : cfg.kinds) {
            if (k_str == "random") kinds.push_back(DataType::RANDOM);
            else if (k_str == "sorted") kinds.push_back(DataType::SORTED);
            else if (k_str == "reversed") kinds.push_back(DataType::REVERSED);
            else if (k_str == "nearly_sorted") kinds.push_back(DataType::NEARLY_SORTED);
            else if (k_str == "few_unique") kinds.push_back(DataType::FEW_UNIQUE);
            else {
                cerr << "Error: Unknown kind '" << k_str << "'.\n";
                bad_kind_found = true; 
            }
        }
    }

    if (bad_kind_found || (kinds.empty() && !cfg.kinds.empty())) {

        std::cout << "\nPlease specify valid data kinds using --kind name,name...\n"
        "Available kinds are: random, sorted, reversed, nearly_sorted, few_unique\n";

        return 1;
    }

    auto cmp_int = [](const int& a, const int& b, Stats& st) -> bool {
        st.comparisons++;
        return a < b;
        };

    auto run_variant_int = [&](const std::function<void(std::vector<int>&, Stats&)>& fn, const string& name,
        const vector<int>& base, size_t n, DataType kind, int rep) {
            vector<int> arr = base;
            Stats st; st.reset();

            auto t0 = steady_clock_t::now();
            fn(arr, st);
            auto t1 = steady_clock_t::now();
            double ms = ms_between(t0, t1);

            if (!is_sorted(arr.begin(), arr.end())) cerr << "Error: not sorted: " << name << " n=" << n << "\n";
            fout << name << "," << n << "," << data_type_name(kind) << "," << rep << "," << ms << "," << st.comparisons << "," << st.copies << "," << st.aux_bytes << "\n";


            if (cfg.save_sorted_file) {
                string outname = cfg.sorted_out;
                if (base.size() > 1) {
                    size_t p = outname.find_last_of('.');
                    string newname;
                    string suffix = "_" + name + "_n" + to_string(n) + "_" + data_type_name(kind) + "_rep" + to_string(rep);

                    if (p == string::npos) newname = outname + suffix;

                    else newname = outname.substr(0, p) + suffix + outname.substr(p);
                    save_int_file(newname, arr);
                    cout << "Saved sorted file: " << newname << "\n";
                }
                else {
                    save_int_file(outname, arr);
                    cout << "Saved sorted file: " << outname << "\n";
                }
            }

        };

    for (size_t n : sizes) {
        for (int kind_i = 0; kind_i < (int)kinds.size(); ++kind_i) {
            DataType kind = kinds[kind_i];
            for (int rep = 0; rep < cfg.repeat; ++rep) {
                vector<int> base = generate_data(n, kind, rng);

                run_variant_int([&](vector<int>& arr, Stats& st) { mergesort_topdown<int>(arr, st, cmp_int); }, "topdown", base, n, kind, rep);
                run_variant_int([&](vector<int>& arr, Stats& st) { mergesort_bottomup<int>(arr, st, cmp_int); }, "bottomup", base, n, kind, rep);
                run_variant_int([&](vector<int>& arr, Stats& st) { mergesort_optimized<int>(arr, st, cmp_int); }, "optimized", base, n, kind, rep);
                run_variant_int([&](vector<int>& arr, Stats& st) { mergesort_kway10<int>(arr, st, cmp_int); }, "kway10", base, n, kind, rep);
                run_variant_int([&](vector<int>& arr, Stats& st) { mergesort_kway10_optimized<int>(arr, st, cmp_int); }, "kway10_optimized", base, n, kind, rep);
            }
        }
    }

    cout << "Results saved to " << cfg.out << "\n";
    return 0;
}
