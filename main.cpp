#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>
#include <map>
#include <string>
#include <codecvt>

#include "./assert.h"

using namespace std;

const string FILE_EXTENSION = ".glo";

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
    NOT,
    TRUE,
    FALSE,
    IF,
    ELSE,
    END,
    DO,
    WHILE,
    PUT,
    PUTS,
    HERE,
    COPY,
    OVER,
    SWAP,
    DROP,
    COUNT,
};

const map<OpType, string> HumanizedOpTypes = {
        {OpType::PUSH_INT, "PUSH INT"},
        {OpType::PUSH_STRING, "PUSH STRING"},
        {OpType::PLUS, "+"},
        {OpType::MINUS, "-"},
        {OpType::MUL, "*"},
        {OpType::DIV, "/"},
        {OpType::MOD, "%"},
        {OpType::BOR, "binary or"},
        {OpType::BAND, "binary and"},
        {OpType::XOR, "xor"},
        {OpType::SHL, "left shift"},
        {OpType::SHR, "right shift"},
        {OpType::EQ, "=="},
        {OpType::NE, "!="},
        {OpType::LT, "<"},
        {OpType::GT, ">"},
        {OpType::LE, "<="},
        {OpType::GE, ">="},
        {OpType::NOT, "not"},
        {OpType::TRUE, "true"},
        {OpType::FALSE, "false"},
        {OpType::IF, "if"},
        {OpType::ELSE, "else"},
        {OpType::END, "end"},
        {OpType::DO, "do"},
        {OpType::WHILE, "while"},
        {OpType::PUT, "put"},
        {OpType::PUTS, "puts"},
        {OpType::HERE, "here"},
        {OpType::COPY, "copy"},
        {OpType::OVER, "over"},
        {OpType::SWAP, "swap"},
        {OpType::DROP, "drop"},
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

string trim_string(string s, const string& substring) {
    size_t pos = s.find(substring);
    if (pos != string::npos) {
        s.erase(pos, substring.length());
    }
    return s;
}

string unescape_string(const string& s) {
    stringstream ss;
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '\\' && i + 1 < s.length()) {
            switch (s[i + 1]) {
                case 'a': ss << '\a'; break;
                case 'b': ss << '\b'; break;
                case 'f': ss << '\f'; break;
                case 'n': ss << '\n'; break;
                case 'r': ss << '\r'; break;
                case 't': ss << '\t'; break;
                case 'v': ss << '\v'; break;
                case '\\': ss << '\\'; break;
                case '\'': ss << '\''; break;
                case '\"': ss << '\"'; break;
                case '\?': ss << '\?'; break;
                case '0': ss << '\0'; break;
                default:
                    if (isdigit(s[i + 1])) {
                        int value = 0;
                        ss << oct << s[i + 1];
                        ss >> value;
                        ss.clear();
                        ss << static_cast<char>(value);
                    } else if (s[i + 1] == 'x' || s[i + 1] == 'X') {
                        int value = 0;
                        ss << hex << s.substr(i + 2, 2);
                        ss >> value;
                        ss.clear();
                        ss << static_cast<char>(value);
                        i += 2;
                    } else {
                        ss << s[i] << s[i + 1];
                    }
                    break;
            }
            ++i;
        } else {
            ss << s[i];
        }
    }
    return ss.str();
}

