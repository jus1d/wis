package runner

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	st "gollo/pkg/stack"
	"syscall"
)

func Run(program []operation.Operation) {
	assert.Assert(operation.Count == 32, "Exhaustive handling in runner.Run()")

	stack := st.New()

	i := 0
	for i < len(program) {
		op := program[i]

		switch op.Code {
		case operation.PUSH:
			stack.Push(op.Value)
			i++
		case operation.PLUS:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a + b)
			i++
		case operation.MINUS:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b - a)
			i++
		case operation.MUL:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b * a)
			i++
		case operation.DIV:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b / a)
			i++
		case operation.REM:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b % a)
			i++
		case operation.BOR:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a | b)
			i++
		case operation.BAND:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a & b)
			i++
		case operation.XOR:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a ^ b)
			i++
		case operation.SHL:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b << a)
			i++
		case operation.SHR:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b >> a)
			i++
		case operation.EQ:
			a := stack.Pop()
			b := stack.Pop()
			if a == b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.NE:
			a := stack.Pop()
			b := stack.Pop()
			if a != b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.LT:
			a := stack.Pop()
			b := stack.Pop()
			if b < a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.GT:
			a := stack.Pop()
			b := stack.Pop()
			if b > a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.LE:
			a := stack.Pop()
			b := stack.Pop()
			if b <= a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.GE:
			a := stack.Pop()
			b := stack.Pop()
			if b >= a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.IF:
			a := stack.Pop()
			if a == 0 {
				i = op.JumpTo
			} else {
				i++
			}
		case operation.ELSE:
			i = op.JumpTo
		case operation.END:
			if program[op.JumpTo].Code == operation.WHILE {
				i = op.JumpTo
			} else {
				i++
			}
		case operation.DO:
			a := stack.Pop()
			if a == 0 {
				i = op.JumpTo
			} else {
				i++
			}
		case operation.WHILE:
			i++
		case operation.PUT:
			fmt.Println(stack.Pop())
			i++
		case operation.COPY:
			stack.Push(stack.Peek())
			i++
		case operation.TWOCOPY:
			b := stack.Pop()
			a := stack.Pop()
			stack.Push(a)
			stack.Push(b)
			stack.Push(a)
			stack.Push(b)
			i++
		case operation.SWAP:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a)
			stack.Push(b)
			i++
		case operation.DROP:
			_ = stack.Pop()
			i++
		case operation.OVER:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b)
			stack.Push(a)
			stack.Push(b)
			i++
		case operation.SYSCALL0:
			syscallNumber := stack.Pop()
			r, _, _ := syscall.Syscall(uintptr(syscallNumber), 0, 0, 0)
			stack.Push(int(r))
			i++
		case operation.SYSCALL1:
			syscallNumber := stack.Pop()
			arg1 := stack.Pop()
			r, _, _ := syscall.Syscall(uintptr(syscallNumber), uintptr(arg1), 0, 0)
			stack.Push(int(r))
			i++
		case operation.SYSCALL2:
			syscallNumber := stack.Pop()
			arg1 := stack.Pop()
			arg2 := stack.Pop()
			r, _, _ := syscall.Syscall(uintptr(syscallNumber), uintptr(arg1), uintptr(arg2), 0)
			stack.Push(int(r))
			i++
		case operation.SYSCALL3:
			syscallNumber := stack.Pop()
			arg1 := stack.Pop()
			arg2 := stack.Pop()
			arg3 := stack.Pop()
			r, _, _ := syscall.Syscall(uintptr(syscallNumber), uintptr(arg1), uintptr(arg2), uintptr(arg3))
			stack.Push(int(r))
			i++
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
