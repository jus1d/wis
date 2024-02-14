package log

import (
	"fmt"
	"strings"
)

func Info(message string) {
	fmt.Printf("INFO: %s\n", message)
}

func Error(message string) {
	fmt.Printf("ERROR: %s\n", message)
}

func CMD(command ...string) {
	fmt.Printf("[CMD]: %s\n", strings.Join(command, " "))
}

func TestPassed(message string) {
	fmt.Printf("PASSED: %s\n", message)
}

func TestFailed(message string) {
	fmt.Printf("    FAILURE: %s\n", message)
}

func Usage(compiler string) {
	fmt.Printf("Usage: %s <SUBCOMMAND> [ARGS] ./examples/foo.glo\n", compiler)
	fmt.Println("SUBCOMMANDS:")
	fmt.Println("    run        Simulately run program")
	fmt.Println("    compile    Compile program into an executable")
	fmt.Println("ARGS:")
	fmt.Println("    -r         Instantly run compiled program after compilation")
}
