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
	"unicode"
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
		tokens = append(tokens, LexLine(filepath, i+1, line)...)
	}

	program := parseTokensAsOperations(tokens)
	crossreferencedProgram := crossreferenceBlocks(program)

	return crossreferencedProgram
}

func LexLine(filepath string, number int, line string) []token.Token {
	tokens := make([]token.Token, 0)
	col := findCol(line, 0, func(x rune) bool { return !unicode.IsSpace(x) })

	for col < len(line) {
		var colEnd int
		if line[col] == '"' {
			colEnd = findCol(line, col+1, func(x rune) bool { return x == '"' })

			// TODO: report unclosed string literals as proper compiler errors instead of assertions
			if line[colEnd] == '"' {
				textOfToken := line[col+1 : colEnd]
				tokens = append(tokens, token.String(textOfToken, fmt.Sprintf("%s:%d:%d", filepath, number, col+1)))
				col = findCol(line, colEnd+1, func(x rune) bool { return !unicode.IsSpace(x) })
			}
		} else {
			colEnd = findCol(line, col, unicode.IsSpace)
			textOfToken := line[col:colEnd]
			if intValue, err := strconv.Atoi(textOfToken); err == nil {
				tokens = append(tokens, token.Int(intValue, fmt.Sprintf("%s:%d:%d", filepath, number, col+1)))
			} else {
				tokens = append(tokens, token.Word(textOfToken, fmt.Sprintf("%s:%d:%d", filepath, number, col+1)))
			}
			col = findCol(line, colEnd, func(x rune) bool { return !unicode.IsSpace(x) })
		}
	}

	return tokens
}

// findCol finds the column index of the next character satisfying the predicate.
func findCol(line string, start int, predicate func(rune) bool) int {
	for i, char := range line[start:] {
		if predicate(char) {
			return start + i
		}
	}
	return len(line)
}

func crossreferenceBlocks(program []operation.Operation) []operation.Operation {
	stack := st.New()

	assert.Assert(operation.Count == 34, "Exhaustive operations handling in lexer.crossreferenceBlocks(). Not all operations should be handled in here.")

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

	assert.Assert(operation.Count == 34, "Exhaustive operations handling in lexer.parseTokensAsOperations()")
	assert.Assert(token.Count == 3, "Exhaustive tokens handling in lexer.parseTokensAsOperations()")

	for _, tok := range tokens {
		switch tok.Code {
		case token.INT:
			program = append(program, operation.PushInt(tok.IntegerValue, tok.Loc))
		case token.STRING:
			program = append(program, operation.PushString(tok.StringValue, tok.Loc))
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
			case "puts":
				program = append(program, operation.Puts(tok.Loc))
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