void complete_string(string& s, const string& additional)
{
    s += additional + "\n";
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

void execute_command_echoed(const string& command)
{
    cout << "[CMD] " << command << endl;
    int exit_code = system(command.c_str());
    if (exit_code != 0)
    {
        cerr << "ERROR: Executing command crashed with " << to_string(exit_code) << " exit code" << endl;
        exit(exit_code);
    }
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
            {
                program.emplace_back(OpType::PUSH_STRING, unescape_string(token.StringValue), token.Loc);
                break;
            }
            case TokenType::WORD:
                assert(static_cast<int>(OpType::COUNT) == 33, "Exhaustive operations handling");

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
                else if (token.StringValue == "not")
                {
                    program.emplace_back(OpType::NOT, token.Loc);
                }
                else if (token.StringValue == "true")
                {
                    program.emplace_back(OpType::TRUE, token.Loc);
                }
                else if (token.StringValue == "false")
                {
                    program.emplace_back(OpType::FALSE, token.Loc);
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
                else if (token.StringValue == "here")
                {
                    program.emplace_back(OpType::HERE, token.Loc);
                }
                else if (token.StringValue == "copy")
                {
                    program.emplace_back(OpType::COPY, token.Loc);
                }
                else if (token.StringValue == "over")
                {
                    program.emplace_back(OpType::OVER, token.Loc);
                }
                else if (token.StringValue == "swap")
                {
                    program.emplace_back(OpType::SWAP, token.Loc);
                }
                else if (token.StringValue == "drop")
                {
                    program.emplace_back(OpType::DROP, token.Loc);
                }
                else
                {
                    cerr << token.Loc << ": ERROR: Unexpected token: '" << token.StringValue << "'" << endl;
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

    assert(static_cast<int>(OpType::COUNT) == 33, "Exhaustive operations handling. Not all operations should be handled in here");

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

        assert(static_cast<int>(OpType::COUNT) == 33, "Exhaustive operations handling");

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::PUSH_STRING:
            case OpType::HERE:
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
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
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
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << " or 2 " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
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
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
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
                    cerr << op.Loc << ": ERROR: Use " << HumanizedOpTypes.at(OpType::BAND) << ", " << HumanizedOpTypes.at(OpType::BOR) << " and " << HumanizedOpTypes.at(OpType::XOR) << " operations to compare booleans. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
                    exit(1);
                } else {
                    cerr << op.Loc << ": ERROR: Only integer values can be compared. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::NOT:
            {
                if (type_checking_stack.empty()) {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 arguments, but found 0" << endl;
                    exit(1);
                }
                Type a = type_checking_stack.top();
                if (a.Code != DataType::BOOL)
                {
                    cerr << a.Loc << ": ERROR: Expected type " << HumanizedDataTypes.at(DataType::BOOL) << " for " << HumanizedOpTypes.at(OpType::NOT) << ", but found " << HumanizedDataTypes.at(a.Code) << endl;
                    exit(1);
                }
                break;
            }
            case OpType::TRUE:
            case OpType::FALSE:
            {
                type_checking_stack.emplace(DataType::BOOL, op.Loc);
                break;
            }
            case OpType::IF:
            case OpType::DO:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                type_checking_stack.pop();

                if (top.Code != DataType::BOOL)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected type " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(top.Code) << "" << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }
                Type top = type_checking_stack.top();
                if (top.Code != DataType::INT && top.Code != DataType::BOOL)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(top.Code) << "" << endl;
                    exit(1);
                }
                type_checking_stack.pop();
                break;
            }
            case OpType::PUTS:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                if (top.Code != DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::STRING) << ", but found " << HumanizedDataTypes.at(top.Code) << "" << endl;
                    exit(1);
                }
                type_checking_stack.pop();

                break;
            }
            case OpType::COPY:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                Type top = type_checking_stack.top();
                if (top.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(top.Code) << ". For copying strings, use 2copy instead" << endl;
                    exit(1);
                }
                type_checking_stack.push(top);

                break;
            }
            case OpType::OVER:
            {
                if (type_checking_stack.size() < 2)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 argument, but found " << type_checking_stack.size() << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();

                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Incorrect argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(a.Code) << "" << endl;
                    exit(1);
                }
                else if (b.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Incorrect argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(b.Code) << "" << endl;
                    exit(1);
                }

                type_checking_stack.push(b);
                type_checking_stack.push(a);
                type_checking_stack.push(b);

                break;
            }
            case OpType::SWAP:
            {
                if (type_checking_stack.size() < 2)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 argument, but found " << type_checking_stack.size() << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();

                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Incorrect argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(a.Code) << "" << endl;
                    exit(1);
                }
                else if (b.Code == DataType::STRING)
                {
                    cerr << op.Loc << ": ERROR: Incorrect argument's type for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::INT) << " or " << HumanizedDataTypes.at(DataType::BOOL) << ", but found " << HumanizedDataTypes.at(b.Code) << "" << endl;
                    exit(1);
                }

                type_checking_stack.push(a);
                type_checking_stack.push(b);

                break;
            }
            case OpType::DROP:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
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
    assert(static_cast<int>(OpType::COUNT) == 33, "Exhaustive operations handling");

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
            case OpType::NOT:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                if (a == 0) runtime_stack.push(1);
                else if (a == 1) runtime_stack.push(0);
                else assert(false, "unreachable");
                ++i;
                break;
            }
            case OpType::TRUE:
            {
                runtime_stack.push(1);
                ++i;
                break;
            }
            case OpType::FALSE:
            {
                runtime_stack.push(0);
                ++i;
                break;
            }
            case OpType::IF:
            case OpType::DO:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();

                if (a == 0) i = op.JumpTo;
                else ++i;
                break;
            }
            case OpType::ELSE:
            {
                i = op.JumpTo;
                break;
            }
            case OpType::END:
            {
                if (program[op.JumpTo].Type == OpType::WHILE) i = op.JumpTo;
                else ++i;
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

                cout << s;
                ++i;
                break;
            }
            case OpType::HERE:
            {
                const byte* bs = reinterpret_cast<const byte*>(op.Loc.c_str());
                int n = op.Loc.length();

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
            case OpType::COPY:
            {
                runtime_stack.push(runtime_stack.top());
                ++i;
                break;
            }
            case OpType::OVER:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();
                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(b);
                runtime_stack.push(a);
                runtime_stack.push(b);

                ++i;
                break;
            }
            case OpType::SWAP:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();
                int b = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a);
                runtime_stack.push(b);

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

