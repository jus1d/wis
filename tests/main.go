package main

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func build() error {
	cmd := exec.Command("make", "build")
	_, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("error running gollo: %v", err)
	}
	return nil
}

func runGollo(file string) (string, error) {
	cmd := exec.Command("./gollo", "run", file)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("error running gollo: %v", err)
	}
	return string(output), nil
}

func compileAndRun(file string) (string, error) {
	cmd := exec.Command("./gollo", "compile", file)
	_, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("error running gollo: %v", err)
	}

	file = strings.TrimSuffix(file, filepath.Ext(file))
	cmd = exec.Command(file)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("error while running compiled file: %v", err)
	}

	return string(output), nil
}

func main() {
	err := build()
	if err != nil {
		fmt.Printf("Can't build ./gollo: %s", err.Error())
		os.Exit(1)
	}

	directoryPath := "./tests/"

	files, err := os.ReadDir(directoryPath)
	if err != nil {
		fmt.Println("Error reading directory:", err)
		return
	}

	gotErrors := false

	for _, file := range files {
		if strings.HasSuffix(file.Name(), ".glo") {
			gloFilePath := filepath.Join(directoryPath, file.Name())
			outputFilePath := strings.TrimSuffix(gloFilePath, ".glo") + ".output.txt"

			expectedOutput, err := os.ReadFile(outputFilePath)
			if err != nil {
				fmt.Printf("Error reading expected output file %s: %v\n", outputFilePath, err)
				continue
			}

			actualRunOutput, err := runGollo(gloFilePath)
			if err != nil {
				fmt.Printf("Error running gollo for %s: %v\n", gloFilePath, err)
				continue
			}

			actualCompileOutput, err := compileAndRun(gloFilePath)
			if err != nil {
				fmt.Printf("Error compiling gollo for %s: %v\n", gloFilePath, err)
				continue
			}

			if strings.TrimSpace(actualRunOutput) != strings.TrimSpace(string(expectedOutput)) {
				fmt.Printf("Test failed for RUN: %s\n", gloFilePath)
				// fmt.Println("Expected output:")
				// fmt.Println(string(expectedOutput))
				// fmt.Println("Actual output:")
				// fmt.Println(actualRunOutput)
				gotErrors = true
			} else {
				fmt.Printf("Test passed for RUN: %s\n", gloFilePath)
			}

			if strings.TrimSpace(actualCompileOutput) != strings.TrimSpace(string(expectedOutput)) {
				fmt.Printf("Test failed for COMPILE: %s\n", gloFilePath)
				// fmt.Println("Expected output:")
				// fmt.Println(string(expectedOutput))
				// fmt.Println("Actual output:")
				// fmt.Println(actualCompileOutput)
				gotErrors = true
			} else {
				fmt.Printf("Test passed for COMPILE: %s\n", gloFilePath)
			}
		}
	}

	if gotErrors {
		os.Exit(1)
	} else {
		fmt.Printf("\nAll tests successfully passed!\n")
	}
}
