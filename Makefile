build: ./cmd/main.go
	go build -o ./gollo ./cmd/main.go

test:
	go run ./tests/main.go