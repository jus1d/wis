#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stack>
#include <map>
#include <string>

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
    BIND,
    MEM,
    LOAD8,
    STORE8,
    LOAD64,
    STORE64,
    USE,
    PUT,
    FPUTS,
    HERE,
    COPY,
    OVER,
    SWAP,
    SWAP2,
    DROP,
    ROT,
    SYSCALL0,
    SYSCALL1,
    SYSCALL2,
    SYSCALL3,
    SYSCALL4,
    SYSCALL5,
    SYSCALL6,
    COUNT,
};

const map<OpType, string> HumanizedOpTypes = {
        {OpType::PUSH_INT, "`PUSH INT`"},
        {OpType::PUSH_STRING, "`PUSH STRING`"},
        {OpType::PLUS, "`+`"},
        {OpType::MINUS, "`-`"},
        {OpType::MUL, "`*`"},
        {OpType::DIV, "`/`"},
        {OpType::MOD, "`%`"},
        {OpType::BOR, "`binary or`"},
        {OpType::BAND, "`binary and`"},
        {OpType::XOR, "`xor`"},
        {OpType::SHL, "`left shift`"},
        {OpType::SHR, "`right shift`"},
        {OpType::EQ, "`==`"},
        {OpType::NE, "`!=`"},
        {OpType::LT, "`<`"},
        {OpType::GT, "`>`"},
        {OpType::LE, "`<=`"},
        {OpType::GE, "`>=`"},
        {OpType::NOT, "`not`"},
        {OpType::TRUE, "`true`"},
        {OpType::FALSE, "`false`"},
        {OpType::IF, "`if`"},
        {OpType::ELSE, "`else`"},
        {OpType::END, "`end`"},
        {OpType::DO, "`do`"},
        {OpType::WHILE, "`while`"},
        {OpType::BIND, "`bind`"},
        {OpType::MEM, "`mem`"},
        {OpType::LOAD8, "`.`"},
        {OpType::STORE8, "`,`"},
        {OpType::LOAD64, "`load64`"},
        {OpType::STORE64, "`store64`"},
        {OpType::USE, "`use`"},
        {OpType::PUT, "`put`"},
        {OpType::FPUTS, "`fputs`"},
        {OpType::HERE, "`here`"},
        {OpType::COPY, "`copy`"},
        {OpType::OVER, "`over`"},
        {OpType::SWAP, "`swap`"},
        {OpType::SWAP2, "`2swap`"},
        {OpType::DROP, "`drop`"},
        {OpType::ROT, "`rot`"},
        {OpType::SYSCALL0, "`syscall0`"},
        {OpType::SYSCALL1, "`syscall1`"},
        {OpType::SYSCALL2, "`syscall2`"},
        {OpType::SYSCALL3, "`syscall3`"},
        {OpType::SYSCALL4, "`syscall4`"},
        {OpType::SYSCALL5, "`syscall5`"},
        {OpType::SYSCALL6, "`syscall6`"},
};

const map<string, OpType> BuiltInOps = {
        {"+", OpType::PLUS},
        {"-", OpType::MINUS},
        {"*", OpType::MUL},
        {"/", OpType::DIV},
        {"%", OpType::MOD},
        {"bor", OpType::BOR},
        {"band", OpType::BAND},
        {"xor", OpType::XOR},
        {"shl", OpType::SHL},
        {"shr", OpType::SHR},
        {"==", OpType::EQ},
        {"!=", OpType::NE},
        {"<", OpType::LT},
        {">", OpType::GT},
        {"<=", OpType::LE},
        {">=", OpType::GE},
        {"not", OpType::NOT},
        {"true", OpType::TRUE},
        {"false", OpType::FALSE},
        {"if", OpType::IF},
        {"else", OpType::ELSE},
        {"end", OpType::END},
        {"do", OpType::DO},
        {"while", OpType::WHILE},
        {"bind", OpType::BIND},
        {"mem", OpType::MEM},
        {".", OpType::LOAD8},
        {",", OpType::STORE8},
        {"load64", OpType::LOAD64},
        {"store64", OpType::STORE64},
        {"use", OpType::USE},
        {"put", OpType::PUT},
        {"fputs", OpType::FPUTS},
        {"here", OpType::HERE},
        {"copy", OpType::COPY},
        {"over", OpType::OVER},
        {"swap", OpType::SWAP},
        {"2swap", OpType::SWAP2},
        {"drop", OpType::DROP},
        {"rot", OpType::ROT},
        {"syscall0", OpType::SYSCALL0},
        {"syscall1", OpType::SYSCALL1},
        {"syscall2", OpType::SYSCALL2},
        {"syscall3", OpType::SYSCALL3},
        {"syscall4", OpType::SYSCALL4},
        {"syscall5", OpType::SYSCALL5},
        {"syscall6", OpType::SYSCALL6},
};

