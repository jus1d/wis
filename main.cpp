#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>
#include <map>
#include <cstring>

#include "./assert.h"

using namespace std;

enum class OpType : int {
    PUSH_INT,
    PUSH_STRING,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    BOR,
    BAND,
    XOR,
    SHL,
    SHR,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    IF,
    ELSE,
    END,
    DO,
    WHILE,
    PUT,
    PUTS,
    COPY,
    DROP,
    COUNT,
};

const map<OpType, string> HumanizedOpTypes = {
        {OpType::PUSH_INT, "PUSH INT"},
        {OpType::PUSH_STRING, "PUSH STRING"},
        {OpType::PLUS, "PLUS"},
        {OpType::MINUS, "MINUS"},
        {OpType::MUL, "MULTIPLY"},
        {OpType::DIV, "DIVISION"},
        {OpType::MOD, "MOD"},
        {OpType::BOR, "BINARY OR"},
        {OpType::BAND, "BINARY AND"},
        {OpType::XOR, "XOR"},
        {OpType::SHL, "LEFT SHIFT"},
        {OpType::SHR, "RIGHT SHIFT"},
        {OpType::EQ, "EQUAL"},
        {OpType::NE, "NOT EQUAL"},
        {OpType::LT, "LESS"},
        {OpType::GT, "GREATER"},
        {OpType::LE, "LESS OR EQUAL"},
        {OpType::GE, "GREATER OR EQUAL"},
        {OpType::IF, "IF"},
        {OpType::ELSE, "ELSE"},
        {OpType::END, "END"},
        {OpType::DO, "DO"},
        {OpType::DO, "WHILE"},
        {OpType::PUT, "PUT"},
        {OpType::PUTS, "PUTS"},
};

class Operation {
public:
    OpType Type;
    int IntegerValue{};
    string StringValue;
    string Loc;
    int Address{};
    int JumpTo{};

    Operation(OpType type, string loc)
            : Type(type), Loc(std::move(loc)) {}

    Operation(OpType type, int integer_value, string loc)
            : Type(type), IntegerValue(integer_value), Loc(std::move(loc)) {}

    Operation(OpType type, string string_value, string loc)
            : Type(type), StringValue(std::move(string_value)), Loc(std::move(loc)) {}
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
    int IntegerValue{};
    string Loc;

    Token(TokenType type, int integer_value, string  loc)
            : Type(type), IntegerValue(integer_value), Loc(std::move(loc)) {}

    Token(TokenType type, string  string_value, string  loc)
            : Type(type), StringValue(std::move(string_value)), Loc(std::move(loc)) {}
};

enum class DataType : int {
    INT,
    STRING,
    BOOL,
    COUNT,
};

const map<DataType, string> HumanizedDataTypes = {
        {DataType::INT, "int"},
        {DataType::STRING, "string"},
        {DataType::BOOL, "bool"}
};

class Type {
public:
    DataType Code;
    string Loc;

    Type(DataType code, string loc)
            : Code(code), Loc(std::move(loc)) {}
};

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

vector<string> split_string(const string& input, const string& delimiter) {
    vector<string> result;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != string::npos) {
        result.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }

    result.push_back(input.substr(start));

    return result;
}

string location_view(const string& filepath, int row, int col)
{
    return filepath + ":" + to_string(row) + ":" + to_string(col);
}

size_t find_col(string const& line, size_t start, bool (*predicate)(char))
{
    while (start < line.size() && !predicate(line[start])) {
        start++;
    }
    return start;
}

