package compiler

import (
	"fmt"
	"gollo/internal/operation"
	"gollo/pkg/assert"
	"gollo/pkg/command"
	"gollo/pkg/log"
	"runtime"
)

func Compile(name string, program []operation.Operation) {
	switch runtime.GOARCH {
	case "amd64":
		compile_x86_64(fmt.Sprintf("%s.asm", name), program)
		log.Info(fmt.Sprintf("Assembly generated to %s.asm", name))
		command.MustExecuteEchoed(false, "nasm", "-felf64", "-o", fmt.Sprintf("%s.o", name), fmt.Sprintf("%s.asm", name))
		command.MustExecuteEchoed(false, "ld", "-o", name, fmt.Sprintf("%s.o", name))
		command.Execute(false, "rm", fmt.Sprintf("%s.o", name))
		log.Info("Compiled to " + name)
	case "arm64":
		compile_arm64(fmt.Sprintf("%s.s", name), program)
		log.Info(fmt.Sprintf("Assembly generated to %s.s", name))
		command.MustExecuteEchoed(false, "as", "-o", fmt.Sprintf("%s.o", name), fmt.Sprintf("%s.s", name))
		command.MustExecuteEchoed(false, "ld", "-o", name, fmt.Sprintf("%s.o", name))
		command.Execute(false, "rm", fmt.Sprintf("%s.o", name))
		log.Info("Compiled to " + name)
	default:
		assert.Assert(false, "unsupported platform: "+runtime.GOARCH)
	}
}