class Operation {
public:
    OpType Type;
    int IntegerValue{};
    string StringValue;
    string Loc;
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

const map<TokenType, string> HumanizedTokenTypes = {
        {TokenType::WORD, "`word`"},
        {TokenType::INT, "`int`"},
        {TokenType::STRING, "`string`"}
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
    PTR,
    BOOL,
    COUNT,
};

const map<DataType, string> HumanizedDataTypes = {
        {DataType::INT, "`int`"},
        {DataType::PTR, "`ptr`"},
        {DataType::BOOL, "`bool`"}
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
    cerr << "Usage: " << compiler_path << " [ARGS] ./examples/goo.glo" << endl;
    cerr << "ARGS:" << endl;
    cerr << "    -r            Run compiled program after compilation" << endl;
    cerr << "    -s            Silent mode for compiler" << endl;
    cerr << "    -I <path>     Add directory to include paths list" << endl;
}

void compilation_error(const string& message)
{
    cerr << "ERROR: " << message << endl;
}

void compilation_error(const string& loc, const string& message)
{
    cerr << loc << ": ERROR: " << message << endl;
}

string shift_vector(vector<string>& vec)
{
    if (!vec.empty()) {
        string result = vec[0];

        vec.erase(vec.begin());

        return result;
    }
    compilation_error("Can't shift empty vector");
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

bool string_to_int(const std::string& str, int& result) {
    std::istringstream iss(str);

    if (!(iss >> result)) return false;
    if (iss.rdbuf()->in_avail() != 0) return false;

    return true;
}

string location_view(const string& filepath, int row, int col)
{
    return filepath + ":" + to_string(row) + ":" + to_string(col);
}

void execute_command(bool silent_mode, const string& command)
{
    if (!silent_mode) cout << "[CMD] " << command << endl;
    system(command.c_str());
}

void extend_with_include_directories(string& file_path, const vector<string>& include_paths)
{
    for (const auto& include_path : include_paths)
    {
        if (!filesystem::exists(include_path)) continue;
        for (const auto& entry : filesystem::directory_iterator(include_path)) {
            vector<string> parts = split_string(entry.path().string(), "/");
            if (parts.empty()) continue;
            string file_name = parts[parts.size() - 1];

            if (file_path == file_name) file_path = entry.path().string();
        }
    }
}

vector<Token> lex_line(string const& filepath, int line_number, string line)
{
    line += " ";
    vector<Token> tokens;

    string cur;
    int i = 0;
    bool string_mode = false;

    while (i < int(line.size()))
    {
        char ch = line[i];
        if (ch == '"')
        {
            if (string_mode)
            {
                string loc = location_view(filepath, line_number, i - int(cur.size()));
                tokens.emplace_back(TokenType::STRING, cur, loc);
                cur = "";
            }
            string_mode = !string_mode;
        }
        else if (ch == ' ' && !cur.empty())
        {
            if (string_mode)
            {
                cur += ch;
            }
            else
            {
                int int_value;
                if (string_to_int(cur, int_value))
                {
                    string loc = location_view(filepath, line_number, i);
                    tokens.emplace_back(TokenType::INT, int_value, loc);
                }
                else
                {
                    string loc = location_view(filepath, line_number, i - int(cur.size()) + 1);
                    tokens.emplace_back(TokenType::WORD, cur, loc);
                }
                cur = "";
            }
        }
        else if (ch != ' ')
        {
            cur += ch;
        }

        ++i;
    }

    return tokens;
}

vector<Token> lex_file(string const& path)
{
    fstream file(path);

    if (!file.is_open())
    {
        compilation_error("File `" + path + "` not found in include directories");
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

    file.close();

    return tokens;
}

vector<Operation> parse_tokens_as_operations(vector<Token>& tokens, const vector<string>& include_paths)
{
    vector<Operation> program;
    map<string, vector<Operation>> bindings;

    assert(static_cast<int>(TokenType::COUNT) == 3, "Exhaustive token types handling");

    for (int i = 0; i < int(tokens.size()); ++i) {
        Token token = tokens[i];

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
                assert(static_cast<int>(OpType::COUNT) == 49, "Exhaustive operations handling");

                if (token.StringValue == "bind")
                {
                    if (++i == int(tokens.size()))
                    {
                        compilation_error(token.Loc, "Unfinished binding definition");
                        exit(1);
                    }

                    token = tokens[i];

                    if (token.Type != TokenType::WORD)
                    {
                        compilation_error(token.Loc, "Invalid token's type for binding's name. Expected token type `word`, but found " + HumanizedTokenTypes.at(token.Type));
                        exit(1);
                    }

                    string name = token.StringValue;
                    auto it = BuiltInOps.find(name);

                    if (it != BuiltInOps.end())
                    {
                        compilation_error(token.Loc, "Binding name can't conflict with built in operations. Use other name, instead of `" + name + "`");
                        exit(1);
                    }

                    int open_blocks = 0;

                    while (++i < int(tokens.size()) && (tokens[i].StringValue != "end" || open_blocks != 0))
                    {
                        token = tokens[i];

                        if (token.StringValue == name)
                        {
                            compilation_error(token.Loc, "Bindings are not support recursive calls");
                            exit(1);
                        }

                        if (token.StringValue == "if" || token.StringValue == "while") open_blocks++;

                        if (token.StringValue != "end" || open_blocks > 0)
                        {
                            switch (token.Type)
                            {
                                case TokenType::STRING:
                                {

                                    bindings[name].emplace_back(OpType::PUSH_STRING, unescape_string(token.StringValue), token.Loc);
                                    break;
                                }
                                case TokenType::INT:
                                {
                                    bindings[name].emplace_back(OpType::PUSH_INT, token.IntegerValue, token.Loc);
                                    break;
                                }
                                default:
                                {
                                    auto iter = BuiltInOps.find(token.StringValue);

                                    if (iter != BuiltInOps.end())
                                    {
                                        bindings[name].emplace_back(BuiltInOps.at(token.StringValue), token.Loc);
                                        break;
                                    }

                                    auto itt = bindings.find(token.StringValue);
                                    if (itt != bindings.end())
                                    {
                                        vector<Operation> binding = bindings[token.StringValue];
                                        for (const auto & op : binding) {
                                            bindings[name].push_back(op);
                                        }
                                        break;
                                    }

                                    compilation_error(token.Loc, "Undefined token: `" + token.StringValue + "`");
                                    exit(1);
                                }
                            }
                        }

                        if (open_blocks > 0 && token.StringValue == "end") open_blocks--;
                    }
                }
                else if (token.StringValue == "use")
                {
                    if (++i == int(tokens.size()))
                    {
                        compilation_error(token.Loc, "Unfinished using definition. Expected filepath, but found nothing");
                        exit(1);
                    }

                    token = tokens[i];

                    if (token.Type != TokenType::STRING)
                    {
                        compilation_error(token.Loc, "Expected type `string` after `use` keyword, bot found " + HumanizedTokenTypes.at(token.Type));
                        exit(1);
                    }

                    string file_path = token.StringValue;

                    extend_with_include_directories(file_path, include_paths);

                    vector<Token> using_tokens = lex_file(file_path);

                    vector<Token> temp_tokens = tokens;

                    tokens = using_tokens;

                    for (size_t j = 0; j < temp_tokens.size(); ++j)
                    {
                        Token tt = temp_tokens[j];
                        if ((tt.Type != TokenType::STRING && tt.StringValue == "use") || (j > 0 && temp_tokens[j-1].Type != TokenType::STRING && temp_tokens[j-1].StringValue == "use")) continue;
                        tokens.push_back(tt);
                    }
                    i = -1;
                    program = vector<Operation>();
                }
                else
                {
                    try
                    {
                        program.emplace_back(BuiltInOps.at(token.StringValue), token.Loc);
                    }
                    catch(...)
                    {
                        auto it = bindings.find(token.StringValue);

                        if (it != bindings.end())
                        {
                            for (const auto& binding : bindings.at(token.StringValue)) {
                                program.push_back(binding);
                            }
                        }
                        else
                        {
                            compilation_error(token.Loc, "Undefined token: `" + token.StringValue + "`");
                            exit(1);
                        }
                    }
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

    assert(static_cast<int>(OpType::COUNT) == 49, "Exhaustive operations handling. Not all operations should be handled in here");

    for (int i = 0; i < int(program.size()); ++i) {
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
                if (crossreference_stack.empty())
                {
                    compilation_error(op.Loc, "`else` operation can only be used in `if` block");
                    exit(1);
                }
                int pos = crossreference_stack.top();
                crossreference_stack.pop();
                program[pos].JumpTo = i + 1;
                crossreference_stack.push(i);
                break;
            }
            case OpType::DO:
            {
                if (crossreference_stack.empty())
                {
                    compilation_error(op.Loc, "`do` operation can only be used in `while` block");
                    exit(1);
                }
                program[i].JumpTo = crossreference_stack.top();
                crossreference_stack.pop();
                crossreference_stack.push(i);
                break;
            }
            case OpType::END:
            {
                if (crossreference_stack.empty())
                {
                    compilation_error(op.Loc, "`end` operation can only be used to close other blocks");
                    exit(1);
                }
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
                        compilation_error(op.Loc, "Only `if` and `while` blocks can be closed with `end` keyword");
                        exit(1);
                    }
                }
            }
        }
    }

    if (!crossreference_stack.empty())
    {
        int latest_pos = crossreference_stack.top();
        crossreference_stack.pop();

        compilation_error(program[latest_pos].Loc, "Not all blocks was closed with `end` keyword");
        exit(1);
    }
}

void type_check_program(const vector<Operation>& program)
{
    assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

    stack<Type> type_checking_stack;

    for (const auto& op : program) {
        assert(static_cast<int>(OpType::COUNT) == 49, "Exhaustive operations handling");

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
                type_checking_stack.emplace(DataType::INT, op.Loc);
                type_checking_stack.emplace(DataType::PTR, op.Loc);
                break;
            }
            case OpType::PLUS:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    compilation_error(op.Loc, "Not enough arguments for `+` operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                }
                else if ((a.Code == DataType::PTR && b.Code == DataType::INT) || (b.Code == DataType::PTR && a.Code == DataType::INT)) {
                    type_checking_stack.emplace(DataType::PTR, op.Loc);
                }
                else {
                    compilation_error(op.Loc, "Invalid arguments types for `+` operation. Expected 2 `int`s or pair of `int` and `ptr`, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                }
                break;
            }
            case OpType::MINUS:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    compilation_error(op.Loc, "Not enough arguments for `-` operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                }
                else if (a.Code == DataType::INT && b.Code == DataType::PTR) {
                    type_checking_stack.emplace(DataType::PTR, op.Loc);
                }
                else {
                    compilation_error(op.Loc, "Invalid arguments types for `-` operation. Expected 2 `int`s, or `ptr` and `int`, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                }
                break;
            }
            case OpType::MUL:
            case OpType::DIV:
            case OpType::MOD:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else {
                    compilation_error(op.Loc, "Invalid arguments types for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 `int`s, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
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
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
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
                    compilation_error(op.Loc, "Invalid arguments types for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 `int`s or 2 `bool`s, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                }
                break;
            }
            case OpType::SHL:
            case OpType::SHR:
            {
                assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

                if (type_checking_stack.size() < 2) {
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::INT, op.Loc);
                } else {
                    compilation_error(op.Loc, "Invalid arguments types for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 `int`s, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
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
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                if (a.Code == DataType::INT && b.Code == DataType::INT) {
                    type_checking_stack.emplace(DataType::BOOL, op.Loc);
                } else if (a.Code == DataType::BOOL && b.Code == DataType::BOOL) {
                    compilation_error(op.Loc, "Use `band`, `bor` and `xor` operations to compare booleans. Expected 2 `int`s, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                } else {
                    compilation_error(op.Loc, "Only integer values can be compared. Expected 2 `int`s, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                }
                break;
            }
            case OpType::NOT:
            {
                if (type_checking_stack.empty()) {
                    compilation_error(op.Loc, "Not enough arguments for `not` operation. Expected 1 arguments, but found 0");
                    exit(1);
                }
                Type a = type_checking_stack.top();
                if (a.Code != DataType::BOOL)
                {
                    compilation_error(op.Loc, "Expected type `bool` for `not` operation, but found " + HumanizedDataTypes.at(a.Code));
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
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 1 argument, but found 0");
                    exit(1);
                }

                Type top = type_checking_stack.top();
                type_checking_stack.pop();

                if (top.Code != DataType::BOOL)
                {
                    compilation_error(op.Loc, "Unexpected argument's type for " + HumanizedOpTypes.at(op.Type) + " operation. Expected type `bool`, but found " + HumanizedDataTypes.at(top.Code));
                    exit(1);
                }
                break;
            }
            case OpType::ELSE:
            case OpType::WHILE:
            case OpType::END:
            {
                // No type checking needed for these operations
                break;
            }
            case OpType::BIND:
            {
                assert(false, "Unreachable. All bindings should be expanded at the compilation step");
            }
            case OpType::MEM:
            {
                type_checking_stack.emplace(DataType::PTR, op.Loc);
                break;
            }
            case OpType::LOAD8:
            case OpType::LOAD64:
            {
                if (type_checking_stack.empty())
                {
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 1 argument, but found 0");
                    exit(1);
                }

                Type top = type_checking_stack.top();
                type_checking_stack.pop();

                if (top.Code != DataType::PTR)
                {
                    compilation_error(op.Loc, "Unexpected argument type for " + HumanizedOpTypes.at(op.Type) + " operation. Expected `ptr`, but found " + HumanizedDataTypes.at(top.Code));
                    exit(1);
                }

                type_checking_stack.emplace(DataType::INT, op.Loc);

                break;
            }
            case OpType::STORE8:
            case OpType::STORE64:
            {
                if (type_checking_stack.size() < 2)
                {
                    compilation_error(op.Loc, "Not enough arguments for " + HumanizedOpTypes.at(op.Type) + " operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code != DataType::INT || b.Code != DataType::PTR)
                {
                    compilation_error(op.Loc, "Unexpected argument's types for " + HumanizedOpTypes.at(op.Type) + " operation. Expected `int` and `ptr`, but found " + HumanizedDataTypes.at(b.Code) + " and " + HumanizedDataTypes.at(a.Code));
                    exit(1);
                }
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
            }
            case OpType::PUT:
            {
                if (type_checking_stack.empty())
                {
                    compilation_error(op.Loc, "Not enough arguments for `put` operation. Expected 1 argument, but found 0");
                    exit(1);
                }
                Type top = type_checking_stack.top();
                type_checking_stack.pop();
                break;
            }
            case OpType::FPUTS:
            {
                if (type_checking_stack.size() < 3)
                {
                    compilation_error(op.Loc, "Not enough arguments for `fputs` operation. Expected 3 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                Type c = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code != DataType::INT || b.Code != DataType::PTR || c.Code != DataType::INT)
                {
                    compilation_error(op.Loc, "Unexpected argument's types for `fputs` operation. Expected `int`, `ptr` and `int`, but found " + HumanizedDataTypes.at(a.Code) + " and " + HumanizedDataTypes.at(b.Code));
                    exit(1);
                }

                break;
            }
            case OpType::COPY:
            {
                if (type_checking_stack.empty())
                {
                    compilation_error(op.Loc, "Not enough arguments for `copy` operation. Expected 1 argument, but found 0");
                    exit(1);
                }

                type_checking_stack.push(type_checking_stack.top());

                break;
            }
            case OpType::OVER:
            {
                if (type_checking_stack.size() < 2)
                {
                    compilation_error(op.Loc, "Not enough arguments for `over` operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();

                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                type_checking_stack.push(b);
                type_checking_stack.push(a);
                type_checking_stack.push(b);

                break;
            }
            case OpType::SWAP:
            {
                if (type_checking_stack.size() < 2)
                {
                    compilation_error(op.Loc, "Not enough arguments for `swap` operation. Expected 2 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();

                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                type_checking_stack.push(a);
                type_checking_stack.push(b);

                break;
            }
            case OpType::SWAP2:
            {
                if (type_checking_stack.size() < 4)
                {
                    compilation_error(op.Loc, "Not enough arguments for `2swap` operation. Expected 4 arguments, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();

                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                Type c = type_checking_stack.top();
                type_checking_stack.pop();

                Type d = type_checking_stack.top();
                type_checking_stack.pop();

                type_checking_stack.push(b);
                type_checking_stack.push(a);
                type_checking_stack.push(d);
                type_checking_stack.push(c);

                break;
            }
            case OpType::DROP:
            {
                if (type_checking_stack.empty())
                {
                    compilation_error(op.Loc, "Not enough arguments for `drop` operation. Expected 1 argument, but found 0");
                    exit(1);
                }

                type_checking_stack.pop();

                break;
            }
            case OpType::ROT:
            {
                if (type_checking_stack.size() < 3)
                {
                    compilation_error(op.Loc, "Not enough arguments for `rot` operation. Expected 3 argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();
                Type c = type_checking_stack.top();
                type_checking_stack.pop();

                type_checking_stack.push(a);
                type_checking_stack.push(c);
                type_checking_stack.push(b);

                break;
            }
            case OpType::SYSCALL0:
            {
                if (type_checking_stack.empty())
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall0` operation. Expected 1 argument, but found 0");
                    exit(1);
                }

                type_checking_stack.pop();
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL1:
            {
                int n = 1;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall1` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL2:
            {
                int n = 2;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall2` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL3:
            {
                int n = 3;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall3` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL4:
            {
                int n = 4;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall4` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL5:
            {
                int n = 5;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall5` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            case OpType::SYSCALL6:
            {
                int n = 6;

                if (int(type_checking_stack.size()) < n + 1)
                {
                    compilation_error(op.Loc, "Not enough arguments for `syscall6` operation. Expected " + to_string((n + 1)) + " argument, but found " + to_string(type_checking_stack.size()));
                    exit(1);
                }

                for (int j = 0; j <= n; ++j) {
                    type_checking_stack.pop();
                }
                type_checking_stack.emplace(DataType::INT, op.Loc);
                break;
            }
            default:
                assert(false, "Unreachable");
        }
    }

    if (!type_checking_stack.empty())
    {
        Type top = type_checking_stack.top();
        cerr << top.Loc << ": ERROR: Unhandled data in the stack." << endl <<
            "Stack size: " << type_checking_stack.size() << endl <<
            "Stack values:" << endl;
        while (!type_checking_stack.empty())
        {
            top = type_checking_stack.top();
            type_checking_stack.pop();
            cerr << "  " << top.Loc << ": " << HumanizedDataTypes.at(top.Code) << endl;
        }
        exit(1);
    }
}

void generate_nasm_linux_x86_64(const string& output_file_path, vector<Operation> program)
{
    std::ofstream out(output_file_path);

    if (!out.is_open()) {
        compilation_error("Can't open file: " + output_file_path);
        exit(1);
    }

    map<string, int> strings;

    string output_content;
    complete_string(output_content, "BITS 64");

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
        complete_string(output_content, "; -- put --");
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

    assert(static_cast<int>(OpType::COUNT) == 49, "Exhaustive operations handling");

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
                    strings[op.StringValue] = int(strings.size());
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
            case OpType::BIND:
            {
                assert(false, "Unreachable. All bindings should be expanded at the compilation step");
            }
            case OpType::MEM:
            {
                complete_string(output_content, "    ; -- mem --");
                complete_string(output_content, "    push    mem");
                break;
            }
            case OpType::LOAD8:
            {
                complete_string(output_content, "    ; -- load8 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    xor     rbx, rbx");
                complete_string(output_content, "    mov     bl, [rax]");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::STORE8:
            {
                complete_string(output_content, "    ; -- store8 --");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    mov     [rax], bl");
                break;
            }
            case OpType::LOAD64:
            {
                complete_string(output_content, "    ; -- load64 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    xor     rbx, rbx");
                complete_string(output_content, "    mov     rbx, [rax]");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::STORE64:
            {
                complete_string(output_content, "    ; -- store64 --");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    mov     [rax], rbx");
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
            }
            case OpType::PUT:
            {
                complete_string(output_content, "    ; -- put --");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    call    put");
                break;
            }
            case OpType::FPUTS:
            {
                complete_string(output_content, "    ; -- fputs --");
                complete_string(output_content, "    mov     rax, 1");
                complete_string(output_content, "    pop     rdi");
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
                if (it == strings.end()) strings[value] = int(strings.size());

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
            case OpType::SWAP2:
            {
                complete_string(output_content, "    ; -- 2swap --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rcx");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    push    rbx");
                complete_string(output_content, "    push    rax");
                complete_string(output_content, "    push    rdx");
                complete_string(output_content, "    push    rcx");
                break;
            }
            case OpType::DROP:
            {
                complete_string(output_content, "    ; -- drop --");
                complete_string(output_content, "    pop     rax");
                break;
            }
            case OpType::ROT:
            {
                complete_string(output_content, "    ; -- rot --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rbx");
                complete_string(output_content, "    pop     rcx");
                complete_string(output_content, "    push    rax");
                complete_string(output_content, "    push    rcx");
                complete_string(output_content, "    push    rbx");
                break;
            }
            case OpType::SYSCALL0:
            {
                complete_string(output_content, "    ; -- syscall0 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL1:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL2:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL3:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL4:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    pop     r10");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL5:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    pop     r10");
                complete_string(output_content, "    pop     r8");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL6:
            {
                complete_string(output_content, "    ; -- syscall --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    pop     r10");
                complete_string(output_content, "    pop     r8");
                complete_string(output_content, "    pop     r9");
                complete_string(output_content, "    syscall");
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

    out << endl << "segment .bss" << endl;
    out << "    mem resb 640000" << endl;
}

void compile(const string& compiler_path, const string& path, vector<Operation> program, bool run_after_compilation, bool silent_mode)
{
    string filename = trim_string(path, FILE_EXTENSION);

    filesystem::path file_path(path);

    if (!filesystem::exists(file_path))
    {
        usage(compiler_path);
        compilation_error("File '" + file_path.string() + "' doesn't exists");
        exit(1);
    }

#ifdef __x86_64__
    if (!silent_mode) cout << "[INFO] Generating assembly -> " << filename << ".asm" << endl;
    generate_nasm_linux_x86_64(filename + ".asm", std::move(program));

    if (!silent_mode) cout << "[INFO] Compiling assembly with NASM" << endl;
    execute_command(silent_mode, "nasm -felf64 -o " + filename + ".o " + filename + ".asm");
    if (!silent_mode) cout << "[INFO] Object file generated: " << filename << ".asm -> " << filename << ".o" << endl;

    execute_command(silent_mode, "ld -o " + filename + " " + filename + ".o");
    if (!silent_mode) cout << "[INFO] Compiled to " << filename << endl;

    if (run_after_compilation) execute_command(silent_mode, filename);

    return;
#endif

    compilation_error("Your processor's architecture is not supported yet");
    exit(1);
}

int main(int argc, char* argv[])
{
    assert(HumanizedOpTypes.size() == static_cast<int>(OpType::COUNT), "Exhaustive checking of humanized operations definition");
    assert(HumanizedDataTypes.size() == static_cast<int>(DataType::COUNT), "Exhaustive checking of humanized data types definition");
    assert(BuiltInOps.size() == static_cast<int>(OpType::COUNT) - 2, "Exhaustive checking of built in operations definition");

    vector<string> args(argv, argv + argc);

    string compiler_path = shift_vector(args);
    vector<string> include_paths = {"./std/"};
    string path;
    bool run_after_compilation;
    bool silent_mode;

    if (args.empty())
    {
        usage(compiler_path);
        compilation_error("No subcommand provided");
        exit(1);
    }

    while (!args.empty())
    {
        string arg = shift_vector(args);

        if (arg == "-r") run_after_compilation = true;
        else if (arg == "-s") silent_mode = true;
        else if (arg == "-I")
        {
            if (args.empty())
            {
                compilation_error("No path provided after `-I` flag");
                exit(1);
            }

            include_paths.push_back(shift_vector(args));
        }
        else path = arg;
    }

    filesystem::path file_path(path);

    if (!filesystem::exists(file_path))
    {
        usage(compiler_path);
        compilation_error("File '" + file_path.string() + "' doesn't exists");
        exit(1);
    }

    vector<Token> tokens = lex_file(file_path.string());

    vector<Operation> program = parse_tokens_as_operations(tokens, include_paths);

    crossreference_blocks(program);

    type_check_program(program);

    compile(compiler_path, path, program, run_after_compilation, silent_mode);

    return 0;
}