vector<Operation> parse_tokens_as_operations(const vector<Token>& tokens)
{
    vector<Operation> program;

    assert(static_cast<int>(TokenType::COUNT) == 3, "Exhaustive token types handling");

    for (const auto& token: tokens) {
        switch (token.Type) {
            case TokenType::INT:
                program.emplace_back(OpType::PUSH_INT, token.IntegerValue, token.Loc);
                break;
            case TokenType::STRING:
                program.emplace_back(OpType::PUSH_STRING, token.StringValue, token.Loc);
                break;
            case TokenType::WORD:
                assert(static_cast<int>(OpType::COUNT) == 27, "Exhaustive operations handling");

                if (token.StringValue == "+")
                {
                    program.emplace_back(OpType::PLUS, token.Loc);
                }
                else if (token.StringValue == "-")
                {
                    program.emplace_back(OpType::MINUS, token.Loc);
                }
                else if (token.StringValue == "*")
                {
                    program.emplace_back(OpType::MUL, token.Loc);
                }
                else if (token.StringValue == "/")
                {
                    program.emplace_back(OpType::DIV, token.Loc);
                }
                else if (token.StringValue == "%")
                {
                    program.emplace_back(OpType::MOD, token.Loc);
                }
                else if (token.StringValue == "bor")
                {
                    program.emplace_back(OpType::BOR, token.Loc);
                }
                else if (token.StringValue == "band")
                {
                    program.emplace_back(OpType::BAND, token.Loc);
                }
                else if (token.StringValue == "xor")
                {
                    program.emplace_back(OpType::XOR, token.Loc);
                }
                else if (token.StringValue == "shl")
                {
                    program.emplace_back(OpType::SHL, token.Loc);
                }
                else if (token.StringValue == "shr")
                {
                    program.emplace_back(OpType::SHR, token.Loc);
                }
                else if (token.StringValue == "==")
                {
                    program.emplace_back(OpType::EQ, token.Loc);
                }
                else if (token.StringValue == "!=")
                {
                    program.emplace_back(OpType::NE, token.Loc);
                }
                else if (token.StringValue == "<")
                {
                    program.emplace_back(OpType::LT, token.Loc);
                }
                else if (token.StringValue == ">")
                {
                    program.emplace_back(OpType::GT, token.Loc);
                }
                else if (token.StringValue == "<=")
                {
                    program.emplace_back(OpType::LE, token.Loc);
                }
                else if (token.StringValue == ">=")
                {
                    program.emplace_back(OpType::GE, token.Loc);
                }
                else if (token.StringValue == "if")
                {
                    program.emplace_back(OpType::IF, token.Loc);
                }
                else if (token.StringValue == "else")
                {
                    program.emplace_back(OpType::ELSE, token.Loc);
                }
                else if (token.StringValue == "end")
                {
                    program.emplace_back(OpType::END, token.Loc);
                }
                else if (token.StringValue == "do")
                {
                    program.emplace_back(OpType::DO, token.Loc);
                }
                else if (token.StringValue == "while")
                {
                    program.emplace_back(OpType::WHILE, token.Loc);
                }
                else if (token.StringValue == "put")
                {
                    program.emplace_back(OpType::PUT, token.Loc);
                }
                else if (token.StringValue == "puts")
                {
                    program.emplace_back(OpType::PUTS, token.Loc);
                }
                else if (token.StringValue == "copy")
                {
                    program.emplace_back(OpType::COPY, token.Loc);
                }
                else if (token.StringValue == "drop")
                {
                    program.emplace_back(OpType::DROP, token.Loc);
                }
                else
                {
                    cerr << token.Loc << ": ERROR: Unexpected token: `" << token.StringValue << "`" << endl;
                    exit(1);
                }
                break;
            default:
                assert(false, "Unreachable");
        }
    }

    return program;
}

void crossreference_blocks(vector<Operation>& program)
{
    stack<int> crossreference_stack;

    assert(static_cast<int>(OpType::COUNT) == 27, "Exhaustive operations handling. Not all operations should be handled in here");

    for (size_t i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        switch (op.Type) {
            case OpType::IF:
            case OpType::WHILE:
            {
                crossreference_stack.push(i);
                break;
            }
            case OpType::ELSE:
            {
                int pos = crossreference_stack.top();
                crossreference_stack.pop();
                program[pos].JumpTo = i + 1;
                crossreference_stack.push(i);
                break;
            }
            case OpType::DO:
            {
                program[i].JumpTo = crossreference_stack.top();
                crossreference_stack.pop();
                crossreference_stack.push(i);
                break;
            }
            case OpType::END:
            {
                int pos = crossreference_stack.top();
                crossreference_stack.pop();

                switch (program[pos].Type) {
                    case OpType::IF:
                    case OpType::ELSE:
                    {
                        program[pos].JumpTo = i;
                        break;
                    }
                    case OpType::DO:
                    {
                        program[i].JumpTo = program[pos].JumpTo;
                        program[pos].JumpTo = i + 1;
                        break;
                    }
                    default:
                    {
                        assert(false, "Only `if` and `while` blocks can be closed with `end`");
                    }
                }
            }
        }
    }

    if (!crossreference_stack.empty())
    {
        int latest_pos = crossreference_stack.top();
        crossreference_stack.pop();

        cerr << program[latest_pos].Loc << ": ERROR: Not all blocks was closed with `end`" << endl;
        exit(1);
    }
}

