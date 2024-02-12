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

func main() {
	args := os.Args
	_, args = slice.Chop(args)

	if len(args) < 1 {
		log.Usage()
		log.Error("no subcommand provided")
		os.Exit(1)
	}
	subcommand, args := slice.Chop(args)

	if len(args) < 1 {
		log.Usage()
		log.Error("no filepath provided")
		os.Exit(1)
	}
	switch subcommand {
	case "run":
		path, _ := slice.Chop(args)

		ext := file.GetExtension(path)
		if ext != ".glo" {
			log.Error("unknown file extension: " + ext)
			os.Exit(1)
		}

		program := lexer.LoadProgramFromFile(path)
		runner.Run(program)
	case "compile":
		runAfterCompile := false

		path, args := slice.Chop(args)
		if path == "-r" {
			runAfterCompile = true
			path, _ = slice.Chop(args)
		}

		ext := file.GetExtension(path)
		if ext != ".glo" {
			log.Error("unknown file extension: " + ext)
			os.Exit(1)
		}

		program := lexer.LoadProgramFromFile(path)
		name := file.GetName(path)

		compiler.Compile(name, program)

		if runAfterCompile {
			command.Execute(true, name)
		}
	default:
		log.Usage()
		log.Error("unknown subcommand provided")
	}
}