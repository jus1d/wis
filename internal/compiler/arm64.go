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

func compile_arm64(filepath string, program []operation.Operation) {
	file, err := os.Create(filepath)
	if err != nil {
		log.Error("can't create an assembly file")
		os.Exit(1)
	}
	_ = file
	var content string

	str.Complete(&content, ".global _main")
	str.Complete(&content, "")
	str.Complete(&content, ".text")
	str.Complete(&content, "_main:")

	assert.Assert(operation.Count == 32, "Exhaustive operations handling in compiler.compile_arm64()")

	for i := 0; i < len(program); i++ {
		op := program[i]

		str.Complete(&content, fmt.Sprintf("    // %s", op.Loc))

		switch op.Code {
		case operation.PUSH_INT:
			str.Complete(&content, fmt.Sprintf("    // -- push int %d --", op.IntegerValue))
			str.Complete(&content, "    mov     x0, #"+strconv.Itoa(op.IntegerValue))
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.PLUS:
			str.Complete(&content, "    // -- plus --")
			str.Complete(&content, "    ldp     x0, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    add     x0, x0, x1")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.MINUS:
			str.Complete(&content, "    // -- minus --")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x0, xzr, [sp], #16")
			str.Complete(&content, "    sub     x0, x0, x1")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.MUL:
			str.Complete(&content, "    // -- multiply --")
			str.Complete(&content, "    ldp     x0, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    mul     x0, x0, x1")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.DIV:
			assert.Assert(false, "not implemented yet")
		case operation.MOD:
			assert.Assert(false, "not implemented yet")
		case operation.BOR:
			str.Complete(&content, "    // -- binary or --")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x2, xzr, [sp], #16")
			str.Complete(&content, "    orr     x0, x1, x2")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.BAND:
			str.Complete(&content, "    // -- binary and --")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x2, xzr, [sp], #16")
			str.Complete(&content, "    and     x0, x1, x2")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.XOR:
			str.Complete(&content, "    // -- xor --")
			str.Complete(&content, "    ldp     x1, xzr, [sp], #16")
			str.Complete(&content, "    ldp     x2, xzr, [sp], #16")
			str.Complete(&content, "    eor     x0, x1, x2")
			str.Complete(&content, "    stp     x0, xzr, [sp, #-16]!")
		case operation.SHL:
			assert.Assert(false, "not implemented yet")
		case operation.SHR:
			assert.Assert(false, "not implemented yet")
		case operation.EQ:
			assert.Assert(false, "not implemented yet")
		case operation.NE:
			assert.Assert(false, "not implemented yet")
		case operation.LT:
			assert.Assert(false, "not implemented yet")
		case operation.GT:
			assert.Assert(false, "not implemented yet")
		case operation.LE:
			assert.Assert(false, "not implemented yet")
		case operation.GE:
			assert.Assert(false, "not implemented yet")
		case operation.IF:
			assert.Assert(false, "not implemented yet")
		case operation.ELSE:
			assert.Assert(false, "not implemented yet")
		case operation.END:
			assert.Assert(false, "not implemented yet")
		case operation.DO:
			assert.Assert(false, "not implemented yet")
		case operation.WHILE:
			assert.Assert(false, "not implemented yet")
		case operation.PUT:
			assert.Assert(false, "not implemented yet")
		case operation.COPY:
			assert.Assert(false, "not implemented yet")
		case operation.TWO_COPY:
			assert.Assert(false, "not implemented yet")
		case operation.SWAP:
			assert.Assert(false, "not implemented yet")
		case operation.DROP:
			assert.Assert(false, "not implemented yet")
		case operation.OVER:
			assert.Assert(false, "not implemented yet")
		case operation.SYSCALL0:
			assert.Assert(false, "not implemented yet")
		case operation.SYSCALL1:
			assert.Assert(false, "not implemented yet")
		case operation.SYSCALL2:
			assert.Assert(false, "not implemented yet")
		case operation.SYSCALL3:
			assert.Assert(false, "not implemented yet")
		default:
			assert.Assert(false, "unreachable")
		}
	}

	str.Complete(&content, "    // -- exit --")
	str.Complete(&content, "    mov    x0, #0")
	str.Complete(&content, "    mov    x16, #1")
	str.Complete(&content, "    svc    #0")

	_, err = file.WriteString(content)
	if err != nil {
		log.Error("can't write assembly file")
		os.Exit(1)
	}
}
