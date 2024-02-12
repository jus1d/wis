package lexer

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/log"
	st "gollo/pkg/stack"
	"os"
	"strconv"
	"strings"
)

type Token struct {
	Filepath string
	Line     int
	Col      int
	Word     string
}

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

	tokens := make([]Token, 0)

	for i := 0; i < len(lines); i++ {
		tokens = append(tokens, lexLine(filepath, i+1, lines[i]+"\n")...)
	}

	program := parseTokensAsOperations(tokens)
	crossreferehcedProgram := crossreferenceBlocks(program)

	return crossreferehcedProgram
}

func crossreferenceBlocks(program []operation.Operation) []operation.Operation {
	stack := st.New()

	i := 0
	for i < len(program) {
		op := program[i]
		if op.Code == operation.OpIf {
			stack.Push(i)
		} else if op.Code == operation.OpElse {
			pos := stack.Pop()
			program[pos].JumpTo = i + 1
			stack.Push(i)
		} else if op.Code == operation.OpEnd {
			pos := stack.Pop()
			program[pos].JumpTo = i
		}
		i++
	}

	return program
}

func lexLine(filepath string, number int, line string) []Token {
	tokens := make([]Token, 0)
	var cur string

	for i := 0; i < len(line); i++ {
		if (line[i] == ' ' || line[i] == '\n') && cur != "" {
			tokens = append(tokens, Token{
				Filepath: filepath,
				Line:     number,
				Col:      i - len(cur) + 1,
				Word:     cur,
			})
			cur = ""
		}
		if line[i] != ' ' && line[i] != '\n' {
			cur += string(line[i])
		}
	}

	return tokens
}

func parseTokensAsOperations(tokens []Token) []operation.Operation {
	program := make([]operation.Operation, 0)

	assert.Assert(operation.Count == 18, "Exhaustive handling in lexer.parseTokensAsOperations()")

	for _, token := range tokens {
		switch token.Word {
		case "+":
			program = append(program, operation.Plus())
		case "-":
			program = append(program, operation.Minus())
		case "*":
			program = append(program, operation.Multiply())
		case "/":
			program = append(program, operation.Division())
		case "==":
			program = append(program, operation.Equal())
		case "!=":
			program = append(program, operation.NotEqual())
		case "<":
			program = append(program, operation.Less())
		case ">":
			program = append(program, operation.Greater())
		case "<=":
			program = append(program, operation.LessOrEqual())
		case ">=":
			program = append(program, operation.GreaterOrEqual())
		case "if":
			program = append(program, operation.If())
		case "else":
			program = append(program, operation.Else())
		case "end":
			program = append(program, operation.End())
		case "put":
			program = append(program, operation.Dump())
		case "copy":
			program = append(program, operation.Copy())
		case "swap":
			program = append(program, operation.Swap())
		case "drop":
			program = append(program, operation.Drop())
		default:
			val, err := strconv.Atoi(token.Word)
			if err != nil {
				log.Error(fmt.Sprintf("%s:%d:%d: can't parse token: %s", token.Filepath, token.Line, token.Col, token.Word))
				os.Exit(1)
			}
			program = append(program, operation.Push(val))
		}
	}
	return program
}
