package compiler

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/log"
	"gollo/pkg/str"
	"os"
	"strconv"
)

func compile_x86_64(filepath string, program []operation.Operation) {
	fmt.Println(program)

	file, err := os.Create(filepath)
	if err != nil {
		log.Error("can't create an assembly file")
		os.Exit(1)
	}
	_ = file
	var content string

	strs := make([]string, 0)

	str.Complete(&content, "put:")
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

	assert.Assert(operation.Count == 33, "Exhaustive operations handling in compiler.compile_x86_64()")

	for i := 0; i < len(program); i++ {
		op := program[i]

		str.Complete(&content, fmt.Sprintf("    ; %s", op.Loc))

		switch op.Code {
		case operation.PUSH_INT:
			str.Complete(&content, fmt.Sprintf("    ; -- push int %d --", op.IntegerValue))
			str.Complete(&content, fmt.Sprintf("    push    %d", op.IntegerValue))
		case operation.PUSH_STRING:
			str.Complete(&content, fmt.Sprintf("    ; -- push str %s --", op.StringValue))
			str.Complete(&content, fmt.Sprintf("    mov     rax, %d", len(op.StringValue)))
			str.Complete(&content, "    push    rax")
			str.Complete(&content, fmt.Sprintf("    push    _str_%d", len(strs)))
			strs = append(strs, op.StringValue)
		case operation.PLUS:
			str.Complete(&content, "    ; -- plus --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    add     rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.MINUS:
			str.Complete(&content, "    ; -- minus --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    sub     rbx, rax")
			str.Complete(&content, "    push    rbx")
		case operation.MUL:
			str.Complete(&content, "    ; -- multiply --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    imul    rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.DIV:
			str.Complete(&content, "    ; -- division --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    xor     rdx, rdx")
			str.Complete(&content, "    div     rbx")
			str.Complete(&content, "    push    rax")
		case operation.MOD:
			str.Complete(&content, "    ; -- mod --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    xor     rdx, rdx")
			str.Complete(&content, "    div     rbx")
			str.Complete(&content, "    push    rdx")
		case operation.BOR:
			str.Complete(&content, "    ; -- binary or --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    or      rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.BAND:
			str.Complete(&content, "    ; -- binary and --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    and     rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.XOR:
			str.Complete(&content, "    ; -- xor --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    xor     rax, rbx")
			str.Complete(&content, "    push    rax")
		case operation.SHL:
			str.Complete(&content, "    ; -- shift left --")
			str.Complete(&content, "    pop     rcx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    shl     rax, cl")
			str.Complete(&content, "    push    rax")
		case operation.SHR:
			str.Complete(&content, "    ; -- shift right --")
			str.Complete(&content, "    pop     rcx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    shr     rax, cl")
			str.Complete(&content, "    push    rax")
		case operation.EQ:
			str.Complete(&content, "    ; -- equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    cmove   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.NE:
			str.Complete(&content, "    ; -- not equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    cmovne  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.LT:
			str.Complete(&content, "    ; -- less --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovl   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.GT:
			str.Complete(&content, "    ; -- greater --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovg   rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.LE:
			str.Complete(&content, "    ; -- less or equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovle  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.GE:
			str.Complete(&content, "    ; -- greater or equal --")
			str.Complete(&content, "    mov     rcx, 0")
			str.Complete(&content, "    mov     rdx, 1")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    cmp     rbx, rax")
			str.Complete(&content, "    cmovge  rcx, rdx")
			str.Complete(&content, "    push    rcx")
		case operation.IF:
			str.Complete(&content, "    ; -- if --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    mov     rbx, 0")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    je      _addr_"+strconv.Itoa(op.JumpTo))
		case operation.ELSE:
			str.Complete(&content, "    ; -- else --")
			str.Complete(&content, "    jmp     _addr_"+strconv.Itoa(op.JumpTo))
			str.Complete(&content, "    _addr_"+strconv.Itoa(i+1)+":")
		case operation.END:
			str.Complete(&content, "    ; -- end --")
			if program[op.JumpTo].Code == operation.WHILE {
				str.Complete(&content, "    jmp     _addr_"+strconv.Itoa(op.JumpTo))
			}
			str.Complete(&content, fmt.Sprintf("_addr_%d:", i))
		case operation.DO:
			str.Complete(&content, "    ; -- do --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    mov     rbx, 0")
			str.Complete(&content, "    cmp     rax, rbx")
			str.Complete(&content, "    je      _addr_"+strconv.Itoa(op.JumpTo-1))
		case operation.WHILE:
			str.Complete(&content, "    ; -- while --")
			str.Complete(&content, "    _addr_"+strconv.Itoa(i)+":")
		case operation.PUT:
			str.Complete(&content, "    ; -- put --")
			str.Complete(&content, "    pop     rdi")
			str.Complete(&content, "    call    put")
		case operation.COPY:
			str.Complete(&content, "    ; -- copy --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rax")
		case operation.TWO_COPY:
			str.Complete(&content, "    ; -- two copy --")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		case operation.SWAP:
			str.Complete(&content, "    ; -- swap --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		case operation.DROP:
			str.Complete(&content, "    ; -- drop --")
			str.Complete(&content, "    pop     rax")
		case operation.OVER:
			str.Complete(&content, "    ; -- over --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rbx")
			str.Complete(&content, "    push    rbx")
			str.Complete(&content, "    push    rax")
			str.Complete(&content, "    push    rbx")
		case operation.SYSCALL0:
			str.Complete(&content, "    ; -- syscall0 --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    syscall")
			str.Complete(&content, "    push    rax")
		case operation.SYSCALL1:
			str.Complete(&content, "    ; -- syscall1 --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rdi")
			str.Complete(&content, "    syscall")
			str.Complete(&content, "    push    rax")
		case operation.SYSCALL2:
			str.Complete(&content, "    ; -- syscall2 --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rdi")
			str.Complete(&content, "    pop     rsi")
			str.Complete(&content, "    syscall")
			str.Complete(&content, "    push    rax")
		case operation.SYSCALL3:
			str.Complete(&content, "    ; -- syscall3 --")
			str.Complete(&content, "    pop     rax")
			str.Complete(&content, "    pop     rdi")
			str.Complete(&content, "    pop     rsi")
			str.Complete(&content, "    pop     rdx")
			str.Complete(&content, "    syscall")
			str.Complete(&content, "    push    rax")
		default:
			assert.Assert(false, "unreachable")
		}
	}

	str.Complete(&content, "    ; -- exit --")
	str.Complete(&content, "    mov     rax, 60")
	str.Complete(&content, "    mov     rdi, 0")
	str.Complete(&content, "    syscall")

	str.Complete(&content, "section .data")
	for i, s := range strs {
		str.Complete(&content, fmt.Sprintf("    _str_%d: db \"%s\", 10", i, s))
	}

	_, err = file.WriteString(content)
	if err != nil {
		log.Error("can't write assembly file")
		os.Exit(1)
	}
}
