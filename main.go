package main

import (
	"fmt"
	stck "gollo/stack"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
)

const (
	OperationPush = iota
	OperationPlus
	OperationMinus
	OperationMultiply
	OperationDivision
	OperationEqual
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

func multiply() Operation {
	return Operation{
		Code: OperationMultiply,
	}
}

func division() Operation {
	return Operation{
		Code: OperationDivision,
	}
}

func equal() Operation {
	return Operation{
		Code: OperationEqual,
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

func logInfo(message string) {
	fmt.Printf("INFO: %s\n", message)
}

func logError(message string) {
	fmt.Printf("ERROR: %s\n", message)
}

func usage() {
	fmt.Println("Usage: gollo <SUBCOMMAND> ./examples/foo.glo")
	fmt.Println("SUBCOMMANDS:")
	fmt.Println("    run       Instantly run program")
	fmt.Println("    compile   Compile program into an object code")
}

func execute(logOutput bool, cmd ...string) {
	sv := strings.Join(cmd, " ")
	fmt.Printf("[CMD]: %s\n", sv)
	command := exec.Command(cmd[0], cmd[1:]...)
	output, err := command.Output()
	if err != nil {
		logError("can't execute command: " + err.Error())
		os.Exit(1)
	}

	if logOutput {
		fmt.Printf("%s", string(output))
	}
}

func loadProgramFromFile(filepath string) []Operation {
	_, err := os.Stat(filepath)
	if err != nil {
		usage()
		logError("can't found a file: " + filepath)
		os.Exit(1)
	}

	byteContent, err := os.ReadFile(filepath)
	if err != nil {
		usage()
		logError("can't read provided file: " + err.Error())
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
		} else if word == "*" {
			program = append(program, multiply())
		} else if word == "/" {
			program = append(program, division())
		} else if word == "==" {
			program = append(program, equal())
		} else if word == "put" {
			program = append(program, dump())
		} else {
			val, err := strconv.ParseInt(word, 10, 64)
			if err != nil {
				logError("can't parse token as integer: " + word)
				os.Exit(1)
			}
			program = append(program, push(val))
		}
	}

	return program
}

func run(program []Operation) {
	stack := stck.New()
	for _, op := range program {
		switch op.Code {
		case OperationPush:
			stack.Push(op.Value)
		case OperationPlus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a + b)
		case OperationMinus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b - a)
		case OperationMultiply:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b * a)
		case OperationDivision:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b / a)
		case OperationEqual:
			a := stack.Pop()
			b := stack.Pop()
			if a == b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
		case OperationDump:
			fmt.Println(stack.Pop())
		default:
			assert(false, "unreachable")
		}
	}
}

// func compile_arm64(program []Operation) {
// 	info("generating assembly")
// 	file, err := os.Create("output.s")
// 	if err != nil {
// 		fmt.Printf("ERROR: can't create an assembly file\n")
// 		os.Exit(1)
// 	}
// 	_ = file
// 	var content string
// 	content += ".text\n"
// 	content += ".global _main\n"
// 	content += "\n"
// 	content += "_main:\n"
// 	for _, op := range program {
// 		switch op.Code {
// 		case OperationPush:
// 			content += fmt.Sprintf("    // -- Push %d --\n", op.Value)
// 			content += fmt.Sprintf("    str %d, [sp, #-16]!\n", op.Value)
// 		}
// 	}
// 	content += "    // -- Exit -- \n"
// 	content += "    mov x0, #0\n"
// 	content += "    mov x8, #93\n"
// 	content += "    svc #0\n"
// 	_, err = file.WriteString(content)
// 	if err != nil {
// 		fmt.Printf("ERROR: can't write assembly file\n")
// 		os.Exit(1)
// 	}
// }

