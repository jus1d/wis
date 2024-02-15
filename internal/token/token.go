package token

const (
	WORD = iota
	INT
	STRING
	Count
)

type Token struct {
	Code         int
	StringValue  string
	IntegerValue int
	Loc          string
}

func Word(value string, loc string) Token {
	return Token{
		Code:        WORD,
		StringValue: value,
		Loc:         loc,
	}
}

func Int(value int, loc string) Token {
	return Token{
		Code:         INT,
		IntegerValue: value,
		Loc:          loc,
	}
}

func String(value string, loc string) Token {
	return Token{
		Code:        STRING,
		StringValue: value,
		Loc:         loc,
	}
}
