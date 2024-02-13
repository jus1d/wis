package runner

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	st "gollo/pkg/stack"
)

func Run(program []operation.Operation) {
	assert.Assert(operation.Count == 18, "Exhaustive handling in runner.Run()")

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
			i++
		case operation.DUMP:
			fmt.Println(stack.Pop())
			i++
		case operation.COPY:
			stack.Push(stack.Peek())
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
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
