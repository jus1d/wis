# Gollo

**Gollo** - is a  concatenative, stack-oriented programming language inspired by [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)). I never wrote anything in Forth, though. So **gollo** is just a concatenative, stack-based programming language.

## Features
- [x] Stack-based
- [x] Compile for `x86_64` architecture
- [ ] Compile for `arm64` architecture
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
bind exit 60 syscall1 end

69 exit
```

That program will exit with `69` exit code.

### Operations

---

#### `<int>`

Pushes integer number on top of the stack

**Stack:** `1` => `1 2`

---

#### `put`

Removes and prints number from top of the stack

**Stack:** `1 2` => `1`

**Output:** `2`

---

#### `puts`

`puts` (put string) is a built-in shortcut for: `1 1 syscall3 drop`

`puts` pushes `1` to the stack, and call `write` syscall

**Example:**

```
"Hello, world!" puts
```

---

#### `copy`

Copies integer on top of the stack

**Stack:** `1 2` => `1 2 2`

---

#### `2copy`

Copies two integers on top of the stack

**Stack:** `1 2 3` => `1 2 3 2 3`

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

You can use `syscall<n>`, where `n` is between 1 and 3. Now supported only syscalls that accepts 1-3 arguments.

**Example:**

Call exit syscall with non-zero exit code

```
1 60 syscall1
```