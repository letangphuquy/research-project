#if !defined(TESTRUN_H)
#define TESTRUN_H

#include "template.hpp"
#include "input.hpp"
#include <filesystem>
#include <map>
#include <set>
namespace fs = std::filesystem;

#define VERBOSE 0

// same run order for different program 
const string TESTSETS[] = {
    "SP", 
    "E", 
    "MC",
    "X",
    "PUC",
    "I080",
    "I160",
    "P4E",
    "P4Z",
    "P6E",
    "P6Z",
    "B",
    "C", 
    "D", 
    // "I320",
    "notestset"
};

// Result format: program | testcase | result | time per line
using MapType = std::map<string,string>;
using SetType = std::set<string>;
using AlgorithmType = std::function<int(std::ofstream&)>;

const SetType TESTS_GOOD({
    "c03", "c04", "c08", "c10", "c19", "c20"
    "d03", "d04", "d08", "d09", "d18", "d20",
    "e13", "e18", "e20",
    "w13c29", "w23c23", "w3c571",
    "berlin52", "brasil58"
});
const SetType TESTS_DEBUG({"p461"});

const SetType SETS_NEW({"P4E", "P4Z", "P6E", "P6Z", "1R", "2R", "I080", "I160"});
const SetType SETS_GOOD({"P4E", "P4Z", "P6E", "P6Z"});
const SetType SETS_BENCHMARK({"SP", "X", "I080", "I160", "C", "D", "E"});

void run_tests(
    string program_name, 
    AlgorithmType algorithm, 
    bool run_new_only = false, 
    MapType testset_start = MapType(), 
    SetType included_sets = SetType(),
    SetType excluded_sets = SetType(),
    SetType included_tests = SetType(),
    SetType excluded_tests = SetType(),
    bool append_to_log = false
    ) 
{
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
                auto path = entry.path();
                if (path.extension() != ".stp") continue;
                string testname = path.filename().replace_extension().string();
                string outf_path = "..\\tests_results\\" + testset 
                    + "\\" + testname + "_" + program_name + ".stp-result";
                if (run_new_only && fs::exists(outf_path)) {
                    if (VERBOSE)
                        std::cerr << "Skipped " << path.filename() << " due to run-new flag\n";
                    continue;
                }
                if (testname == start_test) skipped = false;
                if (skipped) {
                    if (VERBOSE)
                        std::cerr << program_name << " " << path.filename() << " skipped\n";
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
                time_input += benchmark([&] { read_input(path.string()); }, "Input Reading");
                bool can_do;
                time_input += benchmark([&] { can_do = initialization(); }, "Compute Shortest Paths");
                if (!can_do) {
                    cout << "Couldn't get all-pair shortest paths. STP instance " + testname + "skipped\n";
                } else {
                    std::ofstream outf(outf_path);
                    int optimal = INF;
                    time_run = benchmark([&] { optimal = algorithm(outf); }, "Main algorithm");
                    outf.close();
                    resf << std::fixed << std::setprecision(6);
                    resf << program_name << " " << testname << " " << optimal << " " << time_run/1e6 << " " << time_input/1e6 << '\n'; 
                    resf.flush();
                }
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
