package operation

const (
	PUSH = iota
	PLUS
	MINUS
	MUL
	DIV
	MOD
	BOR
	BAND
	XOR
	SHL
	SHR
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
	PUT
	COPY
	TWOCOPY
	SWAP
	DROP
	OVER
	SYSCALL0
	SYSCALL1
	SYSCALL2
	SYSCALL3
	Count
)

type Operation struct {
	Code   int
	Value  int
	Loc    string
	JumpTo int
}

func Push(value int, loc string) Operation {
	return Operation{
		Code:  PUSH,
		Value: value,
		Loc:   loc,
	}
}

func Plus(loc string) Operation {
	return Operation{
		Code: PLUS,
		Loc:  loc,
	}
}

func Minus(loc string) Operation {
	return Operation{
		Code: MINUS,
		Loc:  loc,
	}
}

func Multiply(loc string) Operation {
	return Operation{
		Code: MUL,
		Loc:  loc,
	}
}

func Division(loc string) Operation {
	return Operation{
		Code: DIV,
		Loc:  loc,
	}
}

func Mod(loc string) Operation {
	return Operation{
		Code: MOD,
		Loc:  loc,
	}
}

func Bor(loc string) Operation {
	return Operation{
		Code: BOR,
		Loc:  loc,
	}
}

func Band(loc string) Operation {
	return Operation{
		Code: BAND,
		Loc:  loc,
	}
}

func Xor(loc string) Operation {
	return Operation{
		Code: XOR,
		Loc:  loc,
	}
}

func Shl(loc string) Operation {
	return Operation{
		Code: SHL,
		Loc:  loc,
	}
}

func Shr(loc string) Operation {
	return Operation{
		Code: SHR,
		Loc:  loc,
	}
}

func Equal(loc string) Operation {
	return Operation{
		Code: EQ,
		Loc:  loc,
	}
}

func NotEqual(loc string) Operation {
	return Operation{
		Code: NE,
		Loc:  loc,
	}
}

func Less(loc string) Operation {
	return Operation{
		Code: LT,
		Loc:  loc,
	}
}

func Greater(loc string) Operation {
	return Operation{
		Code: GT,
		Loc:  loc,
	}
}

func LessOrEqual(loc string) Operation {
	return Operation{
		Code: LE,
		Loc:  loc,
	}
}

func GreaterOrEqual(loc string) Operation {
	return Operation{
		Code: GE,
		Loc:  loc,
	}
}

func If(loc string) Operation {
	return Operation{
		Code: IF,
		Loc:  loc,
	}
}

func Else(loc string) Operation {
	return Operation{
		Code: ELSE,
		Loc:  loc,
	}
}

func End(loc string) Operation {
	return Operation{
		Code: END,
		Loc:  loc,
	}
}

func Do(loc string) Operation {
	return Operation{
		Code: DO,
		Loc:  loc,
	}
}

func While(loc string) Operation {
	return Operation{
		Code: WHILE,
		Loc:  loc,
	}
}

func Put(loc string) Operation {
	return Operation{
		Code: PUT,
		Loc:  loc,
	}
}

func Copy(loc string) Operation {
	return Operation{
		Code: COPY,
		Loc:  loc,
	}
}

func TwoCopy(loc string) Operation {
	return Operation{
		Code: TWOCOPY,
		Loc:  loc,
	}
}

func Swap(loc string) Operation {
	return Operation{
		Code: SWAP,
		Loc:  loc,
	}
}

func Drop(loc string) Operation {
	return Operation{
		Code: DROP,
		Loc:  loc,
	}
}

func Over(loc string) Operation {
	return Operation{
		Code: OVER,
		Loc:  loc,
	}
}

func Syscall0(loc string) Operation {
	return Operation{
		Code: SYSCALL0,
		Loc:  loc,
	}
}

func Syscall1(loc string) Operation {
	return Operation{
		Code: SYSCALL1,
		Loc:  loc,
	}
}

func Syscall2(loc string) Operation {
	return Operation{
		Code: SYSCALL2,
		Loc:  loc,
	}
}

func Syscall3(loc string) Operation {
	return Operation{
		Code: SYSCALL3,
		Loc:  loc,
	}
}
