CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
AR = ar
PKG_CONFIG ?= pkg-config
CHECK_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags check 2>/dev/null)
CHECK_LIBS ?= $(shell $(PKG_CONFIG) --libs check 2>/dev/null)
GCOVR ?= gcovr
GCOV ?= $(shell if $(CC) --version | grep -qi clang; then if command -v xcrun >/dev/null; then echo 'xcrun llvm-cov gcov'; else echo 'llvm-cov gcov'; fi; else echo gcov; fi)

SOURCES = s21_mem.c s21_string.c s21_sprintf.c s21_strerror.c
OBJECTS = $(SOURCES:.c=.o)
TEST_SOURCES = tests/mariloum_s21_string.c tests/s21_strerror_test.c tests/s21_core_test.c s21_sprintf_test.c
TEST_BINS = $(addprefix .build/,$(notdir $(TEST_SOURCES:.c=)))

DEPENDENCY_HELP = echo 'Установка зависимостей (подробнее: src/README.md):'; \
	echo '  Ubuntu/Debian: sudo apt install build-essential check pkg-config gcovr'; \
	echo '  macOS (Homebrew): brew install check pkgconf gcovr'

.PHONY: all clean test gcov_report check-test-deps check-coverage-deps
all: s21_string.a

s21_string.a: $(OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c s21_string.h s21_sprintf.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.build:
	mkdir -p $@

.build/%: tests/%.c s21_string.a | .build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CHECK_CFLAGS) $< s21_string.a $(LDFLAGS) $(CHECK_LIBS) -lm -o $@

.build/s21_sprintf_test: s21_sprintf_test.c s21_string.a | .build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CHECK_CFLAGS) $< s21_string.a $(LDFLAGS) $(CHECK_LIBS) -lm -o $@

# Order-only dependencies also check tools when test binaries already exist.
$(TEST_BINS): | check-test-deps

check-test-deps:
ifeq ($(origin CHECK_LIBS),file)
	@command -v $(PKG_CONFIG) >/dev/null 2>&1 || { \
		echo 'Ошибка: не найден pkg-config, необходимый для поиска Check.'; \
		$(DEPENDENCY_HELP); exit 1; }
	@$(PKG_CONFIG) --exists check || { \
		echo 'Ошибка: библиотека Check не найдена через pkg-config.'; \
		$(DEPENDENCY_HELP); exit 1; }
endif

check-coverage-deps: check-test-deps
	@$(GCOVR) --version >/dev/null 2>&1 || { \
		echo 'Ошибка: gcovr не найден или не запускается.'; \
		$(DEPENDENCY_HELP); exit 1; }
	@$(GCOV) --version >/dev/null 2>&1 || { \
		echo 'Ошибка: инструмент покрытия gcov/llvm-cov не найден.'; \
		echo 'Установите GCC (Linux) или Xcode Command Line Tools (macOS). См. src/README.md.'; exit 1; }

test: check-test-deps $(TEST_BINS)
	@set -e; for test in $(TEST_BINS); do ./$$test; done

gcov_report: check-coverage-deps
	$(MAKE) clean
	$(MAKE) test CFLAGS='$(CFLAGS) --coverage -O0 -g' LDFLAGS='$(LDFLAGS) --coverage'
	mkdir -p report
	$(GCOVR) --gcov-executable '$(GCOV)' --root . --filter 's21_(mem|string|sprintf|strerror)\.c$$' --html-details report/index.html --json report/coverage.json
	rm -f $(OBJECTS) s21_string.a $(TEST_BINS)
	$(MAKE) all

clean:
	rm -f $(OBJECTS) s21_string.a *.gcda *.gcno
	rm -rf .build report
