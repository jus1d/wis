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
		line := strings.Split(lines[i], "//")[0] + "\n"
		tokens = append(tokens, lexLine(filepath, i+1, line)...)
	}

	program := parseTokensAsOperations(tokens)
	crossreferehcedProgram := crossreferenceBlocks(program)

	return crossreferehcedProgram
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

func crossreferenceBlocks(program []operation.Operation) []operation.Operation {
	stack := st.New()

	assert.Assert(operation.Count == 28, "Exhaustive handling in lexer.crossreferenceBlocks(). Not all operations should be handled in here.")

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

func parseTokensAsOperations(tokens []Token) []operation.Operation {
	program := make([]operation.Operation, 0)

	assert.Assert(operation.Count == 28, "Exhaustive handling in lexer.parseTokensAsOperations()")

	for _, token := range tokens {
		loc := fmt.Sprintf("%s:%d:%d", token.Filepath, token.Line, token.Col)

		switch token.Word {
		case "+":
			program = append(program, operation.Plus(loc))
		case "-":
			program = append(program, operation.Minus(loc))
		case "*":
			program = append(program, operation.Multiply(loc))
		case "/":
			program = append(program, operation.Division(loc))
		case "%":
			program = append(program, operation.Rem(loc))
		case "bor":
			program = append(program, operation.Bor(loc))
		case "band":
			program = append(program, operation.Band(loc))
		case "xor":
			program = append(program, operation.Xor(loc))
		case "shl":
			program = append(program, operation.Shl(loc))
		case "shr":
			program = append(program, operation.Shr(loc))
		case "==":
			program = append(program, operation.Equal(loc))
		case "!=":
			program = append(program, operation.NotEqual(loc))
		case "<":
			program = append(program, operation.Less(loc))
		case ">":
			program = append(program, operation.Greater(loc))
		case "<=":
			program = append(program, operation.LessOrEqual(loc))
		case ">=":
			program = append(program, operation.GreaterOrEqual(loc))
		case "if":
			program = append(program, operation.If(loc))
		case "else":
			program = append(program, operation.Else(loc))
		case "end":
			program = append(program, operation.End(loc))
		case "do":
			program = append(program, operation.Do(loc))
		case "while":
			program = append(program, operation.While(loc))
		case "put":
			program = append(program, operation.Put(loc))
		case "copy":
			program = append(program, operation.Copy(loc))
		case "2copy":
			program = append(program, operation.TwoCopy(loc))
		case "swap":
			program = append(program, operation.Swap(loc))
		case "drop":
			program = append(program, operation.Drop(loc))
		case "over":
			program = append(program, operation.Over(loc))
		default:
			val, err := strconv.Atoi(token.Word)
			if err != nil {
				log.Error(fmt.Sprintf("%s: can't parse token: %s", loc, token.Word))
				os.Exit(1)
			}
			program = append(program, operation.Push(val, loc))
		}
	}

	return program
}
