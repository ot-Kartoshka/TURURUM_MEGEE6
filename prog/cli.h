#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

struct Config {
    std::string out = "./benchmarks/results.csv";

    int repeat = 5;
    std::string mode = "all";

    std::vector<std::string> variants;

    bool run_bruteforce = false;
    int bf_n = 8;

    bool use_file = false;
    std::string input_file;

    bool save_sorted_file = false;
    std::string sorted_out = "./benchmarks/sorted.dat";

    unsigned long long seed = 0;
    bool seed_provided = false;

    double max_seconds = 0.0;
    std::vector<std::string> kinds;
};

inline void print_usage() {
    std::cout <<
        "Usage: prog [options]\n\n"
        "Options:\n"
        "  --out FILE              Output CSV with benchmark results\n"
        "  --repeat N              Number of repetitions (default: 5)\n"
        "  --mode smoke|all|N      Benchmark mode or a single custom size\n"
        "  --variants v1,v2,...    Limit used algorithms(Not working)\n"
        "  --kind TYPE,TYPE,...     Comma-separated list of array types (random, sorted...)\n"
        "  --bruteforce N          Run bruteforce search for N\n"
        "  --file input.dat        Sort values from file\n"
        "  --save-sorted out.dat   Save sorted array to file\n"
        "  --seed N                RNG seed (default: random)\n"
        "  --max-seconds T         Limit max time per experiment (Not working)\n";
}

inline Config parse_arguments(int argc, char** argv) {
    Config cfg;

    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];

        if (s == "--out" && i + 1 < argc) {
            cfg.out = argv[++i];
        }
        else if (s == "--repeat" && i + 1 < argc) {
            cfg.repeat = std::stoi(argv[++i]);
        }
        else if (s == "--mode" && i + 1 < argc) {
            cfg.mode = argv[++i];
        }
        else if (s == "--variants" && i + 1 < argc) {
            std::stringstream ss(argv[++i]);
            std::string tmp;
            while (std::getline(ss, tmp, ',')) {
                if (!tmp.empty())
                    cfg.variants.push_back(tmp);
            }
        }
        else if (s == "--kind" && i + 1 < argc) {
            std::stringstream ss(argv[++i]);
            std::string tmp;
            while (std::getline(ss, tmp, ',')) {
                if (!tmp.empty())
                    cfg.kinds.push_back(tmp);
            }
        }
        else if (s == "--bruteforce" && i + 1 < argc) {
            cfg.run_bruteforce = true;
            cfg.bf_n = std::stoi(argv[++i]);
        }
        else if (s == "--file" && i + 1 < argc) {
            cfg.use_file = true;
            cfg.input_file = argv[++i];
        }
        else if (s == "--save-sorted" && i + 1 < argc) {
            cfg.save_sorted_file = true;
            cfg.sorted_out = argv[++i];
        }
        else if (s == "--seed" && i + 1 < argc) {
            cfg.seed = std::stoull(argv[++i]);
            cfg.seed_provided = true;
        }
        else if (s == "--max-seconds" && i + 1 < argc) {
            cfg.max_seconds = std::stod(argv[++i]);
        }
        else {
            std::cout << "Unknown option: " << s << "\n";
            print_usage();
        }
    }

    return cfg;
}
