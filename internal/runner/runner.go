package runner

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/log"
	st "gollo/pkg/stack"
	"os"
	"syscall"
)

func Run(program []operation.Operation) {
	assert.Assert(operation.Count == 33, "Exhaustive operations handling in runner.Run()")

	memory := make([]byte, 0, 640000)
	strintsSize := 0

	stack := st.New()

	i := 0
	for i < len(program) {
		op := program[i]

		switch op.Code {
		case operation.PUSH_INT:
			stack.Push(op.IntegerValue)
			i++
		case operation.PUSH_STRING:
			bs := []byte(op.StringValue)
			n := len(bs)
			stack.Push(n)
			op.Address = strintsSize
			memory = append(memory, bs...)
			strintsSize += n
			stack.Push(op.Address)
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
		case operation.MOD:
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
		case operation.TWO_COPY:
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
			if syscallNumber == 1 {
				fd := arg1
				buf := arg2
				n := arg3
				s := string(memory[buf : buf+n])
				if fd == 1 {
					fmt.Fprintf(os.Stdout, "%s\n", s)
				} else if fd == 2 {
					fmt.Fprintf(os.Stderr, "%s\n", s)
				} else {
					log.Error("unknown file descriptor")
					os.Exit(1)
				}
				stack.Push(arg3)
			} else {
				r, _, _ := syscall.Syscall(uintptr(syscallNumber), uintptr(arg1), uintptr(arg2), uintptr(arg3))
				stack.Push(int(r))
			}
			i++
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
