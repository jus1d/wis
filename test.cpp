#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include <map>
#include <string>

using std::string, std::cout, std::cerr, std::endl;

const string FILE_EXTENSION = ".glo";

inline bool is_file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}


void execute_command(bool silent_mode, const string& command)
{
    if (!silent_mode) cout << "[CMD] " << command << endl;
    int exit_code = system(command.c_str());
    if (exit_code != 0)
    {
        cerr << "[ERROR] Executing command crashed with " << std::to_string(exit_code) << " exit code" << endl;
        exit(exit_code);
    }
}

string shift_vector(std::vector<string>& vec)
{
    if (!vec.empty()) {
        string result = vec[0];

        vec.erase(vec.begin());

        return result;
    }
    cerr << "[ERROR] Can't shift empty vector" << endl;
    exit(1);
}

int test_file(const string& file_path) {
    string file_name = file_path.substr(0, file_path.find_last_of('.'));

    string output_path = file_name + ".output";

    if (!is_file_exists(output_path))
    {
        cout << "[INFO] Skipping test for file: " << file_path << ". No recorded output found" << endl;
        return 1;
    }

    string command = "./gollo -quiet " + file_path + " && ./" + file_name;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "ERROR: Command execution failed: " << command << endl;
        return -1;
    }

    string output;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) output += buffer;

    pclose(pipe);

    std::ifstream outputFile(output_path);
    string expected_output((std::istreambuf_iterator<char>(outputFile)), std::istreambuf_iterator<char>());

    if (output == expected_output) {
        cout << "[INFO] Test passed for file: " << file_path << endl;
        return 0;
    } else {
        cerr << "[ERROR] Test failed for file: " << file_path << endl;
        cerr << "  Expected output:\n" << expected_output << "\n  Actual output:\n" << output << endl;
        return -1;
    }
}

void run_tests(std::vector<string> args, std::vector<string> paths)
{
    int i = 0;
    while (i < args.size())
    {
        string arg = shift_vector(args);
        if (arg == "-f")
        {
            if (args.empty())
            {
                cerr << "[ERROR] path to directory not found after `-f` flag" << endl;
                exit(1);
            }

            paths.push_back(args[0]);
        }
    }

    int failed = 0;
    int passed = 0;
    int skipped = 0;

    for (const auto& path : paths)
    {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() == FILE_EXTENSION) {
                int result = test_file(entry.path().string());

                if (result == -1) failed++;
                else if (result == 1) skipped++;
                else passed++;
            }
        }
    }

    cout << endl << "Testing report:" << endl;
    cout << "  Tests passed: " << passed << ", Tests skipped: " << skipped << ", Tests failed: " << failed << endl;

    if (failed > 0) exit(1);
}

void record_test_output(string const& file_path)
{
    string file_name = file_path.substr(0, file_path.length() - FILE_EXTENSION.length());

    execute_command(false, "./gollo " + file_path);

    execute_command(false, "./" + file_name + " > " + file_name + ".output");
}

int main(int argc, char* argv[])
{
    std::vector<string> args(argv, argv + argc);
    std::vector<string> paths = {"./tests/"};

    string program = shift_vector(args);

    if (args.empty())
    {
        cerr << "[ERROR] no subcommand provided" << endl;
        exit(1);
    }

    string subcommand = shift_vector(args);

    if (subcommand == "run")
    {
        run_tests(args, paths);
    }
    else if (subcommand == "record")
    {
        if (args.empty())
        {
            cerr << "[ERROR] not enough arguments for `record` subcommand" << endl;
            exit(1);
        }
        record_test_output(shift_vector(args));
    }
    else
    {
        cerr << "[ERROR] unknown subcommand provided" << endl;
        exit(1);
    }

    return 0;
}