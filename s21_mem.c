#include "s21_string.h"

void* s21_memchr(const void* str, int c, s21_size n) {
  const unsigned char* s = str;

  for (s21_size i = 0; i < n; i++) {
    if (s[i] == (unsigned char)c) return (void*)&s[i];
  }

  return S21_NULL;
}

int s21_memcmp(const void* str1, const void* str2, s21_size n) {
  const unsigned char* s1 = str1;
  const unsigned char* s2 = str2;

  for (s21_size i = 0; i < n; i++) {
    if (s1[i] != s2[i]) return s1[i] - s2[i];
  }

  return 0;
}

void* s21_memcpy(void* dest, const void* src, s21_size n) {
  char* d = dest;
  const char* s = src;

  for (s21_size i = 0; i < n; i++) {
    d[i] = s[i];
  }

  return dest;
}

void* s21_memmove(void* dest, const void* src, s21_size n) {
  char* d = dest;
  const char* s = src;

  if (d < s) {
    for (s21_size i = 0; i < n; i++) d[i] = s[i];
  } else if (d > s) {
    for (s21_size i = n; i > 0; i--) d[i - 1] = s[i - 1];
  }

  return dest;
}

void* s21_memset(void* str, int c, s21_size n) {
  unsigned char* s = str;

  for (s21_size i = 0; i < n; i++) {
    s[i] = (unsigned char)c;
  }

  return str;
}