func compile_x86_64(filepath string, program []Operation) {
	logInfo("generating assembly")
	file, err := os.Create(filepath)
	if err != nil {
		logError("can't create an assembly file")
		os.Exit(1)
	}
	_ = file
	var content string

	completeString(&content, "dump:")
	completeString(&content, "    mov     r9, -3689348814741910323")
	completeString(&content, "    sub     rsp, 40")
	completeString(&content, "    mov     BYTE [rsp+31], 10")
	completeString(&content, "    lea     rcx, [rsp+30]")
	completeString(&content, ".L2:")
	completeString(&content, "    mov     rax, rdi")
	completeString(&content, "    lea     r8, [rsp+32]")
	completeString(&content, "    mul     r9")
	completeString(&content, "    mov     rax, rdi")
	completeString(&content, "    sub     r8, rcx")
	completeString(&content, "    shr     rdx, 3")
	completeString(&content, "    lea     rsi, [rdx+rdx*4]")
	completeString(&content, "    add     rsi, rsi")
	completeString(&content, "    sub     rax, rsi")
	completeString(&content, "    add     eax, 48")
	completeString(&content, "    mov     BYTE [rcx], al")
	completeString(&content, "    mov     rax, rdi")
	completeString(&content, "    mov     rdi, rdx")
	completeString(&content, "    mov     rdx, rcx")
	completeString(&content, "    sub     rcx, 1")
	completeString(&content, "    cmp     rax, 9")
	completeString(&content, "    ja      .L2")
	completeString(&content, "    lea     rax, [rsp+32]")
	completeString(&content, "    mov     edi, 1")
	completeString(&content, "    sub     rdx, rax")
	completeString(&content, "    xor     eax, eax")
	completeString(&content, "    lea     rsi, [rsp+32+rdx]")
	completeString(&content, "    mov     rdx, r8")
	completeString(&content, "    mov     rax, 1")
	completeString(&content, "    syscall")
	completeString(&content, "    add     rsp, 40")
	completeString(&content, "    ret")
	completeString(&content, "\n")
	completeString(&content, "section .text")
	completeString(&content, "    global _start")
	completeString(&content, "_start:")

	for _, op := range program {
		switch op.Code {
		case OperationPush:
			completeString(&content, fmt.Sprintf("    ; -- Push %d --", op.Value))
			completeString(&content, fmt.Sprintf("    push %d", op.Value))
		case OperationPlus:
			completeString(&content, "    ; -- Plus --")
			completeString(&content, "    pop rax")
			completeString(&content, "    pop rbx")
			completeString(&content, "    add rax, rbx")
			completeString(&content, "    push rax")
		case OperationMinus:
			completeString(&content, "    ; -- Minus --")
			completeString(&content, "    pop rax")
			completeString(&content, "    pop rbx")
			completeString(&content, "    sub rbx, rax")
			completeString(&content, "    push rbx")
		case OperationDump:
			completeString(&content, "    ; -- Dump --")
			completeString(&content, "    pop rdi")
			completeString(&content, "    call dump")
		}
	}

	completeString(&content, "    ; -- Exit --")
	completeString(&content, "    mov rax, 60")
	completeString(&content, "    mov rdi, 0")
	completeString(&content, "    syscall")

	_, err = file.WriteString(content)
	if err != nil {
		logError("can't write assembly file")
		os.Exit(1)
	}
}

func getNameFromPath(path string) string {
	filename := filepath.Base(path)
	extension := filepath.Ext(filename)
	return strings.TrimSuffix(path, extension)
}

func completeString(s *string, content string) {
	*s += content + "\n"
}

func main() {
	args := os.Args
	_, args = chop(args)

	if len(args) < 1 {
		usage()
		logError("no subcommand provided")
		os.Exit(1)
	}
	subcommand, args := chop(args)

	if len(args) < 1 {
		usage()
		logError("no filepath provided")
		os.Exit(1)
	}
	switch subcommand {
	case "run":
		path, _ := chop(args)
		program := loadProgramFromFile(path)
		run(program)
	case "compile":
		runAfterCompile := false

		path, args := chop(args)
		if path == "-r" {
			runAfterCompile = true
			path, _ = chop(args)
		}

		program := loadProgramFromFile(path)
		name := getNameFromPath(path)

		switch runtime.GOARCH {
		case "amd64":
			compile_x86_64(fmt.Sprintf("%s.asm", name), program)
			execute(false, "nasm", "-felf64", "-o", fmt.Sprintf("%s.o", name), fmt.Sprintf("%s.asm", name))
			execute(false, "ld", "-o", name, fmt.Sprintf("%s.o", name))
			execute(false, "rm", fmt.Sprintf("%s.o", name))
			logInfo("compiled to " + name)
		default:
			logError("unsupported platform: " + runtime.GOARCH)
		}

		if runAfterCompile {
			execute(true, name)
		}
	default:
		usage()
		logError("unknown subcommand provided")
	}
}
