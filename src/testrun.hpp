#if !defined(TESTRUN_H)
#define TESTRUN_H

#include "template.hpp"
#include "input.hpp"
#include "reduction.hpp"
#include <filesystem>
#include <map>
#include <set>
namespace fs = std::filesystem;

#define VERBOSE 0

// same run order for different program 
const string TESTSETS[] = {
    "P4E",
    "P4Z",
    "P6E",
    "P6Z",
    "SP", 
    "X",
    "MC",
    "PUC",
    "I080",
    "I160",
    "B",
    "C", 
    "D", 
    "E", 
    // "I320",
    "notestset"
};

// Result format: program | testcase | result | time per line
using MapType = std::map<string,string>;
using SetType = std::set<string>;
using AlgorithmType = std::function<int(std::ofstream&)>;

const SetType TESTS_STRONG({
    "e13", "e18",
    "i080-021", "i080-121", "i080-123", "i080-125",
    "i160-024", "i160-025", "i160-121", "i160-122",
    "p461", "p465", "p466",
    "brasil58", "world666",    
    "null"
});
const SetType TESTS_DEBUG({"p466"});

const SetType SETS_DEBUG({"MC"});
const SetType SETS_PROTOTYPE({"SP", "MC", "X", "P4E"});
const SetType SETS_NEW({"P4E", "P4Z", "P6E", "P6Z", "1R", "2R", "I080", "I160"});
const SetType SETS_GOOD({"P4E", "P4Z", "P6E", "P6Z"});
const SetType SETS_BENCHMARK({"SP", "X", "I080", "I160", "C", "D", "E"});
const SetType SETS_BENCHMARK_ADDITIONAL(SETS_GOOD);
const SetType SETS_ANTI_REDUCE({"SP", "I080", "I160"});

SetType set_diff(SetType A, SetType B) { 
    for (auto elem : B) if (A.count(elem)) A.erase(elem); 
    return A; 
}
SetType set_union(SetType A, SetType B) {
    for (auto elem : B) A.insert(elem);
    return A;
}
SetType set_intersection(SetType A, SetType B) {
    SetType result;
    for (auto a : A) if (B.count(a)) result.insert(a);
    return result;
}

bool check_flag(string testname) {
    return (testname.size() > REDNAME.size() && testname.substr(0, REDNAME.size()) == REDNAME);
}
SetType correct_names(SetType tests) {
    SetType result;
    for (auto &testname : tests)
        result.insert(check_flag(testname) ? testname : REDNAME + "_" + testname);
    return result;
}

void run_tests(
    string program_name, 
    AlgorithmType algorithm, 
    bool run_new_only = false, 
    MapType testset_start = MapType(), 
    SetType included_sets = SetType(),
    SetType excluded_sets = SetType(),
    SetType included_tests = SetType(),
    SetType excluded_tests = SetType(),
    bool append_to_log = false,
    bool preprocessing = false
    ) 
{
    bool IS_RED = (program_name == REDNAME);
    if (!IS_RED && preprocessing) {
        if (program_name.back() != 'R') program_name += "_R";
        included_tests = correct_names(included_tests);
        excluded_tests = correct_names(excluded_tests);
    }

    string activity_log_path = "..\\tests_results\\activity_" + program_name + ".log"; 
    freopen(activity_log_path.c_str(), append_to_log ? "a" : "w", stdout);

    string result_path = "..\\tests_results\\results_" + program_name + ".txt"; 
    std::ofstream resf(result_path, std::ios_base::app);

    resf << "\n_____________________________________________\n";
    cout << "\n_____________________________________________\n";
    cout << "NEW BENCHMARK AT: " << get_date_time() << '\n';
    for (auto testset : TESTSETS) {
        if (excluded_sets.size() && excluded_sets.count(testset)) {
            cout << testset << " is excluded\n";
            continue;
        }
        if (included_sets.size() && !included_sets.count(testset)) {
            cout << testset << " is not included\n";
            continue;
        }
        string dirpath = "..\\tests\\" + testset;
        string start_test = testset_start[testset];
        bool skipped = !start_test.empty();
        try
        {
            for (const auto& entry : fs::directory_iterator(dirpath)) {
                auto ipath = entry.path();
                if (ipath.extension() != ".stp") continue;
                string testname = ipath.filename().replace_extension().string();
                string outf_path = "..\\tests_results\\" + testset 
                    + "\\" + testname + "_" + program_name + ".stp-result";
                string rinpf_path = fs::path(ipath.string()).replace_filename(REDNAME + "_" + testname).replace_extension(".stp").string();
                string opath = IS_RED ? rinpf_path : outf_path;
                bool CONTAIN_FLAG = check_flag(testname);
                // Only care about originial .stp file(s)
                // IF an algorithm runs on pre-processing, and it IS NOT the reducer, then allow flag
                if (!IS_RED) {
                    if (preprocessing != CONTAIN_FLAG) continue;
                }
                else if (CONTAIN_FLAG) continue; 
                
                if (run_new_only && fs::exists(opath)) {
                    if (VERBOSE)
                        std::cerr << "Skipped " << ipath.filename() << " due to run-new flag\n";
                    continue;
                }
                if (testname == start_test) skipped = false;
                if (skipped) {
                    if (VERBOSE)
                        std::cerr << program_name << " " << ipath.filename() << " skipped\n";
                    continue;
                }
                if (included_tests.size() && !included_tests.count(testname)) {
                    if (VERBOSE)
                        std::cerr << program_name << " " << testname << " is not included\n";
                    continue;
                }
                if (excluded_tests.size() && excluded_tests.count(testname)) {
                    if (VERBOSE)
                        std::cerr << program_name << " " << testname << " is excluded\n";
                    continue;
                }
                Real time_input = 0, time_run = 0;
                int cost_read;

                time_input += 
                    benchmark([&] {
                        cost_read = read_input(ipath.string()); 
                    }, 
                    "Input Reading");
                if (cost_read < 0) {
                    if (VERBOSE)
                        std::cerr << "Error reading " << ipath.string() << ". Maybe it does not exist?\n";
                    continue;
                }
                time_input += 
                    benchmark([&] { 
                        initialization();
                        if (preprocessing) {
                            if (IS_RED) { input_preprocessing(); return ; }
                            cout << "I received Reduced input. Yippee Yay!\n";
                        }
                        sp_handler.calc_for(graph);
                    }, 
                    "Computing Shortest Paths");

                std::ofstream outf(opath);
                int optimal = INF;
                time_run = benchmark([&] { optimal = algorithm(outf); }, "Main algorithm");
                outf.close();
                resf << std::fixed << std::setprecision(6);
                resf << program_name << " " << testname << " " << optimal << " " << time_run/1e6 << " " << time_input/1e6 << '\n'; 
                if (IS_RED)
                    resf << "\t" << num_nodes << ' ' << num_edges << ' ' << num_terminals << '\n';
                resf.flush();
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    cout << "End at: " << get_date_time() << '\n';
    cout << "=============================================\n";
}

#endif // TESTRUN_H
