package lexer

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/internal/token"
	"gollo/pkg/assert"
	"gollo/pkg/log"
	st "gollo/pkg/stack"
	"os"
	"strconv"
	"strings"
)

func LexFile(compiler string, filepath string) []operation.Operation {
	_, err := os.Stat(filepath)
	if err != nil {
		log.Usage(compiler)
		log.Error("can't found a file: " + filepath)
		os.Exit(1)
	}

	byteContent, err := os.ReadFile(filepath)
	if err != nil {
		log.Usage(compiler)
		log.Error("can't read provided file: " + err.Error())
		os.Exit(1)
	}

	source := string(byteContent)

	lines := strings.Split(source, "\n")

	tokens := make([]token.Token, 0)

	for i := 0; i < len(lines); i++ {
		line := strings.Split(lines[i], "//")[0] + "\n"
		tokens = append(tokens, lexLine(filepath, i+1, line)...)
	}

	program := parseTokensAsOperations(tokens)
	crossreferehcedProgram := crossreferenceBlocks(program)

	return crossreferehcedProgram
}

func lexLine(filepath string, number int, line string) []token.Token {
	tokens := make([]token.Token, 0)
	var cur string

	assert.Assert(token.Count == 3, "Exhaustive tokens handling in lexer.parseTokensAsOperations()")

	for i := 0; i < len(line); i++ {
		parts := strings.Split(line, "\"")
		for ip, part := range parts {
			if ip%2 != 0 {
				tokens = append(tokens, token.Token{
					Code:        token.STRING,
					StringValue: part,
					Loc:         fmt.Sprintf("%s:%d:%d", filepath, number, i-len(cur)+1),
				})
			} else {
				if (line[i] == ' ' || line[i] == '\n') && cur != "" {
					var tok token.Token

					val, err := strconv.Atoi(cur)
					if err != nil {
						_, exists := operation.BuiltIn[cur]
						var code int
						if exists {
							code = token.WORD
						} else {
							code = token.STRING
						}
						tok = token.Token{
							Code:        code,
							StringValue: cur,
							Loc:         fmt.Sprintf("%s:%d:%d", filepath, number, i-len(cur)+1),
						}
					} else {
						tok = token.Token{
							Code:         token.INT,
							IntegerValue: val,
							Loc:          fmt.Sprintf("%s:%d:%d", filepath, number, i-len(cur)+1),
						}
					}

					tokens = append(tokens, tok)
					cur = ""
				}

				if line[i] != ' ' && line[i] != '\n' {
					cur += string(line[i])
				}
			}
		}
	}

	return tokens
}

func crossreferenceBlocks(program []operation.Operation) []operation.Operation {
	stack := st.New()

	assert.Assert(operation.Count == 32, "Exhaustive operations handling in lexer.crossreferenceBlocks(). Not all operations should be handled in here.")

	i := 0
	for i < len(program) {
		op := program[i]

		switch op.Code {
		case operation.IF:
			stack.Push(i)
		case operation.ELSE:
			pos := stack.Pop()
			program[pos].JumpTo = i + 1
			stack.Push(i)
		case operation.DO:
			program[i].JumpTo = stack.Pop()
			stack.Push(i)
		case operation.WHILE:
			stack.Push(i)
		case operation.END:
			pos := stack.Pop()

			if program[pos].Code == operation.IF || program[pos].Code == operation.ELSE {
				program[pos].JumpTo = i
			} else if program[pos].Code == operation.DO {
				program[i].JumpTo = program[pos].JumpTo
				program[pos].JumpTo = i + 1
			} else {
				assert.Assert(false, "`end` can only close `if-else` and `do-while` blocks")
			}
		}

		i++
	}

	if !stack.IsEmpty() {
		log.Error(fmt.Sprintf("%s: block didn't closed with `end`", program[stack.Pop()].Loc))
		os.Exit(1)
	}

	return program
}

func parseTokensAsOperations(tokens []token.Token) []operation.Operation {
	program := make([]operation.Operation, 0)

	assert.Assert(operation.Count == 32, "Exhaustive operations handling in lexer.parseTokensAsOperations()")
	assert.Assert(token.Count == 3, "Exhaustive tokens handling in lexer.parseTokensAsOperations()")

	for _, tok := range tokens {
		switch tok.Code {
		case token.INT:
			program = append(program, operation.PushInt(tok.IntegerValue, tok.Loc))
		case token.STRING:
			assert.Assert(false, "pushing strings not implemented yet")
		case token.WORD:
			switch tok.StringValue {
			case "+":
				program = append(program, operation.Plus(tok.Loc))
			case "-":
				program = append(program, operation.Minus(tok.Loc))
			case "*":
				program = append(program, operation.Multiply(tok.Loc))
			case "/":
				program = append(program, operation.Division(tok.Loc))
			case "%":
				program = append(program, operation.Mod(tok.Loc))
			case "bor":
				program = append(program, operation.Bor(tok.Loc))
			case "band":
				program = append(program, operation.Band(tok.Loc))
			case "xor":
				program = append(program, operation.Xor(tok.Loc))
			case "shl":
				program = append(program, operation.Shl(tok.Loc))
			case "shr":
				program = append(program, operation.Shr(tok.Loc))
			case "==":
				program = append(program, operation.Equal(tok.Loc))
			case "!=":
				program = append(program, operation.NotEqual(tok.Loc))
			case "<":
				program = append(program, operation.Less(tok.Loc))
			case ">":
				program = append(program, operation.Greater(tok.Loc))
			case "<=":
				program = append(program, operation.LessOrEqual(tok.Loc))
			case ">=":
				program = append(program, operation.GreaterOrEqual(tok.Loc))
			case "if":
				program = append(program, operation.If(tok.Loc))
			case "else":
				program = append(program, operation.Else(tok.Loc))
			case "end":
				program = append(program, operation.End(tok.Loc))
			case "do":
				program = append(program, operation.Do(tok.Loc))
			case "while":
				program = append(program, operation.While(tok.Loc))
			case "put":
				program = append(program, operation.Put(tok.Loc))
			case "copy":
				program = append(program, operation.Copy(tok.Loc))
			case "2copy":
				program = append(program, operation.TwoCopy(tok.Loc))
			case "swap":
				program = append(program, operation.Swap(tok.Loc))
			case "drop":
				program = append(program, operation.Drop(tok.Loc))
			case "over":
				program = append(program, operation.Over(tok.Loc))
			case "syscall0":
				program = append(program, operation.Syscall0(tok.Loc))
			case "syscall1":
				program = append(program, operation.Syscall1(tok.Loc))
			case "syscall2":
				program = append(program, operation.Syscall2(tok.Loc))
			case "syscall3":
				program = append(program, operation.Syscall3(tok.Loc))
			default:
				log.Error(fmt.Sprintf("%s: can't parse token: %s", tok.Loc, tok.StringValue))
				os.Exit(1)
			}
		}
	}

	return program
}
