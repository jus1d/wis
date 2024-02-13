package main

import (
	"fmt"
	"gollo/pkg/command"
	"gollo/pkg/log"
	"os"
	"path/filepath"
	"strings"
)

func build() error {
	_, err := command.Execute(true, "make", "build")
	return err
}

func run(file string) (string, error) {
	return command.Execute(false, "./gollo", "run", file)
}

func compile(file string) (string, error) {
	_, err := command.Execute(false, "./gollo", "compile", file)
	if err != nil {
		return "", fmt.Errorf("error running gollo: %v", err)
	}

	file = strings.TrimSuffix(file, filepath.Ext(file))
	return command.Execute(false, file)
}

func main() {
	err := build()
	if err != nil {
		log.Error("can't build a compiler: " + err.Error())
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

			actualRunOutput, err := run(gloFilePath)
			if err != nil {
				fmt.Printf("Error running gollo for %s: %v\n", gloFilePath, err)
				continue
			}

			actualCompileOutput, err := compile(gloFilePath)
			if err != nil {
				fmt.Printf("Error compiling gollo for %s: %v\n", gloFilePath, err)
				continue
			}

			if strings.TrimSpace(actualRunOutput) != strings.TrimSpace(string(expectedOutput)) {
				fmt.Printf("Test failed for RUN: %s\n", gloFilePath)
				gotErrors = true
			} else {
				fmt.Printf("Test passed for RUN: %s\n", gloFilePath)
			}

			if strings.TrimSpace(actualCompileOutput) != strings.TrimSpace(string(expectedOutput)) {
				fmt.Printf("Test failed for COMPILE: %s\n", gloFilePath)
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
