package stack

// Stack represents a stack data structure
type Stack struct {
	items []int64
}

func New() *Stack {
	return &Stack{items: make([]int64, 0)}
}

// Push adds an item to the top of the stack
func (s *Stack) Push(item int64) {
	s.items = append(s.items, item)
}

// Pop removes and returns the top item from the stack
func (s *Stack) Pop() int64 {
	if len(s.items) == 0 {
		return 0
	}
	topIndex := len(s.items) - 1
	popped := s.items[topIndex]
	s.items = s.items[:topIndex]
	return popped
}

// Peek returns the top item from the stack without removing it
func (s *Stack) Peek() int64 {
	if len(s.items) == 0 {
		return 0
	}
	return s.items[len(s.items)-1]
}

// IsEmpty checks if the stack is empty
func (s *Stack) IsEmpty() bool {
	return len(s.items) == 0
}
