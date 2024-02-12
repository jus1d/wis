package operation

const (
	OpPush = iota
	OpPlus
	OpMinus
	OpMultiply
	OpDivision
	OpEqual
	OpNotEqual
	OpLess
	OpGreater
	OpLessOrEqual
	OpGreaterOrEqual
	OpIf
	OpEnd
	OpDump
	OpCopy
	OpSwap
	OpDrop
	Count
)

type Operation struct {
	Code  int
	Value int
	End   int
}

func Push(value int) Operation {
	return Operation{
		Code:  OpPush,
		Value: value,
	}
}

func Plus() Operation {
	return Operation{
		Code: OpPlus,
	}
}

func Minus() Operation {
	return Operation{
		Code: OpMinus,
	}
}

func Multiply() Operation {
	return Operation{
		Code: OpMultiply,
	}
}

func Division() Operation {
	return Operation{
		Code: OpDivision,
	}
}

func Equal() Operation {
	return Operation{
		Code: OpEqual,
	}
}

func NotEqual() Operation {
	return Operation{
		Code: OpNotEqual,
	}
}

func Less() Operation {
	return Operation{
		Code: OpLess,
	}
}

func Greater() Operation {
	return Operation{
		Code: OpGreater,
	}
}

func LessOrEqual() Operation {
	return Operation{
		Code: OpLessOrEqual,
	}
}

func GreaterOrEqual() Operation {
	return Operation{
		Code: OpGreaterOrEqual,
	}
}

func Dump() Operation {
	return Operation{
		Code: OpDump,
	}
}

func Copy() Operation {
	return Operation{
		Code: OpCopy,
	}
}

func Swap() Operation {
	return Operation{
		Code: OpSwap,
	}
}

func Drop() Operation {
	return Operation{
		Code: OpDrop,
	}
}

func If() Operation {
	return Operation{
		Code: OpIf,
	}
}

func End() Operation {
	return Operation{
		Code: OpEnd,
	}
}
