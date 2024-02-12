package runner

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	st "gollo/pkg/stack"
)

func Run(program []operation.Operation) {
	stack := st.New()
	assert.Assert(operation.OpCount == 11, "Exhaustive handling in runner.Run()")
	for _, op := range program {
		switch op.Code {
		case operation.OpPush:
			stack.Push(op.Value)
		case operation.OpPlus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a + b)
		case operation.OpMinus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b - a)
		case operation.OpMultiply:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b * a)
		case operation.OpDivision:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b / a)
		case operation.OpEqual:
			a := stack.Pop()
			b := stack.Pop()
			if a == b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
		case operation.OpLess:
			a := stack.Pop()
			b := stack.Pop()
			if b < a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
		case operation.OpGreater:
			a := stack.Pop()
			b := stack.Pop()
			if b > a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
		case operation.OpDump:
			fmt.Println(stack.Pop())
		case operation.OpCopy:
			stack.Push(stack.Peek())
		case operation.OpSwap:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a)
			stack.Push(b)
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
