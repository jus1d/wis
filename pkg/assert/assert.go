package assert

func Assert(cond bool, message string) {
	if !cond {
		panic("assertion error: " + message)
	}
}
