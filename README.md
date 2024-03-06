# WIS

**WIS** is a concatenative, stack-based programming language inspired by [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language)).

## Quick Start

Running mode is deprecated now. So only compilation mode is currently available.

### Compilation

```console
$ cat ./goo.wis
use "std.wis"
"Hello, world!\n" puts

$ ./wis -quiet -r ./goo.wis
Hello, world!
```

## Language

### Basic operations

`<int>`

Pushes an integer number to the stack

---

`<string>`

Pushes size of string represented as `int` value, and `ptr` where string starts at

---

`put`

Removes and prints unsigned integer from top of the stack

---

`fputs`

Built-in shortcut for sequence of operations: `sys_write syscall3 drop`. The o reason why it is a built-in operation and not a standard binding, to save type checking for this puts operations. Because we can't correctly check types of syscalls' arguments

---

### Basic stack manipulations

`copy`

Duplicates a value on top of the stack

**Stack:** `1 2` => `1 2 2`

---

`over`

Duplicates previous value from stack to top

**Stack:** `1 2` => `1 2 1`

---

`swap`

Swaps around 2 top values in the stack

**Stack:** `1 2` => `2 1`

---

`drop`

Removes top value from the stack

**Stack:** `1 2` => `1`

---

`rot`

Rotate top 3 values on the stack to right direction

**Stack:** `1 2 3` => `3 1 2`

---

`2swap`

Swaps 2 pairs of arguments on top of the stack

**Stack:** `1 2 3 4` => `3 4 1 2`

---

### Memory manipulation

- `mem` operation pushes to the stack a pointer to memory buffer, where you can read and write some data
- `@8` and `@64` (forth-like load) operations loads a byte or 64-bit value from provided pointer and pushes this value to the stack
- `!8` and `!64` (forth-like store) operations puts a byte or 64-bit value to the memory buffer

See examples [here](./tests/09-memory.wis) and [here](./tests/10-64bit-memory.wis)

---

### Bindings

`bind` keyword provide an ability to create some sort of inline procedures (VERY WEIRD, OFC), but al least that is something.

At the compilation step, name of binding will expand to operations, bind to that name

**Example:**

```
bind print-sum + put end

34 35 print-sum
```

Actually, that program after compilation will turn to that view:

```
34 35 + put
```

---

### Control flows

`if` statement

**Usage:**

```
34 35 + 69 == if
  1 put
else
  0 put
end
```

`if` statement currently have no `elif`, but uou can combine block like below:

```
false if
  1 put
else true if 
  2 put
else
  3 put
end end
```

But it always will end with this weird `end`-trail

---

`while` loop

**Usage:**

```
1 while copy 10 <= do
  copy put
end drop
```

This program will print numbers from 0 to 10

---

### Other

`use` keyword provide an ability to include outer file to current. If you have ANY operations outside the bindings in the including file, they will NOT ingore by the compiler.

---

`syscal<n>` allows you to manipulate directly with linux kernel. 

**Example:**

`syscall3` accepts 3 arguments and 1 additional argument for syscall number

```
"Some data\n" 1 1 syscall3
69 60 syscall1
```

That simple program will print "Some data" to `stdout` and exit with 69 exit code

For now, I have no idea how to create proper type checking for syscalls' arguments, so now we are just checking their amount only

---

### Standard library

#### Constants

- `stdin`, `sydout` and `stderr` are constants for default streams

#### Wrappers

- `write`, `read`, `open` and `exit` are wrappers for common and used syscalls
- `puts` wrapper for printing strings to `stdout`
- `eputs` wrapper for printing strings to `stderr`
- `putd` is a binding for printing signed integers

#### Stack operations

- `2copy`: Stack: `1 2` => `1 2 1 2`
- `2drop`: Stack: `1 2 3` => `1`

#### String bindgings

- `strlen` - binding for calculate string's length
- `endl` just shortcut for "\n" symbol