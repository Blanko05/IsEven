# is-even-but-worse

Have you ever needed to check if a number is even? I know you have. And I know you probably used something ancient and primitive like this:

```c
if (x % 2 == 1) return false;
```

Embarrassing. One line. No dependencies. What is this, 1972?

**I decided to fix that.**

## What this is

A C program that, given a number N, **generates two brand new C files** containing a hardcoded `if`-statement for every integer from 0 to N — each one matching against:

- The raw integer
- The numeric string (`"375001"`)
- The lowercase written form (`"three hundred seventy-five thousand one"`)
- The Title Case written form (`"Three Hundred Seventy-Five Thousand One"`)
- The UPPERCASE written form (`"THREE HUNDRED SEVENTY-FIVE THOUSAND ONE"`)

It then **compiles both files, runs them on separate threads simultaneously, consults GPT-4o-mini for a third opinion, and only renders a final verdict once all three agree.**

```c
if (number == 4 || strcmp(str, "four") == 0
    || strcmp(str, "Four") == 0
    || strcmp(str, "FOUR") == 0) return true;
```

*Robust. Readable. Enterprise-ready.*

## How it works

```
[IsEven.exe] says: true
[IsOdd.exe]  says: false
[OpenAI]     says: true

FINAL VERDICT: 42 is EVEN. (All three agree.)
```

Three independent sources. Two compiled executables. One live AI API call. Zero instances of `% 2`.

Because what if one program is wrong? What if *both* are wrong? What if numbers themselves are lying to us? You can't just trust a program you wrote yourself. That's how bugs happen.

One can never be too sure.

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
gcc generator.c -o generator.exe -IC:\msys64\ucrt64\include -LC:\msys64\ucrt64\lib -lcurl
./generator.exe
# Enter a number: 42
# ... generates IsEven.c and IsOdd.c, compiles both, runs them on separate threads, calls OpenAI, renders verdict
```

Don't forget to paste your OpenAI API key in `generator.c` before building.

## Requirements

- Windows (uses `copy`, `del`, `gcc`)
- GCC via MSYS2 ucrt64
- libcurl (`pacman -S mingw-w64-ucrt-x86_64-curl`)
- An OpenAI API key
- A great deal of patience for large N
- No shame

> **Linux/Mac users:** swap `copy` → `cp`, `del` → `rm`, and drop the `.exe`. You're smart, you'll figure it out. libcurl is probably already installed.

## Why

Inspired by the legendary [is-even](https://github.com/samuelmarina/is-even) JS project. They built it by hand. I automated it in C, added multithreading, and consulted AI. Because one can never be too sure.

## License

Do whatever you want with this. I can't stop you.
