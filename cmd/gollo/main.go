package main

import (
	"gollo/internal/compiler"
	"gollo/internal/lexer"
	"gollo/internal/runner"
	"gollo/pkg/command"
	"gollo/pkg/file"
	"gollo/pkg/log"
	"gollo/pkg/slice"
	"os"
)

func exploreProvidedFilepath(path string) string {
	info, err := os.Stat(path)
	if err != nil {
		log.Error("can't read path: " + path)
		os.Exit(1)
	}

	if info.IsDir() {
		if path[len(path)-1] == '/' {
			path += "main.glo"
		} else {
			path += "/main.glo"
		}
	}

	ext := file.GetExtension(path)
	if ext != ".glo" {
		log.Error("unknown file extension: " + ext)
		os.Exit(1)
	}

	return path
}

func main() {
	args := os.Args
	compilerName, args := slice.Chop(args)

	if len(args) < 1 {
		log.Usage(compilerName)
		log.Error("no subcommand provided")
		os.Exit(1)
	}
	subcommand, args := slice.Chop(args)

	if len(args) < 1 {
		log.Usage(compilerName)
		log.Error("no filepath provided")
		os.Exit(1)
	}

	switch subcommand {
	case "run":
		path, _ := slice.Chop(args)

		path = exploreProvidedFilepath(path)

		program := lexer.LexFile(compilerName, path)
		runner.Run(program)
	case "compile":
		runAfterCompile := false

		path, args := slice.Chop(args)
		if path == "-r" {
			runAfterCompile = true
			path, _ = slice.Chop(args)
		}

		path = exploreProvidedFilepath(path)

		program := lexer.LexFile(compilerName, path)
		name := file.GetName(path)

		compiler.Compile(name, program)

		if runAfterCompile {
			command.Execute(true, name)
		}
	default:
		log.Usage(compilerName)
		log.Error("unknown subcommand provided")
	}
}
