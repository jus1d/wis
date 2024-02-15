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
