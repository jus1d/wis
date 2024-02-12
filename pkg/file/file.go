package file

import (
	"path/filepath"
	"strings"
)

func GetName(path string) string {
	filename := filepath.Base(path)
	extension := filepath.Ext(filename)
	return strings.TrimSuffix(path, extension)
}

func GetExtension(path string) string {
	filename := filepath.Base(path)
	return filepath.Ext(filename)
}
