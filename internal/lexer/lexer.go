package lexer

import (
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/log"
	"os"
	"strconv"
	"strings"
)

func LoadProgramFromFile(filepath string) []operation.Operation {
	_, err := os.Stat(filepath)
	if err != nil {
		log.Usage()
		log.Error("can't found a file: " + filepath)
		os.Exit(1)
	}

	byteContent, err := os.ReadFile(filepath)
	if err != nil {
		log.Usage()
		log.Error("can't read provided file: " + err.Error())
		os.Exit(1)
	}

	source := string(byteContent)

	words := strings.FieldsFunc(source, func(r rune) bool {
		return r == ' ' || r == '\n'
	})

	program := make([]operation.Operation, 0)

	assert.Assert(operation.OpCount == 10, "Exhaustive handling in loadProgramFromFile()")

	for _, word := range words {
		switch word {
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
		case "<":
			program = append(program, operation.Less())
		case ">":
			program = append(program, operation.Greater())
		case "put":
			program = append(program, operation.Dump())
		case "copy":
			program = append(program, operation.Copy())
		default:
			val, err := strconv.ParseInt(word, 10, 64)
			if err != nil {
				log.Error("can't parse token as integer: " + word)
				os.Exit(1)
			}
			program = append(program, operation.Push(val))
		}
	}

	return program
}
