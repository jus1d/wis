package command

import (
	"fmt"
	"gollo/pkg/log"
	"os"
	"os/exec"
)

func Execute(logOutput bool, cmd ...string) (string, error) {
	c := exec.Command(cmd[0], cmd[1:]...)
	output, err := c.Output()
	if err != nil {
		log.Error("error while executing command: " + err.Error())
		return "", err
	}

	if logOutput {
		fmt.Printf("%s", string(output))
	}

	return string(output), nil
}

func ExecuteEchoed(logOutput bool, cmd ...string) (string, error) {
	log.CMD(cmd...)
	return Execute(logOutput, cmd...)
}

func MustExecute(logOutput bool, cmd ...string) string {
	c := exec.Command(cmd[0], cmd[1:]...)
	output, err := c.Output()
	if err != nil {
		log.Error("error while executing command: " + err.Error())
		os.Exit(1)
	}

	if logOutput {
		fmt.Printf("%s", string(output))
	}

	return string(output)
}

func MustExecuteEchoed(logOutput bool, cmd ...string) string {
	log.CMD(cmd...)
	return MustExecute(logOutput, cmd...)
}
