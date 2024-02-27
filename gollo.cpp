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
    USE,
    PUT,
    PUTS,
    HERE,
    COPY,
    OVER,
    SWAP,
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
        {OpType::USE, "`use`"},
        {OpType::PUT, "`put`"},
        {OpType::PUTS, "`puts`"},
        {OpType::HERE, "`here`"},
        {OpType::COPY, "`copy`"},
        {OpType::OVER, "`over`"},
        {OpType::SWAP, "`swap`"},
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
        {"use", OpType::USE},
        {"put", OpType::PUT},
        {"puts", OpType::PUTS},
        {"here", OpType::HERE},
        {"copy", OpType::COPY},
        {"over", OpType::OVER},
        {"swap", OpType::SWAP},
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
    cerr << "Usage: " << compiler_path << " <SUBCOMMAND> ./examples/goo.glo" << endl;
    cerr << "SUBCOMMANDS:" << endl;
    cerr << "    run                 Instantly run the program" << endl;
    cerr << "    compile [ARGS]      Compile the program into an executable" << endl;
    cerr << "      ARGS:" << endl;
    cerr << "          -r            Run compiled program after compilation" << endl;
    cerr << "          -s            Silent mode for compiler" << endl;
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

int find_col(string const& line, int start, bool (*predicate)(char))
{
    while (start < int(line.size()) && !predicate(line[start])) {
        start++;
    }
    return start;
}

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

vector<Token> lex_line(string const& filepath, int line_number, string line)
{
    line += " ";
    vector<Token> tokens;

    string cur = "";
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

    file.close();

    return tokens;
}

vector<Operation> parse_tokens_as_operations(vector<Token>& tokens)
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
                assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling");

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
                else if (token.StringValue == "bind")
                {
                    if (++i == int(tokens.size()))
                    {
                        cerr << token.Loc << ": ERROR: Unfinished binding definition" << endl;
                        exit(1);
                    }

                    token = tokens[i];

                    if (token.Type != TokenType::WORD)
                    {
                        cerr << token.Loc << ": ERROR: Invalid token's type for binding's name. Expected " << HumanizedTokenTypes.at(TokenType::WORD) << ", but found " << HumanizedTokenTypes.at(token.Type) << endl;
                        exit(1);
                    }

                    string name = token.StringValue;
                    auto it = BuiltInOps.find(name);

                    if (it != BuiltInOps.end())
                    {
                        cerr << token.Loc << ": ERROR: Binding name can't conflict with built in operations. Use other name, instead of `" << name << "`" << endl;
                        exit(1);
                    }

                    int open_blocks = 0;

                    while (++i < int(tokens.size()) && (tokens[i].StringValue != "end" || open_blocks != 0))
                    {
                        token = tokens[i];

                        if (token.StringValue == name)
                        {
                            cerr << token.Loc << ": ERROR: Bindings are not support recursive calls" << endl;
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

                                    cerr << token.Loc << ": ERROR: Undefined token: '" << token.StringValue << "'" << endl;
                                    exit(1);
                                    break;
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
                        cerr << token.Loc << ": ERROR: Unfinished using definition. Expected filepath, but found nothing" << endl;
                        exit(1);
                    }

                    token = tokens[i];

                    if (token.Type != TokenType::STRING)
                    {
                        cerr << token.Loc << ": ERROR: Expected type " << HumanizedTokenTypes.at(TokenType::STRING) << " after " << HumanizedOpTypes.at(OpType::USE) << " keyword, bot found " <<  HumanizedTokenTypes.at(token.Type) << endl;
                        exit(1);
                    }

                    vector<Token> using_tokens = lex_file(token.StringValue);

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
                else if (token.StringValue == "rot")
                {
                    program.emplace_back(OpType::ROT, token.Loc);
                }
                else if (token.StringValue == "syscall0")
                {
                    program.emplace_back(OpType::SYSCALL0, token.Loc);
                }
                else if (token.StringValue == "syscall1")
                {
                    program.emplace_back(OpType::SYSCALL1, token.Loc);
                }
                else if (token.StringValue == "syscall2")
                {
                    program.emplace_back(OpType::SYSCALL2, token.Loc);
                }
                else if (token.StringValue == "syscall3")
                {
                    program.emplace_back(OpType::SYSCALL3, token.Loc);
                }
                else if (token.StringValue == "syscall4")
                {
                    program.emplace_back(OpType::SYSCALL4, token.Loc);
                }
                else if (token.StringValue == "syscall5")
                {
                    program.emplace_back(OpType::SYSCALL5, token.Loc);
                }
                else if (token.StringValue == "syscall6")
                {
                    program.emplace_back(OpType::SYSCALL6, token.Loc);
                }
                else
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
                        cerr << token.Loc << ": ERROR: Undefined token: '" << token.StringValue << "'" << endl;
                        exit(1);
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

    assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling. Not all operations should be handled in here");

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
                        assert(false, "Only " << HumanizedOpTypes.at(OpType::IF) << " and " << HumanizedOpTypes.at(OpType::WHILE) << " blocks can be closed with " << HumanizedOpTypes.at(OpType::END));
                    }
                }
            }
        }
    }

    if (!crossreference_stack.empty())
    {
        int latest_pos = crossreference_stack.top();
        crossreference_stack.pop();

        cerr << program[latest_pos].Loc << ": ERROR: Not all blocks was closed with " << HumanizedOpTypes.at(OpType::END) << endl;
        exit(1);
    }
}

