#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
#include <map>
#include <string>

using namespace std;

const string FILE_EXTENSION = ".glo";

void execute_command(bool silent_mode, const string& command)
{
    if (!silent_mode) cout << "[CMD] " << command << endl;
    int exit_code = system(command.c_str());
    if (exit_code != 0)
    {
        cerr << "ERROR: Executing command crashed with " << to_string(exit_code) << " exit code" << endl;
        exit(exit_code);
    }
}

string shift_vector(vector<string>& vec)
{
    if (!vec.empty()) {
        string result = vec[0];

        vec.erase(vec.begin());

        return result;
    }
    cerr << "ERROR: Can't shift empty vector" << endl;
    exit(1);
}

void test_file(const string& filePath) {
    string file_name = filePath.substr(0, filePath.find_last_of('.'));

    map<string, string> commands = {
            {"run", "./gollo run " + filePath},
            {"compile", "./gollo compile -s " + filePath + " && ./" + file_name}
    };

    map <string, int> errors = {
            {"run", 0},
            {"compile", 0}
    };

    for (auto const& command : commands)
    {
        FILE* pipe = popen(command.second.c_str(), "r");
        if (!pipe) {
            cerr << "Error executing command: " << command.second << endl;
            return;
        }

        string output;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }

        pclose(pipe);

        string outputPath = file_name + ".output";

        ifstream outputFile(outputPath);
        string expectedOutput((istreambuf_iterator<char>(outputFile)), istreambuf_iterator<char>());

        if (output == expectedOutput) {
            cout << "[" << command.first << "] Test passed for file: " << filePath << endl;
        } else {
            cerr << "[" << command.first << "] Test failed for file: " << filePath << endl;
            cerr << "  Expected output:\n" << expectedOutput << "\n  Actual output:\n" << output << endl;
        }
    }
}

void run_tests_in_directory(const string& directoryPath) {
    for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
        if (entry.path().extension() == FILE_EXTENSION) {
            test_file(entry.path().string());
        }
    }
}

void run_tests(vector<string> args, vector<string> paths)
{
    int i = 0;
    while (i < args.size())
    {
        string arg = shift_vector(args);
        if (arg == "-f")
        {
            if (args.empty())
            {
                cerr << "ERROR: path to directory not found after `-f` flag" << endl;
                exit(1);
            }

            paths.push_back(args[0]);
        }
    }

    for (const auto& path : paths) run_tests_in_directory(path);
}

void record_test_output(string const& file_path)
{
    string file_name = file_path.substr(0, file_path.length() - FILE_EXTENSION.length());

    execute_command(false, "./gollo compile " + file_path);

    execute_command(false, "./" + file_name + " > " + file_name + ".output");
}

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);
    vector<string> paths = {"./tests/"};

    string program = shift_vector(args);

    execute_command(false, "make build");

    if (args.empty())
    {
        cerr << "ERROR: no subcommand provided" << endl;
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
            cerr << "ERROR: not enough arguments for `record` subcommand" << endl;
            exit(1);
        }
        record_test_output(shift_vector(args));
    }
    else
    {
        cerr << "ERROR: unknown subcommand provided" << endl;
        exit(1);
    }

    return 0;
}