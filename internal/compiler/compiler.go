package compiler

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/command"
	"gollo/pkg/log"
	"gollo/pkg/str"
	"os"
	"runtime"
)

func Compile(name string, program []operation.Operation) {
	switch runtime.GOARCH {
	case "amd64":
		compile_x86_64(fmt.Sprintf("%s.asm", name), program)
		command.Execute(false, "nasm", "-felf64", "-o", fmt.Sprintf("%s.o", name), fmt.Sprintf("%s.asm", name))
		command.Execute(false, "ld", "-o", name, fmt.Sprintf("%s.o", name))
		command.Execute(false, "rm", fmt.Sprintf("%s.o", name))
		log.Info("compiled to " + name)
	default:
		assert.Assert(false, "unsupported platform: "+runtime.GOARCH)
	}
}

func compile_x86_64(filepath string, program []operation.Operation) {
	log.Info("generating assembly")
	file, err := os.Create(filepath)
	if err != nil {
		log.Error("can't create an assembly file")
		os.Exit(1)
	}
	_ = file
	var content string

	str.Complete(&content, "dump:")
	str.Complete(&content, "    mov     r9, -3689348814741910323")
	str.Complete(&content, "    sub     rsp, 40")
	str.Complete(&content, "    mov     BYTE [rsp+31], 10")
	str.Complete(&content, "    lea     rcx, [rsp+30]")
	str.Complete(&content, ".L2:")
	str.Complete(&content, "    mov     rax, rdi")
	str.Complete(&content, "    lea     r8, [rsp+32]")
	str.Complete(&content, "    mul     r9")
	str.Complete(&content, "    mov     rax, rdi")
	str.Complete(&content, "    sub     r8, rcx")
	str.Complete(&content, "    shr     rdx, 3")
	str.Complete(&content, "    lea     rsi, [rdx+rdx*4]")
	str.Complete(&content, "    add     rsi, rsi")
	str.Complete(&content, "    sub     rax, rsi")
	str.Complete(&content, "    add     eax, 48")
	str.Complete(&content, "    mov     BYTE [rcx], al")
	str.Complete(&content, "    mov     rax, rdi")
	str.Complete(&content, "    mov     rdi, rdx")
	str.Complete(&content, "    mov     rdx, rcx")
	str.Complete(&content, "    sub     rcx, 1")
	str.Complete(&content, "    cmp     rax, 9")
	str.Complete(&content, "    ja      .L2")
	str.Complete(&content, "    lea     rax, [rsp+32]")
	str.Complete(&content, "    mov     edi, 1")
	str.Complete(&content, "    sub     rdx, rax")
	str.Complete(&content, "    xor     eax, eax")
	str.Complete(&content, "    lea     rsi, [rsp+32+rdx]")
	str.Complete(&content, "    mov     rdx, r8")
	str.Complete(&content, "    mov     rax, 1")
	str.Complete(&content, "    syscall")
	str.Complete(&content, "    add     rsp, 40")
	str.Complete(&content, "    ret")
	str.Complete(&content, "\n")
	str.Complete(&content, "section .text")
	str.Complete(&content, "    global _start")
	str.Complete(&content, "_start:")

	assert.Assert(operation.OpCount == 13, "Exhaustive handling in compile_x86_64()")

	for _, op := range program {
		switch op.Code {
		case operation.OpPush:
			str.Complete(&content, fmt.Sprintf("    ; -- Push %d --", op.Value))
			str.Complete(&content, fmt.Sprintf("    push %d", op.Value))
		case operation.OpPlus:
			str.Complete(&content, "    ; -- Plus --")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    add rax, rbx")
			str.Complete(&content, "    push rax")
		case operation.OpMinus:
			str.Complete(&content, "    ; -- Minus --")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    sub rbx, rax")
			str.Complete(&content, "    push rbx")
		case operation.OpMultiply:
			str.Complete(&content, "    ; -- Multiply --")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    imul rax, rbx")
			str.Complete(&content, "    push rax")
		case operation.OpDivision:
			str.Complete(&content, "    ; -- Division --")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    xor rdx, rdx")
			str.Complete(&content, "    div rbx")
			str.Complete(&content, "    push rax")
		case operation.OpEqual:
			str.Complete(&content, "    ; -- Equal --")
			str.Complete(&content, "    mov rcx, 0")
			str.Complete(&content, "    mov rdx, 1")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    cmp rax, rbx")
			str.Complete(&content, "    cmove rcx, rdx")
			str.Complete(&content, "    push rcx")
		case operation.OpLess:
			str.Complete(&content, "    ; -- Less --")
			str.Complete(&content, "    mov rcx, 0")
			str.Complete(&content, "    mov rdx, 1")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    cmp rbx, rax")
			str.Complete(&content, "    cmovl rcx, rdx")
			str.Complete(&content, "    push rcx")
		case operation.OpGreater:
			str.Complete(&content, "    ; -- Greater --")
			str.Complete(&content, "    mov rcx, 0")
			str.Complete(&content, "    mov rdx, 1")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    cmp rbx, rax")
			str.Complete(&content, "    cmovg rcx, rdx")
			str.Complete(&content, "    push rcx")
		case operation.OpLessOrEqual:
			str.Complete(&content, "    ; -- Less --")
			str.Complete(&content, "    mov rcx, 0")
			str.Complete(&content, "    mov rdx, 1")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    cmp rbx, rax")
			str.Complete(&content, "    cmovle rcx, rdx")
			str.Complete(&content, "    push rcx")
		case operation.OpGreaterOrEqual:
			str.Complete(&content, "    ; -- Greater --")
			str.Complete(&content, "    mov rcx, 0")
			str.Complete(&content, "    mov rdx, 1")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    cmp rbx, rax")
			str.Complete(&content, "    cmovge rcx, rdx")
			str.Complete(&content, "    push rcx")
		case operation.OpDump:
			str.Complete(&content, "    ; -- Dump --")
			str.Complete(&content, "    pop rdi")
			str.Complete(&content, "    call dump")
		case operation.OpCopy:
			str.Complete(&content, "    ; -- Copy --")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    push rax")
			str.Complete(&content, "    push rax")
		case operation.OpSwap:
			str.Complete(&content, "    ; -- Swap --")
			str.Complete(&content, "    pop rax")
			str.Complete(&content, "    pop rbx")
			str.Complete(&content, "    push rax")
			str.Complete(&content, "    push rbx")
		default:
			assert.Assert(false, "unreachable")
		}
	}

	str.Complete(&content, "    ; -- Exit --")
	str.Complete(&content, "    mov rax, 60")
	str.Complete(&content, "    mov rdi, 0")
	str.Complete(&content, "    syscall")

	_, err = file.WriteString(content)
	if err != nil {
		log.Error("can't write assembly file")
		os.Exit(1)
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
