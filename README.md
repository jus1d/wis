# Gollo

**WARNING! THIS LANGUAGE IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT!**

**Gollo** - is a  concatenative, stack-oriented programming language inspired by [Forth]("https://en.wikipedia.org/wiki/Forth_(programming_language)")

## Quick Start

### Run

Run just interprets the program.

```console
$ cat ./examples/goo.glo
34 35 + 69 == put

$ ./gollo run ./examples/goo.glo
1
```

### Compile

Compile mode compiles `.glo` into an assembly code. Be sure that you have `nasm` if you're on x86_64, or `as` if you're on arm64 architecture.

```console
$ cat ./examples/goo.glo
34 35 + 69 == put

$ ./gollo compile ./examples/goo.glo
$ ./examples/goo
1
```