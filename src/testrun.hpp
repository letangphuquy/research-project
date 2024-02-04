#if !defined(TESTRUN_H)
#define TESTRUN_H

#include "template.hpp"
#include "input.hpp"
#include <filesystem>
#include <map>
#include <set>
namespace fs = std::filesystem;

// same run order for different program 
const string TESTSETS[] = {
    "B",
    "C", 
    "D", 
    "SP", 
    "E", 
    "MC",
    "X",
    "PUC",
    "notestset"
};

// Result format: program | testcase | result | time per line
using MapType = std::map<string,string>;
using SetType = std::set<string>;
using AlgorithmType = std::function<int(std::ofstream&)>;

void run_tests(string program_name, AlgorithmType algorithm, bool run_new_only = false, MapType testset_start = MapType(), SetType excluded_tests = SetType(), SetType included_tests = SetType()) {
    string activity_log_path = "..\\tests_results\\activity_" + program_name + ".log"; 
    freopen(activity_log_path.c_str(), "w", stdout);

    string result_path = "..\\tests_results\\results_" + program_name + ".txt"; 
    std::ofstream resf(result_path);

    cout << "\n_____________________________________________\n";
    cout << "NEW BENCHMARK AT: " << get_date_time() << '\n';
    for (auto testset : TESTSETS) {
        string dirpath = "..\\tests\\" + testset;
        string start_test = testset_start[testset];
        bool skipped = !start_test.empty();
        for (const auto& entry : fs::directory_iterator(dirpath)) {
            auto path = entry.path();
            if (path.extension() != ".stp") continue;
            string testname = path.filename().replace_extension().string();
            string outf_path = "..\\tests_results\\" + testset 
                + "\\" + path.filename().concat("_" + program_name).replace_extension(".stp-result").string();
            if (run_new_only && fs::exists(outf_path)) {
                cout << "Skipped " << path.filename() << " due to run-new flag\n";
                continue;
            }
            if (testname == start_test) skipped = false;
            if (skipped) {
                cout << path.filename() << " skipped\n";
                continue;
            }
            if (included_tests.size() && !included_tests.count(testname)) {
                cout << testname << " is not included\n";
                continue;
            }
            if (excluded_tests.size() && excluded_tests.count(testname)) {
                cout << testname << " is excluded\n";
                continue;
            }
            Real time_input, time_run;
            time_input += benchmark([&] { read_input(path.string()); }, "Input Reading");
            bool can_do;
            time_input += benchmark([&] { can_do = input_preprocessing(); }, "Input Preprocessing");
            if (!can_do) {
                cout << "Couldn't get all-pair shortest paths. STP instance " + testname + "skipped\n";
            } else {
                std::ofstream outf(outf_path);
                int optimal = INF;
                time_run = benchmark([&] { optimal = algorithm(outf); }, "Main algorithm");
                outf.close();
                resf << program_name << " " << testname << " " << optimal << " " << time_run << " " << time_input << '\n'; 
            }
        }
    }
    cout << "End at: " << get_date_time() << '\n';
    cout << "=============================================\n";
}

#endif // TESTRUN_H
