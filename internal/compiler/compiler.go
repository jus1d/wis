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
		command.MustExecuteEchoed(false, "nasm", "-felf64", "-o", fmt.Sprintf("%s.o", name), fmt.Sprintf("%s.asm", name))
		command.MustExecuteEchoed(false, "ld", "-o", name, fmt.Sprintf("%s.o", name))
		command.Execute(false, "rm", fmt.Sprintf("%s.o", name))
		log.Info("Compiled to " + name)
	default:
		assert.Assert(false, "unsupported platform: "+runtime.GOARCH)
	}
}

func compile_x86_64(filepath string, program []operation.Operation) {
	log.Info("Generating assembly")
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
	str.Complete(&content, "")
	str.Complete(&content, "section .text")
	str.Complete(&content, "    global _start")
	str.Complete(&content, "_start:")

	assert.Assert(operation.Count == 23, "Exhaustive handling in compiler.compile_x86_64()")

	for i := 0; i < len(program); i++ {
		op := program[i]

		switch op.Code {
		case operation.PUSH:
			str.Complete(&content, fmt.Sprintf("    ; -- Push %d --", op.Value))
			str.Complete(&content, fmt.Sprintf("    push    %d", op.Value))
		case operation.PLUS:
			str.Complete(&content, "    ; -- Plus --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    add     rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.MINUS:
			str.Complete(&content, "    ; -- Minus --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    sub     rbx, rax")
			str.Complete(&content, "    push    rbx")
		case operation.MUL:
			str.Complete(&content, "    ; -- Multiply --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    imul    rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.DIV:
			str.Complete(&content, "    ; -- Division --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    xor     rdx, rdx")
			str.Complete(&content, "    div     rbx")
			str.Complete(&content, "    push    rax")
		case operation.REM:
			str.Complete(&content, "    ; -- Remainder of division --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    xor     rdx, rdx")
			str.Complete(&content, "    div     rbx")
			str.Complete(&content, "    push    rdx")
		case operation.EQ:
			str.Complete(&content, "    ; -- Equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    cmove   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.NE:
			str.Complete(&content, "    ; -- Not Equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    cmovne  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.LT:
			str.Complete(&content, "    ; -- Less --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovl   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.GT:
			str.Complete(&content, "    ; -- Greater --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovg   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.LE:
			str.Complete(&content, "    ; -- Less or equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovle  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.GE:
			str.Complete(&content, "    ; -- Greater or equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovge  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.IF:
			str.Complete(&content, "    ; -- If --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    mov     rbx, 0")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, fmt.Sprintf("    je      _addr_%d", op.JumpTo))
		case operation.ELSE:
			str.Complete(&content, "    ; -- Else --")
			str.Complete(&content, fmt.Sprintf("    jmp     _addr_%d", op.JumpTo))
			str.Complete(&content, fmt.Sprintf("_addr_%d:", i+1))
		case operation.END:
			str.Complete(&content, "    ; -- End --")
			if program[op.JumpTo].Code == operation.WHILE {
				str.Complete(&content, fmt.Sprintf("    jmp     _addr_%d", op.JumpTo))
			}
			str.Complete(&content, fmt.Sprintf("_addr_%d:", i))
		case operation.DO:
			str.Complete(&content, "    ; -- Do --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    mov     rbx, 0")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    je      "+fmt.Sprintf("_addr_%d", op.JumpTo-1))
		case operation.WHILE:
			str.Complete(&content, "    ; -- While --")
			str.Complete(&content, fmt.Sprintf("_addr_%d:", i))
		case operation.DUMP:
			str.Complete(&content, "    ; -- Dump --")
			str.Complete(&content, "    pop     rdi")
			str.Complete(&content, "    call    dump")
		case operation.COPY:
			str.Complete(&content, "    ; -- Copy --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rax")
		case operation.TWOCOPY:
			str.Complete(&content, "    ; -- Two Copy --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		case operation.SWAP:
			str.Complete(&content, "    ; -- Swap --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		case operation.DROP:
			str.Complete(&content, "    ; -- Drop --")
			str.Complete(&content, "    pop     rax")
		case operation.OVER:
			str.Complete(&content, "    ; -- Over --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    push    rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		default:
			assert.Assert(false, "unreachable")
		}
	}

	str.Complete(&content, "    ; -- Exit --")
	str.Complete(&content, "    mov     rax, 60")
	str.Complete(&content, "    mov     rdi, 0")
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
