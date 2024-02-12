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
		case operation.OpPush:
			stack.Push(op.Value)
			i++
		case operation.OpPlus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a + b)
			i++
		case operation.OpMinus:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b - a)
			i++
		case operation.OpMultiply:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b * a)
			i++
		case operation.OpDivision:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(b / a)
			i++
		case operation.OpEqual:
			a := stack.Pop()
			b := stack.Pop()
			if a == b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpNotEqual:
			a := stack.Pop()
			b := stack.Pop()
			if a != b {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpLess:
			a := stack.Pop()
			b := stack.Pop()
			if b < a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpGreater:
			a := stack.Pop()
			b := stack.Pop()
			if b > a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpLessOrEqual:
			a := stack.Pop()
			b := stack.Pop()
			if b <= a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpGreaterOrEqual:
			a := stack.Pop()
			b := stack.Pop()
			if b >= a {
				stack.Push(1)
			} else {
				stack.Push(0)
			}
			i++
		case operation.OpIf:
			a := stack.Pop()
			if a == 0 {
				i = op.JumpTo
			} else {
				i++
			}
		case operation.OpElse:
			i = op.JumpTo
		case operation.OpEnd:
			i++
		case operation.OpDump:
			fmt.Println(stack.Pop())
			i++
		case operation.OpCopy:
			stack.Push(stack.Peek())
			i++
		case operation.OpSwap:
			a := stack.Pop()
			b := stack.Pop()
			stack.Push(a)
			stack.Push(b)
			i++
		case operation.OpDrop:
			_ = stack.Pop()
			i++
		default:
			assert.Assert(false, "unreachable")
		}
	}
}
