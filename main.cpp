#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>
#include <map>

using namespace std;

enum class OpType : int {
    PUSH_INT,
    PUSH_STRING,
    PLUS,
    MINUS,
    PUT,
    PUTS,
    COUNT,
};

class Operation {
public:
    OpType Type;
    int IntegerValue;
    string StringValue;
    string Loc;
    int Address;

    Operation(OpType type, string loc)
            : Type(type), IntegerValue(0), Loc(std::move(loc)) {}

    Operation(OpType type, int integer_value, string loc)
            : Type(type), IntegerValue(integer_value), Loc(std::move(loc)) {}

    Operation(OpType type, string string_value, string loc)
            : Type(type), IntegerValue(0), StringValue(std::move(string_value)), Loc(std::move(loc)) {}
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

enum class DataType : int {
    INT,
    STRING,
    BOOL,
    COUNT,
};

class Type {
public:
    DataType Code;
    string Loc;

    Type(DataType code, string loc)
        : Code(code), Loc(loc) {}
};

const map<DataType, string> HumanizedDataTypes = {
    {DataType::INT, "int"},
    {DataType::STRING, "string"},
    {DataType::BOOL, "bool"}
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
    }
    cerr << "ERROR: Can't shift empty vector" << endl;
    exit(1);
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
                assert(int(OpType::COUNT) == 6, "Exhaustive operations handling in parse_tokens_as_operations()");

                if (token.StringValue == "+")
                {
                    program.emplace_back(OpType::PLUS, token.Loc);
                }
                else if (token.StringValue == "-")
                {
                    program.emplace_back(OpType::MINUS, token.Loc);
                }
                else if (token.StringValue == "put")
                {
                    program.emplace_back(OpType::PUT, token.Loc);
                }
                else if (token.StringValue == "puts")
                {
                    program.emplace_back(OpType::PUTS, token.Loc);
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
    assert(int(OpType::COUNT) == 6, "Exhaustive operations handling in crossreference_blocks(). "
                                    "Not all operations should be handled in here");
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

void type_check_program(vector<Operation> program)
{
    assert(int(DataType::COUNT) == 3, "Exhaustive data types handling in type_check_program()");

    stack<Type> type_checking_stack;

    for (int i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        assert(int(OpType::COUNT) == 6, "Exhaustive operations handling in type_check_program()");

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::PUSH_STRING:
            {
                type_checking_stack.emplace(DataType::STRING, op.Loc);
                break;
            }
            case OpType::PLUS:
            {
                if (type_checking_stack.size() < 2)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for PLUS operation. "
                                      "Expected 2 arguments, got " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT)
                {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                }
                else
                {
                    // TODO: Add expected and actual types
                    cerr << a.Loc << ": ERROR: Invalid arguments types for PLUS operation" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::MINUS:
            {
                if (type_checking_stack.size() < 2) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for MINUS operation. "
                                      "Expected 2 arguments, got " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else {
                    // TODO: Add expected and actual types
                    cerr << a.Loc << ": ERROR: Invalid arguments types for MINUS operation" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::PUT:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for PUT operation. "
                                      "Expected 1 argument, got 0" << endl;
                    exit(1);
                }
                Type top = type_checking_stack.top();
                if (top.Code != DataType::INT)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for PUT operation. "
                                      "Expected `int`, but got `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                type_checking_stack.pop();
                break;
            }
            case OpType::PUTS:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for PUTS operation. "
                                      "Expected 1 argument, got 0" << endl;
                    exit(1);
                }
                Type top = type_checking_stack.top();
                if (top.Code != DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for PUTS operation. "
                                      "Expected `string`, but got `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                type_checking_stack.pop();
                break;
            }
            default:
                assert(false, "unreachable");
        }
    }

    if (type_checking_stack.size() != 0)
    {
        Type top = type_checking_stack.top();
        cerr << top.Loc << ": ERROR: Unhandled data in the stack" << endl;
        exit(1);
    }
}

void run_program(vector<Operation> program)
{
    assert(int(OpType::COUNT) == 6, "Exhaustive operations handling in run_program()");

    stack<int> runtime_stack;
    vector<byte> memory;
    size_t strings_size = 0;

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
                const std::byte* bs = reinterpret_cast<const std::byte*>(op.StringValue.c_str());
                int n = op.StringValue.length();

                runtime_stack.push(n);

                op.Address = strings_size;
                runtime_stack.push(op.Address);

                for (int j = 0; j < n; ++j) {
                    memory.push_back(bs[j]);
                }

                strings_size += n;
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
            case OpType::MINUS:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b - a);
                break;
            }
            case OpType::PUT:
            {
                int val = runtime_stack.top();
                runtime_stack.pop();
                cout << to_string(val) << endl;
                break;
            }
            case OpType::PUTS:
            {
                int buf = runtime_stack.top();
                runtime_stack.pop();
                int n = runtime_stack.top();
                runtime_stack.pop();

                std::string s;
                if (buf >= 0 && n >= 0 && buf + n <= static_cast<int>(memory.size())) {
                    s.reserve(n);

                    for (int j = buf; j < buf + n; ++j) {
                        s.push_back(static_cast<char>(memory[j]));
                    }
                } else {
                    cerr << op.Loc << ": ERROR: Invalid length buffer or length values" << endl;
                    exit(1);
                }

                cout << s << endl;

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

        type_check_program(program);
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