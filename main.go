package main

import (
	"fmt"
	"gollo/stack"
	"os"
	"os/exec"
	"strconv"
	"strings"
)

const (
	OperationPush = iota
	OperationPlus
	OperationMinus
	OperationDump
)

type Operation struct {
	Code  int
	Value int64
}

func push(value int64) Operation {
	return Operation{
		Code:  OperationPush,
		Value: value,
	}
}

func plus() Operation {
	return Operation{
		Code: OperationPlus,
	}
}

func minus() Operation {
	return Operation{
		Code: OperationMinus,
	}
}

func dump() Operation {
	return Operation{
		Code: OperationDump,
	}
}

func assert(cond bool, message string) {
	if !cond {
		panic("assertion error: " + message)
	}
}

func chop(arr []string) (string, []string) {
	if len(arr) == 0 {
		panic("ERROR: can't chop")
	}
	return arr[0], arr[1:]
}

func usage() {
	fmt.Println("Usage: gollo <SUBCOMMAND> ./examples/foo.gll")
	fmt.Println("SUBCOMMANDS:")
	fmt.Println("    run       Instantly run program")
	fmt.Println("    compile   Compile program into an object code")
}

func execute(cmd ...string) {
	sv := strings.Join(cmd, " ")
	fmt.Printf("[CMD]: %s\n", sv)
	command := exec.Command(cmd[0], cmd[1:]...)
	err := command.Run()
	if err != nil {
		fmt.Printf("ERROR: can't execute command: %s\n", err.Error())
		os.Exit(1)
	}
}

func info(message string) {
	fmt.Printf("[INFO]: %s\n", message)
}

func loadProgramFromFile(filepath string) []Operation {
	_, err := os.Stat(filepath)
	if err != nil {
		usage()
		fmt.Printf("ERROR: can't found a file: '%s'\n", filepath)
		os.Exit(1)
	}

	byteContent, err := os.ReadFile(filepath)
	if err != nil {
		usage()
		fmt.Printf("ERROR: can't read provided file: %s", err.Error())
		os.Exit(1)
	}

	source := string(byteContent)

	words := strings.FieldsFunc(source, func(r rune) bool {
		return r == ' ' || r == '\n'
	})

	program := make([]Operation, 0)

	for _, word := range words {
		if word == "+" {
			program = append(program, plus())
		} else if word == "-" {
			program = append(program, minus())
		} else if word == "put" {
			program = append(program, dump())
		} else {
			val, err := strconv.ParseInt(word, 10, 64)
			if err != nil {
				fmt.Printf("ERROR: can't parse token as integer: %s", word)
			}
			program = append(program, push(val))
		}
	}

	return program
}

func run(program []Operation) {
	st := stack.New()
	for _, op := range program {
		switch op.Code {
		case OperationPush:
			st.Push(op.Value)
		case OperationPlus:
			a := st.Pop()
			b := st.Pop()
			st.Push(a + b)
		case OperationMinus:
			a := st.Pop()
			b := st.Pop()
			st.Push(b - a)
		case OperationDump:
			fmt.Println(st.Pop())
		default:
			assert(false, "unreachable")
		}
	}
}

func compileArm64(program []Operation) {
	info("generating assembly")
	file, err := os.Create("output.s")
	if err != nil {
		fmt.Printf("ERROR: can't create an assembly file\n")
		os.Exit(1)
	}
	_ = file
	var content string
	content += ".text\n"
	content += ".global _main\n"
	content += "\n"
	content += "_main:\n"
	for _, op := range program {
		switch op.Code {
		case OperationPush:
			content += fmt.Sprintf("    // -- Push %d --\n", op.Value)
			content += fmt.Sprintf("    str %d, [sp, #-16]!\n", op.Value)
		}
	}
	content += "    // -- Exit -- \n"
	content += "    mov x0, #0\n"
	content += "    mov x8, #93\n"
	content += "    svc #0\n"

	_, err = file.WriteString(content)
	if err != nil {
		fmt.Printf("ERROR: can't write assembly file\n")
		os.Exit(1)
	}
}

func main() {
	args := os.Args
	_, args = chop(args)

	if len(args) < 1 {
		usage()
		fmt.Printf("ERROR: no subcommand provided\n")
		os.Exit(1)
	}
	subcommand, args := chop(args)

	if len(args) < 1 {
		usage()
		fmt.Printf("ERROR: no filepath provided\n")
		os.Exit(1)
	}
	switch subcommand {
	case "run":
		filepath, _ := chop(args)
		program := loadProgramFromFile(filepath)
		run(program)
	case "compile":
		filepath, _ := chop(args)
		program := loadProgramFromFile(filepath)
		compileArm64(program)
		execute("as", "-o", "output.o", "output.s")
		execute("ld", "-o", "output", "output.o")
		execute("rm", "output.o")
		info("compiled to ./output")
	default:
		usage()
		fmt.Printf("ERROR: unknown subcommand provided\n")
	}
}
