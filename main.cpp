#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>

using namespace std;

enum class OpType : int {
    PUSH_INT,
    PUSH_STRING,
    PLUS,
    PUT,
    COUNT,
};

enum class TokenType : int {
    WORD,
    INT,
    STRING,
    COUNT,
};

class Token {
public:
    TokenType Type;
    string StringValue;
    int IntegerValue;
    string Loc;

    Token(TokenType type, int integer_value, string  loc)
            : Type(type), IntegerValue(integer_value), Loc(std::move(loc)) {}

    Token(TokenType type, string  string_value, string  loc)
            : Type(type), StringValue(std::move(string_value)), IntegerValue(0), Loc(std::move(loc)) {}
};

class Operation {
public:
    OpType Type;
    int IntegerValue;
    string StringValue;
    string Loc;

    Operation(OpType type, string loc)
            : Type(type), IntegerValue(0), Loc(std::move(loc)) {}

    Operation(OpType type, int integer_value, string loc)
        : Type(type), IntegerValue(integer_value), Loc(std::move(loc)) {}

    Operation(OpType type, string string_value, string loc)
            : Type(type), IntegerValue(0), StringValue(std::move(string_value)), Loc(std::move(loc)) {}
};

void assert(bool condition, string const& message)
{
    if (!condition)
    {
        cerr << "ASSERTION ERROR: " << message << endl;
        exit(1);
    }
}

void usage(string const& compiler_path)
{
    cerr << "Usage: " << compiler_path << " <SUBCOMMAND> ./examples/goo.glo" << endl;
    cerr << "SUBCOMMANDS:" << endl;
    cerr << "    run                 Instantly run the program" << endl;
    cerr << "    compile [ARGS]      Compile the program into an executable" << endl;
    cerr << "      ARGS:" << endl;
    cerr << "          -r            Run compiled program after compilation" << endl;
}

string shift_vector(vector<string>& vec)
{
    if (!vec.empty()) {
        string result = vec[0];

        vec.erase(vec.begin());

        return result;
    } else {
        cerr << "ERROR: Can't shift empty vector" << endl;
        return "";
    }
}

string location_view(const string& filepath, int row, int col)
{
    return filepath + ":" + to_string(row) + ":" + to_string(col);
}

int find_col(string const& line, int start, bool (*predicate)(char))
{
    while (start < line.size() && !predicate(line[start])) {
        start++;
    }
    return start;
}

vector<Token> lex_line(string const& filepath, int line_number, string const& line)
{
    vector<Token> tokens;

    int col = find_col(line, 0, [](char x) { return !isspace(x); });
    while (col < line.size())
    {
        int col_end;
        if (line[col] == '"')
        {
            col_end = find_col(line, col+1, [](char x) { return x == '"'; });
            assert(line[col_end] == '"', "Closing string quote symbol expected");

            string text_of_token = line.substr(col + 1, col_end - col - 1);
            string loc = location_view(filepath, line_number, col + 1);
            Token token(TokenType::STRING, text_of_token, loc);
            tokens.push_back(token);
            col = find_col(line, col_end + 1, [](char x) { return !isspace(x); });
        }
        else
        {
            col_end = find_col(line, col, [](char x) { return !!isspace(x); });
            string text_of_token = line.substr(col, col_end - col);

            try {
                int int_value = stoi(text_of_token);

                string loc = location_view(filepath, line_number, col + 1);
                Token token(TokenType::INT, int_value, loc);
                tokens.push_back(token);
            }
            catch (const invalid_argument&) {
                string loc = location_view(filepath, line_number, col + 1);
                Token token(TokenType::WORD, text_of_token, loc);
                tokens.push_back(token);
            }
            col = find_col(line, col_end, [](char x) { return !isspace(x); });
        }
    }

    return tokens;
}

vector<Operation> parse_tokens_as_operations(const vector<Token>& tokens)
{
    vector<Operation> program;

    assert(int(TokenType::COUNT) == 3, "Exhaustive token types handling in parse_tokens_as_operations()");

    for (const auto& token: tokens) {
        switch (token.Type) {
            case TokenType::INT:
                program.emplace_back(OpType::PUSH_INT, token.IntegerValue, token.Loc);
                break;
            case TokenType::STRING:
                program.emplace_back(OpType::PUSH_STRING, token.StringValue, token.Loc);
                break;
            case TokenType::WORD:
                assert(int(OpType::COUNT) == 4, "Exhaustive operations handling in parse_tokens_as_operations()");

                if (token.StringValue == "+")
                {
                    program.emplace_back(OpType::PLUS, token.Loc);
                }
                else if (token.StringValue == "put")
                {
                    program.emplace_back(OpType::PUT, token.Loc);
                }
                else
                {
                    cerr << token.Loc << ": ERROR: Unexpected token: `" << token.StringValue << "`" << endl;
                    exit(1);
                }
                break;
            default:
                assert(false, "unreachable");
        }
    }

    return program;
}

void crossreference_blocks(vector<Operation>& program)
{
    assert(int(OpType::COUNT) == 4, "Exhaustive operations handling in crossreference_blocks(). "
                                    "Not all operations should be handled in here.");
}

vector<Operation> lex_file(string const& path)
{
    fstream file(path);

    if (!file.is_open())
    {
        cerr << "ERROR: Can't open file." << endl;
        exit(1);
    }

    vector<Token> tokens;

    string line;
    int line_number = 1;
    while (getline(file, line))
    {
        vector<Token> line_tokens = lex_line(path, line_number, line);

        for (const auto & line_token : line_tokens) {
            tokens.push_back(line_token);
        }
        line_number++;
    }

    vector<Operation> program = parse_tokens_as_operations(tokens);

    crossreference_blocks(program);

    file.close();

    return program;
}

void run_program(vector<Operation> program)
{
    assert(int(OpType::COUNT) == 4, "Exhaustive operations handling in run_program()");

    stack<int> runtime_stack;

    for (int i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                runtime_stack.push(op.IntegerValue);
                break;
            }
            case OpType::PUSH_STRING:
            {
                assert(false, "Pushing strings not implemented yet");
                break;
            }
            case OpType::PLUS:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a + b);
                break;
            }
            case OpType::PUT: {
                int val = runtime_stack.top();
                runtime_stack.pop();
                cout << to_string(val) << endl;
                break;
            }
            default:
                assert(false, "unreachable");
        }
    }
}

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);
    string compiler_path = shift_vector(args);

    if (args.empty())
    {
        usage(compiler_path);
        cerr << "ERROR: No subcommand provided" << endl;
        exit(1);
    }

    string subcommand = shift_vector(args);

    if (subcommand == "run")
    {
        if (args.empty())
        {
            usage(compiler_path);
            cerr << "ERROR: No path to file provided" << endl;
            exit(1);
        }

        filesystem::path file_path(shift_vector(args));

        if (!filesystem::exists(file_path))
        {
            usage(compiler_path);
            cerr << "ERROR: File " << file_path.string() << " doesn't exists" << endl;
            exit(1);
        }

        vector<Operation> program = lex_file(file_path.string());

        run_program(program);
    }
    else if (subcommand == "compile")
    {
        assert(false, "Compilation mode not implemented yet");
    }
    else
    {
        usage(compiler_path);
        cerr << "ERROR: Provided unknown subcommand: " << subcommand << endl;
        exit(1);
    }
    return 0;
}