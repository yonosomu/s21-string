# s21_string+

<!-- tech-stack-badges -->
<p align="center">
<img alt="C" src="https://img.shields.io/badge/C-C11-A8B9CC?style=for-the-badge&amp;logo=c&amp;logoColor=white" />
<img alt="Make" src="https://img.shields.io/badge/Make-Build-427819?style=for-the-badge&amp;logo=gnu&amp;logoColor=white" />
<img alt="Check" src="https://img.shields.io/badge/Check-Tests-4B5563?style=for-the-badge" />
<img alt="gcovr" src="https://img.shields.io/badge/gcovr-Coverage-2563EB?style=for-the-badge" />
</p>
<!-- /tech-stack-badges -->

A C11 implementation of common `string.h` and `sprintf` functionality. This
School 21 curriculum project is presented as a portfolio project: the library
is implemented from scratch and tested against the standard-library behaviour.

## Highlights

- Static library: `s21_string.a`
- C11, compiled with `-Wall -Wextra -Werror`
- Memory operations: `memchr`, `memcmp`, `memcpy`, `memmove`, `memset`
- String operations: `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`,
  `strcmp`, `strncmp`, `strchr`, `strrchr`, `strstr`, `strspn`, `strcspn`,
  `strpbrk`, `strtok`, and `strerror`
- Partial `sprintf` with integer, floating-point, string, character, percent,
  width, precision, flags, and length-modifier handling
- Automated tests using [Check](https://libcheck.github.io/check/)

## Project layout

```text
.
├── s21_string.h       # Public API
├── s21_*.c            # Library implementation
├── s21_sprintf.h      # Formatting internals
├── tests/             # Check-based unit tests
├── s21_sprintf_test.c # Additional sprintf tests
└── Makefile
```

## Build and test

### Requirements

- C11 compiler (`gcc` or `clang`)
- `make` and `ar`
- [Check](https://libcheck.github.io/check/) and `pkg-config` to run tests
- `gcovr` to generate a coverage report

<details>
<summary>Install dependencies</summary>

**Ubuntu / Debian**

```sh
sudo apt update
sudo apt install build-essential check pkg-config gcovr
```

**macOS**

```sh
xcode-select --install
brew install check pkgconf gcovr
```
</details>

```sh
# Build the static library
make

# Run all tests
make test

# Generate HTML and JSON coverage reports in report/
make gcov_report

# Remove generated files
make clean
```

If Check cannot create temporary files in a restricted environment, run:

```sh
mkdir -p .tmp-check
TEMP="$PWD/.tmp-check" TMPDIR="$PWD/.tmp-check" make test
rm -rf .tmp-check
```

## API example

```c
#include "s21_string.h"

int main(void) {
  char buffer[64];
  s21_sprintf(buffer, "Hello, %s!", "world");
  return s21_strcmp(buffer, "Hello, world!");
}
```
