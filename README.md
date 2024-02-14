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

**Usage:**

```
1
while copy 10 <= do
  copy put
end
```

**Output:** `1 2 3 4 5 6 7 8 9 10`

---

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