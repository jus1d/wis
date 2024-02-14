package operation

const (
	PUSH = iota
	PLUS
	MINUS
	MUL
	DIV
	EQ
	NE
	LT
	GT
	LE
	GE
	IF
	ELSE
	END
	DO
	WHILE
	DUMP
	COPY
	TWOCOPY
	SWAP
	DROP
	OVER
	REM
	Count
)

type Operation struct {
	Code   int
	Value  int
	JumpTo int
}

func Push(value int) Operation {
	return Operation{
		Code:  PUSH,
		Value: value,
	}
}

func Plus() Operation {
	return Operation{
		Code: PLUS,
	}
}

func Minus() Operation {
	return Operation{
		Code: MINUS,
	}
}

func Multiply() Operation {
	return Operation{
		Code: MUL,
	}
}

func Division() Operation {
	return Operation{
		Code: DIV,
	}
}

func Equal() Operation {
	return Operation{
		Code: EQ,
	}
}

func NotEqual() Operation {
	return Operation{
		Code: NE,
	}
}

func Less() Operation {
	return Operation{
		Code: LT,
	}
}

func Greater() Operation {
	return Operation{
		Code: GT,
	}
}

func LessOrEqual() Operation {
	return Operation{
		Code: LE,
	}
}

func GreaterOrEqual() Operation {
	return Operation{
		Code: GE,
	}
}

func Dump() Operation {
	return Operation{
		Code: DUMP,
	}
}

func Copy() Operation {
	return Operation{
		Code: COPY,
	}
}

func TwoCopy() Operation {
	return Operation{
		Code: TWOCOPY,
	}
}

func Swap() Operation {
	return Operation{
		Code: SWAP,
	}
}

func Drop() Operation {
	return Operation{
		Code: DROP,
	}
}

func Over() Operation {
	return Operation{
		Code: OVER,
	}
}

func If() Operation {
	return Operation{
		Code: IF,
	}
}

func Else() Operation {
	return Operation{
		Code: ELSE,
	}
}

func End() Operation {
	return Operation{
		Code: END,
	}
}

func Do() Operation {
	return Operation{
		Code: DO,
	}
}

func While() Operation {
	return Operation{
		Code: WHILE,
	}
}

func Rem() Operation {
	return Operation{
		Code: REM,
	}
}
