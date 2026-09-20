#include <check.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../s21_string.h"

#define ASSERT_FORMAT(format, ...)                                             \
  do {                                                                         \
    char expected[4096] = {0}, actual[4096] = {0};                             \
    int want = sprintf(expected, format, __VA_ARGS__);                         \
    int got = s21_sprintf(actual, format, __VA_ARGS__);                        \
    ck_assert_int_eq(got, want);                                               \
    ck_assert_msg(memcmp(actual, expected, (size_t)want + 1) == 0,             \
                  "format %s: expected %s, got %s", format, expected, actual); \
  } while (0)

START_TEST(test_memory) {
  unsigned char input[] = {0, 1, 127, 128, 255, 1};
  for (size_t n = 0; n <= sizeof(input); ++n) {
    for (int c = -1; c <= 256; ++c)
      ck_assert_ptr_eq(s21_memchr(input, c, n), memchr(input, c, n));
    unsigned char a[16] = {0}, b[16] = {0};
    ck_assert_ptr_eq(s21_memcpy(a, input, n), a);
    memcpy(b, input, n);
    ck_assert_int_eq(memcmp(a, b, sizeof(a)), 0);
    ck_assert_int_eq(s21_memcmp(input, input, n), 0);
    ck_assert_ptr_eq(s21_memset(a, 511, n), a);
    memset(b, 511, n);
    ck_assert_int_eq(memcmp(a, b, sizeof(a)), 0);
  }
  unsigned char high[] = {255}, low[] = {1};
  ck_assert_int_gt(s21_memcmp(high, low, 1), 0);
  ck_assert_int_lt(s21_memcmp(low, high, 1), 0);
  for (int shift = -2; shift <= 2; ++shift) {
    char a[] = "0123456789", b[] = "0123456789";
    ck_assert_ptr_eq(s21_memmove(a + 2 + shift, a + 2, 6), a + 2 + shift);
    memmove(b + 2 + shift, b + 2, 6);
    ck_assert_int_eq(memcmp(a, b, sizeof(a)), 0);
  }
}
END_TEST

START_TEST(test_copy_compare) {
  const char *strings[] = {"", "abc", "abcdef", "abcX", "\xff", "\x01"};
  for (size_t i = 0; i < sizeof(strings) / sizeof(*strings); ++i) {
    ck_assert_uint_eq(s21_strlen(strings[i]), strlen(strings[i]));
    char a[32] = {0}, b[32] = {0};
    ck_assert_ptr_eq(s21_strcpy(a, strings[i]), a);
    strcpy(b, strings[i]);
    ck_assert_str_eq(a, b);
    ck_assert_ptr_eq(s21_strcat(a, strings[i]), a);
    strcat(b, strings[i]);
    ck_assert_str_eq(a, b);
    for (size_t n = 0; n < 9; ++n) {
      memset(a, '!', sizeof(a));
      memset(b, '!', sizeof(b));
      ck_assert_ptr_eq(s21_strncpy(a, strings[i], n), a);
      strncpy(b, strings[i], n);
      ck_assert_int_eq(memcmp(a, b, sizeof(a)), 0);
      strcpy(a, "prefix");
      strcpy(b, "prefix");
      ck_assert_ptr_eq(s21_strncat(a, strings[i], n), a);
      strncat(b, strings[i], n);
      ck_assert_str_eq(a, b);
      for (size_t j = 0; j < sizeof(strings) / sizeof(*strings); ++j) {
        int want = strncmp(strings[i], strings[j], n);
        int got = s21_strncmp(strings[i], strings[j], n);
        ck_assert_int_eq((got > 0) - (got < 0), (want > 0) - (want < 0));
        want = strcmp(strings[i], strings[j]);
        got = s21_strcmp(strings[i], strings[j]);
        ck_assert_int_eq((got > 0) - (got < 0), (want > 0) - (want < 0));
      }
    }
  }
}
END_TEST

START_TEST(test_error_table) {
  for (int i = -10; i <= 150; ++i)
    ck_assert_str_eq(s21_strerror(i), strerror(i));
}
END_TEST

START_TEST(test_sprintf_regressions) {
  ASSERT_FORMAT("a%cafter", 0);
  ASSERT_FORMAT("a%5cafter", 0);
  ASSERT_FORMAT("a%-5cafter", 0);
  ASSERT_FORMAT("%hd", INT_MAX);
  ASSERT_FORMAT("%hu", 65537);
  ASSERT_FORMAT("%ld", LONG_MIN);
  ASSERT_FORMAT("%lu", ULONG_MAX);
  ASSERT_FORMAT("%.3d", -12);
  ASSERT_FORMAT("%+.0d", 0);
  ASSERT_FORMAT("% .0d", 0);
  char unterminated[] = {'a', 'b', 'c'};
  ASSERT_FORMAT("%.3s", unterminated);
  ASSERT_FORMAT("%.0s", unterminated);
  double values[] = {
      0,       -0.0,         1.5,      -1.5,      2.5,   -2.5,   9.5,
      -9.5,    0.125,        0.375,    -0.125,    9.999, -9.999, DBL_MAX,
      DBL_MIN, DBL_TRUE_MIN, INFINITY, -INFINITY, NAN,   -NAN};
  const char *formats[] = {"%.0f", "%.2f", "%+20.6f", "%- 25.20f", "%.350f"};
  for (size_t i = 0; i < sizeof(values) / sizeof(*values); ++i)
    for (size_t j = 0; j < sizeof(formats) / sizeof(*formats); ++j)
      ASSERT_FORMAT(formats[j], values[i]);
}
END_TEST

START_TEST(test_float_differential) {
  uint64_t state = 123456789;
  for (int i = 0; i < 1000; ++i) {
    state = state * UINT64_C(6364136223846793005) + 1;
    double value;
    memcpy(&value, &state, sizeof(value));
    if (isfinite(value)) {
      char format[32];
      sprintf(format, "%%+30.%df", i % 25);
      ASSERT_FORMAT(format, value);
    }
  }
}
END_TEST

int main(void) {
  Suite *suite = suite_create("s21_core");
  TCase *tc = tcase_create("core");
  tcase_set_timeout(tc, 30);
  tcase_add_test(tc, test_memory);
  tcase_add_test(tc, test_copy_compare);
  tcase_add_test(tc, test_error_table);
  tcase_add_test(tc, test_sprintf_regressions);
  tcase_add_test(tc, test_float_differential);
  suite_add_tcase(suite, tc);
  SRunner *runner = srunner_create(suite);
  srunner_run_all(runner, CK_NORMAL);
  int failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failed != 0;
}