vector<Token> lex_line(string const& filepath, int line_number, string const& line)
{
    vector<Token> tokens;

    size_t col = find_col(line, 0, [](char x) { return !isspace(x); });
    while (col < line.size())
    {
        int col_end;
        if (line[col] == '"')
        {
            col_end = find_col(line, col+1, [](char x) { return x == '"'; });
            if (line[col_end] != '"')
            {
                cerr << location_view(filepath, line_number, col + 1) << ": ERROR: Missed closing string literal" << endl;
                exit(1);
            }

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
        line = split_string(line, "//")[0];
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
    assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

    stack<Type> type_checking_stack;

    for (size_t i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        assert(static_cast<int>(OpType::COUNT) == 27, "Exhaustive operations handling");

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
            case OpType::MINUS:
            case OpType::MUL:
            case OpType::DIV:
            case OpType::MOD:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else {
                    // TODO: Maybe print location of incorrect argument
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 `" << HumanizedDataTypes.at(DataType::INT) << "`, but found `" << HumanizedDataTypes.at(b.Code) << "` and `" << HumanizedDataTypes.at(a.Code) << "`" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::BOR:
            case OpType::BAND:
            case OpType::XOR:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else if (a.Code == DataType::BOOL && b.Code == DataType::BOOL) {
                    type_checking_stack.emplace(DataType::BOOL, op.Loc);
                } else {
                    // TODO: Maybe print location of incorrect argument
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 `" << HumanizedDataTypes.at(DataType::INT) << "` or 2 `" << HumanizedDataTypes.at(DataType::BOOL) << "`, but found `" << HumanizedDataTypes.at(b.Code) << "` and `" << HumanizedDataTypes.at(a.Code) << "`" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::SHL:
            case OpType::SHR:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else {
                    // TODO: Maybe print location of incorrect argument
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 `" << HumanizedDataTypes.at(DataType::INT) << "`, but found `" << HumanizedDataTypes.at(b.Code) << "` and `" << HumanizedDataTypes.at(a.Code) << "`" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::EQ:
            case OpType::NE:
            case OpType::LT:
            case OpType::GT:
            case OpType::LE:
            case OpType::GE:
            {
                if (type_checking_stack.size() < 2) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << to_string(type_checking_stack.size()) << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::BOOL, op.Loc);
                } else if (a.Code == DataType::BOOL && b.Code == DataType::BOOL) {
                    cerr << op.Loc << ": ERROR: Use `" << HumanizedOpTypes.at(OpType::BAND) << "`, `" << HumanizedOpTypes.at(OpType::BOR) << "` and `" << HumanizedOpTypes.at(OpType::XOR) << "` operations to compare booleans. Expected 2 `" << HumanizedDataTypes.at(DataType::INT) << "`, but found `" << HumanizedDataTypes.at(b.Code) << "` and `" << HumanizedDataTypes.at(a.Code) << "`" << endl;
                    exit(1);
                } else {
                    // TODO: Maybe print location of incorrect argument
                    cerr << op.Loc << ": ERROR: Only integer values can be compared. Expected 2 `" << HumanizedDataTypes.at(DataType::INT) << "`, but found `" << HumanizedDataTypes.at(b.Code) << "` and `" << HumanizedDataTypes.at(a.Code) << "`" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::IF:
            case OpType::DO:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for `" << HumanizedOpTypes.at(OpType::PUT) << "` operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                type_checking_stack.pop();

                if (top.Code != DataType::BOOL)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument's type for `" << HumanizedOpTypes.at(OpType::PUT) << "` operation. Expected type `" << HumanizedDataTypes.at(DataType::BOOL) << "`, but found `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::ELSE:
            case OpType::WHILE:
            case OpType::END:
            {
                // No type checking needed for these operations.
                break;
            }
            case OpType::PUT:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(OpType::PUT) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }
                Type top = type_checking_stack.top();
                if (top.Code != DataType::INT && top.Code != DataType::BOOL)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for " << HumanizedOpTypes.at(OpType::PUT) << " operation. Expected `" << HumanizedDataTypes.at(DataType::INT) << "` or `" << HumanizedDataTypes.at(DataType::BOOL) << "`, but found `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                type_checking_stack.pop();
                break;
            }
            case OpType::PUTS:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(OpType::PUTS) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                if (top.Code != DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for " << HumanizedOpTypes.at(OpType::PUTS) << " operation. Expected `" << HumanizedDataTypes.at(DataType::STRING) << "`, but found `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                type_checking_stack.pop();

                break;
            }
            case OpType::COPY:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(OpType::COPY) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                if (top.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument's type for " << HumanizedOpTypes.at(OpType::COPY) << " operation. Expected `" << HumanizedDataTypes.at(DataType::INT) << "` or `" << HumanizedDataTypes.at(DataType::BOOL) << "`, but found `" << HumanizedDataTypes.at(top.Code) << "`" << endl;
                    exit(1);
                }
                type_checking_stack.push(top);

                break;
            }
            case OpType::DROP:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(OpType::COPY) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                type_checking_stack.pop();

                break;
            }
            default:
                assert(false, "Unreachable");
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
    assert(static_cast<int>(OpType::COUNT) == 27, "Exhaustive operations handling");

    stack<int> runtime_stack;
    vector<byte> memory;
    size_t strings_size = 0;

    for (size_t i = 0; i < program.size();) {
        Operation op = program[i];

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                runtime_stack.push(op.IntegerValue);
                ++i;
                break;
            }
            case OpType::PUSH_STRING:
            {
                const byte* bs = reinterpret_cast<const byte*>(op.StringValue.c_str());
                int n = op.StringValue.length();

                runtime_stack.push(n);

                op.Address = strings_size;
                runtime_stack.push(op.Address);

                for (int j = 0; j < n; ++j) {
                    memory.push_back(bs[j]);
                }

                strings_size += n;
                ++i;
                break;
            }
            case OpType::PLUS:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a + b);
                ++i;
                break;
            }
            case OpType::MINUS:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b - a);
                ++i;
                break;
            }
            case OpType::MUL:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a * b);
                ++i;
                break;
            }
            case OpType::DIV:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b / a);
                ++i;
                break;
            }
            case OpType::MOD:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b % a);
                ++i;
                break;
            }
            case OpType::BOR:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a | b);
                ++i;
                break;
            }
            case OpType::BAND:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a & b);
                ++i;
                break;
            }
            case OpType::XOR:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a ^ b);
                ++i;
                break;
            }
            case OpType::SHL:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b << a);
                ++i;
                break;
            }
            case OpType::SHR:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b >> a);
                ++i;
                break;
            }
            case OpType::EQ:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a == b);
                ++i;
                break;
            }
            case OpType::NE:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a != b);
                ++i;
                break;
            }
            case OpType::LT:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b < a);
                ++i;
                break;
            }
            case OpType::GT:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b > a);
                ++i;
                break;
            }
            case OpType::LE:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b <= a);
                ++i;
                break;
            }
            case OpType::GE:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b >= a);
                ++i;
                break;
            }
            case OpType::IF:
            case OpType::DO:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                if (a == 0)
                {
                    i = op.JumpTo;
                }
                else
                {
                    ++i;
                }
                break;
            }
            case OpType::ELSE:
            {
                i = op.JumpTo;
                break;
            }
            case OpType::END:
            {
                if (program[op.JumpTo].Type == OpType::WHILE)
                {
                    i = op.JumpTo;
                }
                else
                {
                    ++i;
                }
                break;
            }
            case OpType::WHILE:
            {
                ++i;
                break;
            }
            case OpType::PUT:
            {
                int val = runtime_stack.top();
                runtime_stack.pop();
                cout << to_string(val) << endl;
                ++i;
                break;
            }
            case OpType::PUTS:
            {
                int buf = runtime_stack.top();
                runtime_stack.pop();
                int n = runtime_stack.top();
                runtime_stack.pop();

                string s;
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
                ++i;
                break;
            }
            case OpType::COPY:
            {
                runtime_stack.push(runtime_stack.top());
                ++i;
                break;
            }
            case OpType::DROP:
            {
                runtime_stack.pop();
                ++i;
                break;
            }
            default:
                assert(false, "Unreachable");
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