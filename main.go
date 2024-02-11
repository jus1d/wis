package main

import (
	"fmt"
	"gollo/stack"
)

const (
	OperationPush = iota
	OperationPlus
	OperationDump
	OperationCount
)

type Operation struct {
	Code  int
	Value int64
}

func push(value int64) Operation {
	return Operation{
		Code:  OperationPush,
		Value: value,
	}
}

func plus() Operation {
	return Operation{
		Code: OperationPlus,
	}
}

func dump() Operation {
	return Operation{
		Code: OperationDump,
	}
}

func assert(cond bool, message string) {
	if !cond {
		panic("assertion error: " + message)
	}
}

func chop[T any](arr []T) (T, []T) {
	if len(arr) == 0 {
		panic("ERROR: can't chop")
	}
	return arr[0], arr[1:]
}

func run(program []Operation) {
	st := stack.New()
	for _, op := range program {
		if op.Code == OperationPush {
			st.Push(op.Value)
		} else if op.Code == OperationPlus {
			a := st.Pop()
			b := st.Pop()
			st.Push(a + b)
		} else if op.Code == OperationDump {
			fmt.Println(st.Pop())
		}
	}
}

func compile(program []Operation) {
	assert(false, "not implemented")
}

func main() {
	program := []Operation{
		push(34),
		push(35),
		plus(),
		dump(),
	}

	run(program)
}
