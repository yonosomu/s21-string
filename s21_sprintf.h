#ifndef S21_SPRINTF
#define S21_SPRINTF

#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct {
  int minus;
  int plus;
  int space;
  int width;
  int flag_precision;
  int precision;
  char length;
  char specifier;
} Specifier;

typedef struct {
  char c;
  long long ll;
  long double f;
  char *s;
  unsigned long long ull;
} Value;

int s21_sprintf(char *str, const char *format, ...);

#endif