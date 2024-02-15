package token

const (
	WORD = iota
	INT
	Count
)

type Token struct {
	Code         int
	StringValue  string
	IntegerValue int
	Loc          string
}
