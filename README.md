# is-even-but-worse

Have you ever needed to check if a number is even? I know you have. And I know you probably used something ancient and primitive like this:

```c
if (x % 2 == 1) return false;
```

Embarrassing. One line. No dependencies. What is this, 1972?

**I decided to fix that.**

## What this is

A C program that, given a number N, **generates a brand new C file** containing a hardcoded `if`-statement for every integer from 0 to N — each one matching against:

- The raw integer
- The numeric string (`"375001"`)
- The lowercase written form (`"three hundred seventy-five thousand one"`)
- The Title Case written form (`"Three Hundred Seventy-Five Thousand One"`)
- The UPPERCASE written form (`"THREE HUNDRED SEVENTY-FIVE THOUSAND ONE"`)

It then **compiles and runs the generated file automatically.**

```c
if (number == 4 || strcmp(str, "four") == 0
    || strcmp(str, "Four") == 0
    || strcmp(str, "FOUR") == 0) return true;
```

*Robust. Readable. Enterprise-ready.*

## Performance

| N | Generated file size | Compile time | Your CPU's feelings |
|---|---|---|---|
| 100 | ~12 KB | instant | fine |
| 1,000 | ~100 KB | a moment | concerned |
| 10,000 | ~1.2 MB | a few seconds | sweating |
| 100,000 | ~14 MB | good time to get coffee | betrayed |

Is it O(1) lookup? Technically yes.  
Is it O(n) to generate? Also yes.  
Is it an abomination? We don't say that word here.

## Usage

```bash
gcc generator.c -o generator
./generator
# Enter a number: 1000
# ... compiles and runs IsEven.exe automatically
```

The generated binary will then ask you for a number and tell you if it's even. You're welcome.

## Requirements

- Windows (uses `copy`, `del`, `gcc`)
- GCC
- A great deal of patience for large N
- No shame

> **Linux/Mac users:** swap `copy` → `cp`, `del` → `rm`, and drop the `.exe`. You're smart, you'll figure it out.

## Why

Inspired by the legendary [is-even](https://github.com/samuelmarina/is-even) JS project. They built it by hand. I automated it. In C. Because I could.

## License

Do whatever you want with this. I can't stop you.