void generate_nasm_linux_x86_64(const string& output_file_path, vector<Operation> program)
{
    std::ofstream out(output_file_path);

    if (!out.is_open()) {
        cerr << "ERROR: Can't open file: " << output_file_path << endl;
        exit(1);
    }

    map<string, int> strings;

    string output_content;

    bool is_put_needed;
    for (const auto& op : program)
    {
        if (op.Type == OpType::PUT)
        {
            is_put_needed = true;
            break;
        }
    }
    if (is_put_needed)
    {
        complete_string(output_content, "put:");
        complete_string(output_content, "    mov     r9, -3689348814741910323");
        complete_string(output_content, "    sub     rsp, 40");
        complete_string(output_content, "    mov     BYTE [rsp+31], 10");
        complete_string(output_content, "    lea     rcx, [rsp+30]");
        complete_string(output_content, ".L2:");
        complete_string(output_content, "    mov     rax, rdi");
        complete_string(output_content, "    lea     r8, [rsp+32]");
        complete_string(output_content, "    mul     r9");
        complete_string(output_content, "    mov     rax, rdi");
        complete_string(output_content, "    sub     r8, rcx");
        complete_string(output_content, "    shr     rdx, 3");
        complete_string(output_content, "    lea     rsi, [rdx+rdx*4]");
        complete_string(output_content, "    add     rsi, rsi");
        complete_string(output_content, "    sub     rax, rsi");
        complete_string(output_content, "    add     eax, 48");
        complete_string(output_content, "    mov     BYTE [rcx], al");
        complete_string(output_content, "    mov     rax, rdi");
        complete_string(output_content, "    mov     rdi, rdx");
        complete_string(output_content, "    mov     rdx, rcx");
        complete_string(output_content, "    sub     rcx, 1");
        complete_string(output_content, "    cmp     rax, 9");
        complete_string(output_content, "    ja      .L2");
        complete_string(output_content, "    lea     rax, [rsp+32]");
        complete_string(output_content, "    mov     edi, 1");
        complete_string(output_content, "    sub     rdx, rax");
        complete_string(output_content, "    xor     eax, eax");
        complete_string(output_content, "    lea     rsi, [rsp+32+rdx]");
        complete_string(output_content, "    mov     rdx, r8");
        complete_string(output_content, "    mov     rax, 1");
        complete_string(output_content, "    syscall");
        complete_string(output_content, "    add     rsp, 40");
        complete_string(output_content, "    ret\n");
    }

    complete_string(output_content, "section .text");
    complete_string(output_content, "    global _start\n");
    complete_string(output_content, "_start:");

    assert(static_cast<int>(OpType::COUNT) == 33, "Exhaustive operations handling");

    for (size_t i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                complete_string(output_content, "    ; -- push int -- ");
                complete_string(output_content, "    push    " + to_string(op.IntegerValue));
                break;
            }
            case OpType::PUSH_STRING:
            {
                complete_string(output_content, "    ; -- push str --");
                complete_string(output_content, "    push    " + to_string(op.StringValue.size()));
                auto it = strings.find(op.StringValue);
                if (it == strings.end())
                {
                    strings[op.StringValue] = strings.size();
                }
                complete_string(output_content, "    push    str_" + to_string(strings.at(op.StringValue)));
                break;
            }
            case OpType::PLUS:
            {
                complete_string(output_content, "    ; -- plus --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    add     rax, rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::MINUS:
            {
                complete_string(output_content, "    ; -- minus --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    sub     rbx, rax");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::MUL:
            {
                complete_string(output_content, "    ; -- multiply --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    imul    rax, rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::DIV:
            {
                complete_string(output_content, "    ; -- division --");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    xor     rdx, rdx");
                complete_string(output_content, "    div     rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::MOD:
            {
                complete_string(output_content, "    ; -- mod --");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    xor     rdx, rdx");
                complete_string(output_content, "    div     rbx");
                complete_string(output_content, "    push    rdx");
                break;
            }
            case OpType::BOR:
            {
                complete_string(output_content, "    ; -- binary or --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    or      rax, rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::BAND:
            {
                complete_string(output_content, "    ; -- binary and --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    and     rax, rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::XOR:
            {
                complete_string(output_content, "    ; -- xor --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    xor     rax, rbx");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::SHL:
            {
                complete_string(output_content, "    ; -- shift left --");
                complete_string(output_content, "    pop     rcx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    shl     rax, cl");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::SHR:
            {
                complete_string(output_content, "    ; -- shift right --");
                complete_string(output_content, "    pop     rcx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    shr     rax, cl");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::EQ:
            {
                complete_string(output_content, "    ; -- equal --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rax, rbx");
                complete_string(output_content, "    cmove   rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::NE:
            {
                complete_string(output_content, "    ; -- not equal --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rax, rbx");
                complete_string(output_content, "    cmovne  rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::LT:
            {
                complete_string(output_content, "    ; -- less --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rbx, rax");
                complete_string(output_content, "    cmovl   rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::GT:
            {
                complete_string(output_content, "    ; -- greater --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rbx, rax");
                complete_string(output_content, "    cmovg   rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::LE:
            {
                complete_string(output_content, "    ; -- less or equal --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rbx, rax");
                complete_string(output_content, "    cmovle  rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::GE:
            {
                complete_string(output_content, "    ; -- greater or equal --");
                complete_string(output_content, "    mov     rcx, 0");
                complete_string(output_content, "    mov     rdx, 1");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    cmp     rbx, rax");
                complete_string(output_content, "    cmovge  rcx, rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::NOT:
            {
                complete_string(output_content, "    ; -- not --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    xor     rax, 1");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::TRUE:
            {
                complete_string(output_content, "    ; -- true --");
                complete_string(output_content, "    push    1");
                break;
            }
            case OpType::FALSE:
            {
                complete_string(output_content, "    ; -- false --");
                complete_string(output_content, "    push    0");
                break;
            }
            case OpType::IF:
            {
                complete_string(output_content, "    ; -- if --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    mov     rbx, 0");
                complete_string(output_content, "    cmp     rax, rbx");
                complete_string(output_content, "    je      addr_" + to_string(op.JumpTo));
                break;
            }
            case OpType::ELSE:
            {
                complete_string(output_content, "    ; -- else --");
                complete_string(output_content, "    jmp     addr_" + to_string(op.JumpTo));
                complete_string(output_content, "    addr_" + to_string(i+1) + ":");
                break;
            }
            case OpType::END:
            {
                complete_string(output_content, "    ; -- end --");
                if (program[op.JumpTo].Type == OpType::WHILE)
                {
                    complete_string(output_content, "    jmp    addr_" + to_string(op.JumpTo));
                }
                complete_string(output_content, "addr_" + to_string(i) + ":");
                break;
            }
            case OpType::DO:
            {
                complete_string(output_content, "    ; -- do --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    mov     rbx, 0");
                complete_string(output_content, "    cmp     rax, rbx");
                complete_string(output_content, "    je      addr_" + to_string(op.JumpTo-1));
                break;
            }
            case OpType::WHILE:
            {
                complete_string(output_content, "    ; -- while --");
                complete_string(output_content, "    addr_" + to_string(i) + ":");
                break;
            }
            case OpType::PUT:
            {
                complete_string(output_content, "    ; -- put --");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    call    put");
                break;
            }
            // TODO: Make it like a binding in standard library
            case OpType::PUTS:
            {
                complete_string(output_content, "    ; -- puts --");
                complete_string(output_content, "    mov     rax, 1");
                complete_string(output_content, "    mov     rdi, 1");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::HERE:
            {
                string value = op.Loc;
                complete_string(output_content, "    ; -- here --");
                complete_string(output_content, "    push    " + to_string(value.size()));
                auto it = strings.find(value);
                if (it == strings.end())
                {
                    strings[value] = strings.size();
                }
                complete_string(output_content, "    push    str_" + to_string(strings.at(value)));
                break;
            }
            case OpType::COPY:
            {
                complete_string(output_content, "    ; -- copy --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    push    rax");
                complete_string(output_content, "    push    rax");
                break;
            }
            case OpType::OVER:
            {
                complete_string(output_content, "    ; -- over --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    push    rbx");
                complete_string(output_content, "    push    rax");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::SWAP:
            {
                complete_string(output_content, "    ; -- swap --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    push    rax");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::DROP:
            {
                complete_string(output_content, "    ; -- drop --");
                complete_string(output_content, "    pop     rax");
                break;
            }
            default:
                assert(false, "Unreachable");
        }
    }

    complete_string(output_content, "    ; -- exit --");
    complete_string(output_content, "    mov     rax, 60");
    complete_string(output_content, "    mov     rdi, 0");
    complete_string(output_content, "    syscall");

    if (!strings.empty()) complete_string(output_content, "\nsection .data");

    out << output_content;

    for (const auto& pair : strings) {
        out << "    str_" << pair.second << ": db ";

        const string& s = pair.first;
        for (size_t i = 0; i < s.size(); ++i) {
            out << "0x" << setw(2) << setfill('0') << hex << static_cast<int>(s[i]);

            if (i < s.size() - 1) {
                out << ",";
            }
        }

        out << endl;
    }
}

void run_mode(string compiler_path, vector<string> args)
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
        cerr << "ERROR: File '" << file_path.string() << "' doesn't exists" << endl;
        exit(1);
    }

    vector<Operation> program = lex_file(file_path.string());

    type_check_program(program);
    run_program(program);
}

void compile_mode(string compiler_path, vector<string> args)
{
    if (args.empty())
    {
        usage(compiler_path);
        cerr << "ERROR: No path to file provided" << endl;
        exit(1);
    }

    string path = shift_vector(args);

    bool run_after_compilation = path == "-r";

    if (run_after_compilation)
    {
        if (args.empty())
        {
            usage(compiler_path);
            cerr << "ERROR: No path to file provided" << endl;
            exit(1);
        }

        path = shift_vector(args);
    }

    string filename = trim_string(path, FILE_EXTENSION);

    filesystem::path file_path(path);

    if (!filesystem::exists(file_path))
    {
        usage(compiler_path);
        cerr << "ERROR: File '" << file_path.string() << "' doesn't exists" << endl;
        exit(1);
    }

    vector<Operation> program = lex_file(file_path.string());

    type_check_program(program);

#ifdef __x86_64__
    cout << "[INFO] Generating assembly -> " << filename << ".asm" << endl;
    generate_nasm_linux_x86_64(filename + ".asm", program);

    cout << "[INFO] Compiling assembly with NASM" << endl;
    execute_command_echoed("nasm -felf64 -o " + filename + ".o " + filename + ".asm");
    cout << "[INFO] Object file generated: " << filename << ".asm -> " << filename << ".o" << endl;

    execute_command_echoed("ld -o " + filename + " " + filename + ".o");
    cout << "[INFO] Compiled to " << filename << endl;

    if (run_after_compilation) execute_command_echoed(filename);
    
    return;
#endif

#ifdef __aarch64__
    cerr << "ERROR: Support for `arm64` architecture, will delivered in nearest future" << endl;
    return;
#endif

    cerr << "ERROR: Your processor's architecture is not supported yet" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    assert(HumanizedOpTypes.size() == static_cast<int>(OpType::COUNT), "Exhaustive checking of humanized operations");
    assert(HumanizedDataTypes.size() == static_cast<int>(DataType::COUNT), "Exhaustive checking of humanized data types");

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
        run_mode(compiler_path, args);
    }
    else if (subcommand == "compile")
    {
        compile_mode(compiler_path, args);
    }
    else
    {
        usage(compiler_path);
        cerr << "ERROR: Provided unknown subcommand: " << subcommand << endl;
        exit(1);
    }
    return 0;
}