void type_check_program(const vector<Operation>& program)
{
    assert(static_cast<int>(DataType::COUNT) == 3, "Exhaustive data types handling");

    stack<Type> type_checking_stack;

    for (const auto& op : program) {
        assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling");

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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << to_string(type_checking_stack.size()) << endl;
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
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << "s or pair of " << HumanizedDataTypes.at(DataType::INT) << " and " << HumanizedDataTypes.at(DataType::PTR) << ", but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
                    exit(1);
                }
                break;
            }
            case OpType::MINUS:
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
                }
                else if (a.Code == DataType::INT && b.Code == DataType::PTR) {
                    type_checking_stack.emplace(DataType::PTR, op.Loc);
                }
                else {
                    cerr << op.Loc << ": ERROR: Invalid arguments types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 " << HumanizedDataTypes.at(DataType::INT) << "s, or " << HumanizedDataTypes.at(DataType::PTR) << " and " << HumanizedDataTypes.at(DataType::INT) << " (in this order), but found " << HumanizedDataTypes.at(b.Code) << " and " << HumanizedDataTypes.at(a.Code) << "" << endl;
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
            case OpType::BIND:
            {
                assert(false, "Unreachable. All bindings should be expanded at the compilation step");
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
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
                type_checking_stack.pop();
                break;
            }
            case OpType::PUTS:
            {
                if (type_checking_stack.size() < 2)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << type_checking_stack.size() << endl;
                    exit(1);
                }

                Type a = type_checking_stack.top();
                type_checking_stack.pop();
                Type b = type_checking_stack.top();
                type_checking_stack.pop();

                if (a.Code != DataType::PTR || b.Code != DataType::INT)
                {
                    cerr << op.Loc << ": ERROR: Unexpected argument's types for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << HumanizedDataTypes.at(DataType::PTR) << " and " << HumanizedDataTypes.at(DataType::INT) << ", but found " << HumanizedDataTypes.at(a.Code) << " and " << HumanizedDataTypes.at(b.Code) << endl;
                    exit(1);
                }

                break;
            }
            case OpType::COPY:
            {
                if (type_checking_stack.empty())
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
                    exit(1);
                }

                type_checking_stack.push(type_checking_stack.top());

                break;
            }
            case OpType::OVER:
            {
                if (type_checking_stack.size() < 2)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 2 arguments, but found " << type_checking_stack.size() << endl;
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
            case OpType::ROT:
            {
                if (type_checking_stack.size() < 3)
                {
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 3 argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected 1 argument, but found 0" << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
                    cerr << op.Loc << ": ERROR: Not enough arguments for " << HumanizedOpTypes.at(op.Type) << " operation. Expected " << n + 1 << " argument, but found " << type_checking_stack.size() << endl;
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
        cerr << top.Loc << ": ERROR: Unhandled data in the stack" << endl;
        exit(1);
    }
}

void run_program(vector<Operation> program)
{
    assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling");

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
                int n = int(op.StringValue.length());

                runtime_stack.push(n);

                op.Address = int(strings_size);
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
            case OpType::BIND:
            {
                assert(false, "Unreachable. All bindings should be expanded at the compilation step");
                ++i;
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
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
                int n = int(op.Loc.length());

                runtime_stack.push(n);

                op.Address = int(strings_size);
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
            case OpType::ROT:
            {
                int a = runtime_stack.top();
                runtime_stack.pop();
                int b = runtime_stack.top();
                runtime_stack.pop();
                int c = runtime_stack.top();
                runtime_stack.pop();

                runtime_stack.push(a);
                runtime_stack.push(c);
                runtime_stack.push(b);
                ++i;
                break;
            }
            case OpType::SYSCALL0:
            case OpType::SYSCALL1:
            case OpType::SYSCALL2:
            case OpType::SYSCALL3:
            case OpType::SYSCALL4:
            case OpType::SYSCALL5:
            case OpType::SYSCALL6:
            {
                assert(false, "Not implemented for running mode");
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

    assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling");

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
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
                break;
            }
            case OpType::PUT:
            {
                complete_string(output_content, "    ; -- put --");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    call    put");
                break;
            }
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
                complete_string(output_content, "    ; -- syscall1 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL2:
            {
                complete_string(output_content, "    ; -- syscall2 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL3:
            {
                complete_string(output_content, "    ; -- syscall3 --");
                complete_string(output_content, "    pop     rax");
                complete_string(output_content, "    pop     rdi");
                complete_string(output_content, "    pop     rsi");
                complete_string(output_content, "    pop     rdx");
                complete_string(output_content, "    syscall");
                break;
            }
            case OpType::SYSCALL4:
            {
                complete_string(output_content, "    ; -- syscall4 --");
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
                complete_string(output_content, "    ; -- syscall5 --");
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
                complete_string(output_content, "    ; -- syscall6 --");
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
}

void generate_assembly_macos_arm64(const string& output_file_path, vector<Operation> program)
{
    std::ofstream out(output_file_path);

    if (!out.is_open()) {
        cerr << "ERROR: Can't open file: " << output_file_path << endl;
        exit(1);
    }

    map<string, int> strings;

    string output_content;

    complete_string(output_content, ".text");
    complete_string(output_content, ".global _main\n");
    complete_string(output_content, "_main:\n");

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
        assert(false, "`put` operation is not implemented yet");
    }

    assert(static_cast<int>(OpType::COUNT) == 43, "Exhaustive operations handling");

    for (size_t i = 0; i < program.size(); ++i) {
        Operation op = program[i];

        switch (op.Type)
        {
            case OpType::PUSH_INT:
            {
                complete_string(output_content, "    ; -- push int -- ");
                complete_string(output_content, "    mov     x0, #" + to_string(op.IntegerValue));
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::PUSH_STRING:
            {
                complete_string(output_content, "    ; -- push str --");
                assert(false, "`push str` operation is not implemented yet");
                break;
            }
            case OpType::PLUS:
            {
                complete_string(output_content, "    ; -- plus --");
                complete_string(output_content, "    ldp     x0, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    add     x0, x0, x1");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::MINUS:
            {
                complete_string(output_content, "    ; -- minus --");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x0, xzr, [sp], #16");
                complete_string(output_content, "    sub     x0, x0, x1");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::MUL:
            {
                complete_string(output_content, "    ; -- multiply --");
                complete_string(output_content, "    ldp     x0, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    mul     x0, x0, x1");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::DIV:
            {
                complete_string(output_content, "    ; -- division --");
                assert(false, "`division` operation is not implemented yet");
                break;
            }
            case OpType::MOD:
            {
                complete_string(output_content, "    ; -- mod --");
                assert(false, "`mod` operation is not implemented yet");
                break;
            }
            case OpType::BOR:
            {
                complete_string(output_content, "    ; -- binary or --");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x2, xzr, [sp], #16");
                complete_string(output_content, "    orr     x0, x1, x2");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::BAND:
            {
                complete_string(output_content, "    ; -- binary and --");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x2, xzr, [sp], #16");
                complete_string(output_content, "    and     x0, x1, x2");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::XOR:
            {
                complete_string(output_content, "    ; -- xor --");
                complete_string(output_content, "    ldp     x1, xzr, [sp], #16");
                complete_string(output_content, "    ldp     x2, xzr, [sp], #16");
                complete_string(output_content, "    eor     x0, x1, x2");
                complete_string(output_content, "    stp     x0, xzr, [sp, #-16]!");
                break;
            }
            case OpType::SHL:
            {
                complete_string(output_content, "    ; -- shift left --");
                assert(false, "`shl` operation is not implemented yet");
                break;
            }
            case OpType::SHR:
            {
                complete_string(output_content, "    ; -- shift right --");
                assert(false, "`shr` operation is not implemented yet");
                break;
            }
            case OpType::EQ:
            {
                complete_string(output_content, "    ; -- equal --");
                assert(false, "`equal` operation is not implemented yet");
                break;
            }
            case OpType::NE:
            {
                complete_string(output_content, "    ; -- not equal --");
                assert(false, "`not equal` operation is not implemented yet");
                break;
            }
            case OpType::LT:
            {
                complete_string(output_content, "    ; -- less --");
                assert(false, "`less` operation is not implemented yet");
                break;
            }
            case OpType::GT:
            {
                complete_string(output_content, "    ; -- greater --");
                assert(false, "`greater` operation is not implemented yet");
                break;
            }
            case OpType::LE:
            {
                complete_string(output_content, "    ; -- less or equal --");
                assert(false, "`less or equal` operation is not implemented yet");
                break;
            }
            case OpType::GE:
            {
                complete_string(output_content, "    ; -- greater or equal --");
                assert(false, "`greater or equal` operation is not implemented yet");
                break;
            }
            case OpType::NOT:
            {
                complete_string(output_content, "    ; -- not --");
                assert(false, "`not` operation is not implemented yet");
                break;
            }
            case OpType::TRUE:
            {
                complete_string(output_content, "    ; -- true --");
                assert(false, "`true` operation is not implemented yet");
                break;
            }
            case OpType::FALSE:
            {
                complete_string(output_content, "    ; -- false --");
                assert(false, "`false` operation is not implemented yet");
                break;
            }
            case OpType::IF:
            {
                complete_string(output_content, "    ; -- if --");
                assert(false, "`if` operation is not implemented yet");
                break;
            }
            case OpType::ELSE:
            {
                complete_string(output_content, "    ; -- else --");
                assert(false, "`else` operation is not implemented yet");
                break;
            }
            case OpType::END:
            {
                complete_string(output_content, "    ; -- end --");
                assert(false, "`end` operation is not implemented yet");
                break;
            }
            case OpType::DO:
            {
                complete_string(output_content, "    ; -- do --");
                assert(false, "`do` operation is not implemented yet");
                break;
            }
            case OpType::WHILE:
            {
                complete_string(output_content, "    ; -- while --");
                assert(false, "`while` operation is not implemented yet");
                break;
            }
            case OpType::BIND:
            {
                assert(false, "Unreachable. All bindings should be expanded at the compilation step");
                break;
            }
            case OpType::USE:
            {
                assert(false, "Unreachable. All `use` operations should be eliminated at the compilation step");
                break;
            }
            case OpType::PUT:
            {
                complete_string(output_content, "    ; -- put --");
                assert(false, "`put` operation is not implemented yet");
                break;
            }
            case OpType::PUTS:
            {
                complete_string(output_content, "    ; -- puts --");
                assert(false, "`puts` operation is not implemented yet");
                break;
            }
            case OpType::HERE:
            {
                string value = op.Loc;
                complete_string(output_content, "    ; -- here --");
                assert(false, "`here` operation is not implemented yet");
                break;
            }
            case OpType::COPY:
            {
                complete_string(output_content, "    ; -- copy --");
                assert(false, "`copy` operation is not implemented yet");
                break;
            }
            case OpType::OVER:
            {
                complete_string(output_content, "    ; -- over --");
                assert(false, "`over` operation is not implemented yet");
                break;
            }
            case OpType::SWAP:
            {
                complete_string(output_content, "    ; -- swap --");
                assert(false, "`swap` operation is not implemented yet");
                break;
            }
            case OpType::DROP:
            {
                complete_string(output_content, "    ; -- drop --");
                assert(false, "`drop` operation is not implemented yet");
                break;
            }
            case OpType::ROT:
            {
                complete_string(output_content, "    ; -- rot --");
                assert(false, "`rot` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL0:
            {
                complete_string(output_content, "    ; -- syscall0 --");
                assert(false, "`syscall0` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL1:
            {
                complete_string(output_content, "    ; -- syscall1 --");
                assert(false, "`syscall1` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL2:
            {
                complete_string(output_content, "    ; -- syscall2 --");
                assert(false, "`syscall2` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL3:
            {
                complete_string(output_content, "    ; -- syscall3 --");
                assert(false, "`syscall3` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL4:
            {
                complete_string(output_content, "    ; -- syscall4 --");
                assert(false, "`syscall4` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL5:
            {
                complete_string(output_content, "    ; -- syscall5 --");
                assert(false, "`syscall5` operation is not implemented yet");
                break;
            }
            case OpType::SYSCALL6:
            {
                complete_string(output_content, "    ; -- syscall6 --");
                assert(false, "`syscall6` operation is not implemented yet");
                break;
            }
            default:
                assert(false, "Unreachable");
        }
    }

    complete_string(output_content, "    ; -- exit --");
    complete_string(output_content, "    mov     x0, #0");
    complete_string(output_content, "    mov     x16, #1");
    complete_string(output_content, "    svc     #0");

//    if (!strings.empty()) complete_string(output_content, "\nsection .data");
//
    out << output_content;
//
//    for (const auto& pair : strings) {
//        out << "    str_" << pair.second << ": db ";
//
//        const string& s = pair.first;
//        for (size_t i = 0; i < s.size(); ++i) {
//            out << "0x" << setw(2) << setfill('0') << hex << static_cast<int>(s[i]);
//
//            if (i < s.size() - 1) {
//                out << ",";
//            }
//        }
//
//        out << endl;
//    }
}

void compile(const string& compiler_path, const string& path, vector<Operation> program, bool run_after_compilation, bool silent_mode)
{
    string filename = trim_string(path, FILE_EXTENSION);

    filesystem::path file_path(path);

    if (!filesystem::exists(file_path))
    {
        usage(compiler_path);
        cerr << "ERROR: File '" << file_path.string() << "' doesn't exists" << endl;
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

#ifdef __aarch64__
    if (!silent_mode) cout << "[INFO] Generating assembly -> " << filename << ".s" << endl;
    generate_assembly_macos_arm64(filename + ".s", std::move(program));

    if (!silent_mode) cout << "[INFO] Compiling assembly" << endl;
    execute_command(silent_mode, "as -o " + filename + ".o " + filename + ".s");
    if (!silent_mode) cout << "[INFO] Object file generated: " << filename << ".s -> " << filename << ".o" << endl;

    execute_command(silent_mode, "ld -o " + filename + " " + filename + ".o");
    if (!silent_mode) cout << "[INFO] Compiled to " << filename << endl;

    if (run_after_compilation) execute_command(silent_mode, filename);

    return;
#endif

    cerr << "ERROR: Your processor's architecture is not supported yet" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    assert(HumanizedOpTypes.size() == static_cast<int>(OpType::COUNT), "Exhaustive checking of humanized operations definition");
    assert(HumanizedDataTypes.size() == static_cast<int>(DataType::COUNT), "Exhaustive checking of humanized data types definition");
    assert(BuiltInOps.size() == static_cast<int>(OpType::COUNT) - 2, "Exhaustive checking of built in operations definition");

    vector<string> args(argv, argv + argc);

    string compiler_path = shift_vector(args);
    string path;
    string subcommand = shift_vector(args);
    if (subcommand != "run" && subcommand != "compile")
    {
        usage(compiler_path);
        cerr << "ERROR: Provided unknown subcommand: '" << subcommand << "'" << endl;
        exit(1);
    }

    bool run_after_compilation;
    bool silent_mode;

    while (!args.empty())
    {
        string arg = shift_vector(args);

        if (arg == "-r") run_after_compilation = true;
        else if (arg == "-s") silent_mode = true;
        else path = arg;
    }

    filesystem::path file_path(path);

    if (!filesystem::exists(file_path))
    {
        usage(compiler_path);
        cerr << "ERROR: File '" << file_path.string() << "' doesn't exists" << endl;
        exit(1);
    }

    vector<Token> tokens = lex_file(file_path.string());

    vector<Operation> program = parse_tokens_as_operations(tokens);

    crossreference_blocks(program);

    type_check_program(program);

    if (subcommand == "run")
    {
        run_program(program);
    }
    else if (subcommand == "compile")
    {
        compile(compiler_path, path, program, run_after_compilation, silent_mode);
    }
    return 0;
}