package runner

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	stck "gollo/pkg/stack"
)

func Run(program []operation.Operation) {
	stack := stck.New()
	assert.Assert(operation.OpCount == 9, "Exhaustive handling in run()")
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
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
