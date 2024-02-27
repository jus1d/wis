#include <iostream>
#include <vector>

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

void run_tests_in_directory(string const& path)
{
    cout << "Running tests for path: " << path << endl;
    return;
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

    for (auto path : paths) run_tests_in_directory(path);
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
    vector<string> paths = {"./tests/", "./euler/", "./examples/"};

    string program = shift_vector(args);

    execute_command(false, "make");

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