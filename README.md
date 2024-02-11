# Gollo

**WARNING! THIS LANGUAGE IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT!**

**Gollo** - is a  concatenative, stack-oriented programming language inspired by [Forth]("https://en.wikipedia.org/wiki/Forth_(programming_language)")

## Quick Start

### Run

Run just interprets the program.

```console
$ cat ./examples/foo.gll
34 35 + put

$ ./gollo run ./examples/foo.gll
69
```