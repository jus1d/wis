package command

import (
	"fmt"
	"gollo/pkg/log"
	"os"
	"os/exec"
)

func Execute(logOutput bool, cmd ...string) {
	log.CMD(cmd...)
	c := exec.Command(cmd[0], cmd[1:]...)
	output, err := c.Output()
	if err != nil {
		log.Error("can't execute command: " + err.Error())
		os.Exit(1)
	}

	if logOutput {
		fmt.Printf("%s", string(output))
	}
}
