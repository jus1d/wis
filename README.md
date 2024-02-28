# Gollo

**Gollo** - is a  concatenative, stack-oriented programming language inspired by [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)). I never wrote anything in Forth, though. So **gollo** is just a concatenative, stack-based programming language.

## Features
- [x] Stack-based
- [x] Compile for `x86_64` architecture
- [ ] ~~Compile for `arm64` architecture~~ (maybe in the future, but now i dont wanna fuck with `arm64` assembly)
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

---

### Compile

Compile mode compiles `.glo` into an assembly code for your architecture. Be sure that you have `nasm` if you're on x86_64, or `as` if you're on arm64 architecture.

```console
$ cat ./examples/goo.glo
34 35 + 69 == put

$ ./gollo compile ./examples/goo.glo
$ ./examples/goo
1
```

## Language Describe

### Control Flows

---

#### `if-else` - default condition workflow

`if` pops the value on top of the stack. If popped value is zero, program jumps to `else` or `end` of current block. Otherwise, program will go into the `if` branch

**Usage:**

```
34 35 + 69 == if
  69 put
else
  420 put
end
```

**Output:** `69`

---

#### `while` - loops with condition

`do` pops the value from top of the stack. If popped value is zero, program jumps to `end` of current block. Otherwise, program jump into the `while` block

**Usage:**

```
1
while copy 10 <= do
  copy put
end
```

**Output:** `1 2 3 4 5 6 7 8 9 10`

### Bindings

---

**Usage:**

```
bind exit sys_exit syscall1 end

69 exit
```

That program will exit with `69` exit code.

### Usings

--- 

Only needed bindings will expand to operation at the compilation step

```
here eputs ": ERROR: some error message" eputs
69 exit
```

### Memory

- `mem` keyword pushes a pointer to buffer, where you can write and read
- `store32` keyword puts a 32-bit value to memory buffer

```
mem 69 store32
```
Puts a `69` to program's memory

- `load32` keyword loads a 32-bit value from the memory buffer to the stack

```
mem load32 put
```

Loads a value from memory to the stack

### Operations

---

#### `<int>`

Pushes integer number on top of the stack

**Stack:** `1` => `1 2`

---

#### `<string>`

Pushes integer number that represents string's length, and pointer where string is started

**Stack:** `1` => `1 size ptr`

---

#### `put`

Removes and prints number from top of the stack

**Stack:** `1 2` => `1`

**Output:** `2`

---

#### `fputs`

`fputs` (put string) is a built-in shortcut for: `sys_write syscall3 drop`

`fputs` call `write` syscall and drop it's return value

**Example:**

```
"Hello, world!"             stdout fputs
"ERROR: some error message" stderr fputs
```

---

#### `copy`

Copies integer on top of the stack

**Stack:** `1 2` => `1 2 2`

---

#### `over`

Copies previous integer from stack to top

**Stack:** `1 2` => `1 2 1`

---

#### `swap`

Swaps two integers on top of the stack

**Stack:** `1 2` => `2 1`

---

#### `drop`

Drops integer from top of the stack

**Stack:** `1 2 3` => `1 2`

---

#### `rot`

Rotates 3 values on top of the stack

**Stack:** `1 2 3` => `3 1 2`

---

### System calls

You can use `syscall<n>`, where `n` is a number between 1 and 6

**Example:**

Call exit syscall with non-zero exit code

```
69 sys_exit syscall1
```

This program will exit with `69` exit code