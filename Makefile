build: ./cmd/gollo/main.go
	go build -o ./gollo ./cmd/gollo/main.go

test:
	go run ./cmd/tests/main.go