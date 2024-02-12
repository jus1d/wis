package slice

func Chop[T any](arr []T) (T, []T) {
	if len(arr) == 0 {
		panic("ERROR: can't chop")
	}
	return arr[0], arr[1:]
}
