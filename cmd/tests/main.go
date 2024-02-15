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
		log.Error("Can't build a compiler: " + err.Error())
		os.Exit(1)
	}

	testsDirectory := "./tests/"

	files, err := os.ReadDir(testsDirectory)
	if err != nil {
		log.Error("Error reading directory: " + err.Error())
		return
	}

	runsFailed := 0
	compilesFailed := 0
	totalTests := 0

	for _, file := range files {
		if strings.HasSuffix(file.Name(), ".glo") {
			runFailed := false
			compilationFailed := false
			totalTests++

			name := strings.TrimSuffix(filepath.Join(testsDirectory, file.Name()), ".glo")

			outputFilepath := fmt.Sprintf("%s.output.txt", name)
			golloFilepath := fmt.Sprintf("%s.glo", name)

			expectedOutput, err := os.ReadFile(outputFilepath)
			if err != nil {
				log.Error("Error reading expected output file: " + err.Error())
				continue
			}

			actualRunOutput, err := run(golloFilepath)
			if err != nil {
				log.TestFailed(fmt.Sprintf("%s.glo: Running exitted abnormally: %s", name, err.Error()))
				runFailed = true
			}

			actualCompileOutput, err := compile(golloFilepath)
			if err != nil {
				log.TestFailed(fmt.Sprintf("%s.glo: Compiling exitted abnormally: %s", name, err.Error()))
				compilationFailed = true
			}

			if strings.TrimSpace(actualRunOutput) != strings.TrimSpace(string(expectedOutput)) {
				log.TestFailed(fmt.Sprintf("%s.glo: Expected and actual outputs didn't match in run mode", name))
				runFailed = true
			} else {
				log.TestPassed(fmt.Sprintf("%s.glo: Sucessfully passed the test in run mode", name))
			}

			if strings.TrimSpace(actualCompileOutput) != strings.TrimSpace(string(expectedOutput)) {
				log.TestFailed(fmt.Sprintf("%s.glo: Expected and actual outputs didn't match in compile mode", name))
				compilationFailed = true
			} else {
				log.TestPassed(fmt.Sprintf("%s.glo: Sucessfully passed the test in compile mode", name))
			}

			if runFailed {
				runsFailed++
			}
			if compilationFailed {
				compilesFailed++
			}

			fmt.Println()
		}
	}

	fmt.Println()
	if runsFailed != 0 || compilesFailed != 0 {
		log.TestFailed(fmt.Sprintf("Runs failed: %d/%d, compilations failed: %d/%d", runsFailed, totalTests, compilesFailed, totalTests))
		os.Exit(1)
	} else {
		log.TestPassed(fmt.Sprintf("All tests passed in %s", testsDirectory))
	}
}
