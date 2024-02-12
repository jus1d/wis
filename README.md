# Gollo

**Gollo** - is a  concatenative, stack-oriented programming language inspired by [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)). I never wrote anything in Forth, though. So **gollo** is just a concatenative, stack-based programming language. 

## Features
- [x] Stack-based
- [x] Compile for x86_64 architecture
- [ ] Compile for ARM64 architecture
- [ ] Turing-complete

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

Compile mode compiles `.glo` into an assembly code for your architecture. Be sure that you have `nasm` if you're on x86_64, or `as` if you're on arm64 architecture.

```console
$ cat ./examples/goo.glo
34 35 + 69 == put

$ ./gollo compile ./examples/goo.glo
$ ./examples/goo
1
```
