#include <check.h>
#include <string.h>

#include "../s21_string.h"

#define TOK_BUF_SIZE 4096

// Вспомогательные функции для сравнения с эталонной библиотекой

static void assert_strchr_eq(const char* s, int c) {
  const char* expected = strchr(s, c);
  const char* actual = s21_strchr(s, c);
  ck_assert_ptr_eq(expected, actual);
}

static void assert_strrchr_eq(const char* s, int c) {
  const char* expected = strrchr(s, c);
  const char* actual = s21_strrchr(s, c);
  ck_assert_ptr_eq(expected, actual);
}

static void assert_strstr_eq(const char* haystack, const char* needle) {
  const char* expected = strstr(haystack, needle);
  const char* actual = s21_strstr(haystack, needle);
  ck_assert_ptr_eq(expected, actual);
}

static void assert_strspn_eq(const char* s, const char* accept) {
  size_t expected = strspn(s, accept);
  s21_size actual = s21_strspn(s, accept);
  ck_assert_uint_eq((unsigned long)expected, (unsigned long)actual);
}

static void assert_strcspn_eq(const char* s, const char* reject) {
  size_t expected = strcspn(s, reject);
  s21_size actual = s21_strcspn(s, reject);
  ck_assert_uint_eq((unsigned long)expected, (unsigned long)actual);
}

static void assert_strpbrk_eq(const char* s, const char* accept) {
  const char* expected = strpbrk(s, accept);
  const char* actual = s21_strpbrk(s, accept);
  ck_assert_ptr_eq(expected, actual);
}

static void assert_strtok_seq(const char* input, const char* delim) {
  char expected[TOK_BUF_SIZE];
  char actual[TOK_BUF_SIZE];

  size_t len = strlen(input);
  ck_assert(len < TOK_BUF_SIZE);

  strcpy(expected, input);
  strcpy(actual, input);

  char* e = strtok(expected, delim);
  char* a = s21_strtok(actual, delim);

  while (e != NULL || a != NULL) {
    ck_assert_int_eq(e != NULL, a != NULL);

    if (e != NULL && a != NULL) {
      ck_assert_str_eq(e, a);
    }

    e = strtok(NULL, delim);
    a = s21_strtok(NULL, delim);
  }
}

// Тесты

START_TEST(test_s21_strchr) {
  assert_strchr_eq("", 'a');
  assert_strchr_eq("", '\0');
  assert_strchr_eq("a", 'a');
  assert_strchr_eq("a", 'b');
  assert_strchr_eq("hello", 'h');
  assert_strchr_eq("hello", 'l');
  assert_strchr_eq("hello", 'x');
  assert_strchr_eq("hello", '\0');

  ck_assert_ptr_eq(s21_strchr("abc", 256), s21_strchr("abc", '\0'));
  ck_assert_ptr_null(s21_strchr("abc", 128));
}
END_TEST

START_TEST(test_s21_strrchr) {
  assert_strrchr_eq("", 'a');
  assert_strrchr_eq("banana", 'a');
  assert_strrchr_eq("banana", 'b');
  assert_strrchr_eq("banana", 'x');
  assert_strrchr_eq("banana", '\0');

  ck_assert_ptr_eq(s21_strrchr("abc", 256), s21_strrchr("abc", '\0'));
  ck_assert_ptr_null(s21_strrchr("abc", 128));
}
END_TEST

START_TEST(test_s21_strstr) {
  assert_strstr_eq("", "");
  assert_strstr_eq("abc", "");
  assert_strstr_eq("abc", "a");
  assert_strstr_eq("abc", "abc");
  assert_strstr_eq("abc", "abcd");
  assert_strstr_eq("mississippi", "issip");
  assert_strstr_eq("aaaa", "aa");
  assert_strstr_eq("abc\0def", "def");
}
END_TEST

START_TEST(test_s21_strspn) {
  assert_strspn_eq("", "abc");
  assert_strspn_eq("abc", "");
  assert_strspn_eq("abc", "a");
  assert_strspn_eq("abc", "abc");
  assert_strspn_eq("abc123", "abc");
  assert_strspn_eq("aaaa", "b");
}
END_TEST

START_TEST(test_s21_strcspn) {
  assert_strcspn_eq("", "abc");
  assert_strcspn_eq("abc", "");
  assert_strcspn_eq("abc", "a");
  assert_strcspn_eq("abc", "b");
  assert_strcspn_eq("abc123", "1");
  assert_strcspn_eq("aaaa", "b");
}
END_TEST

START_TEST(test_s21_strpbrk) {
  assert_strpbrk_eq("", "abc");
  assert_strpbrk_eq("abc", "");
  assert_strpbrk_eq("abc", "a");
  assert_strpbrk_eq("abc", "xyz");
  assert_strpbrk_eq("hello", "aeiou");
  assert_strpbrk_eq("abc\0def", "d");
}
END_TEST

START_TEST(test_s21_strtok) {
  assert_strtok_seq("a,b,c", ",");
  assert_strtok_seq(",,a,b,c,,", ",");
  assert_strtok_seq("", ",");
  assert_strtok_seq(",,,", ",");
  assert_strtok_seq("abc", ",");
  assert_strtok_seq("a,b;c", ",;");
  assert_strtok_seq("a b  c", " ");
  assert_strtok_seq("aaaa", "a");
  assert_strtok_seq("abcde", "edcba");
}
END_TEST

Suite* s21_string_suite(void) {
  Suite* suite = suite_create("s21_string_search");
  TCase* tc = tcase_create("core");

  tcase_set_timeout(tc, 10);

  tcase_add_test(tc, test_s21_strchr);
  tcase_add_test(tc, test_s21_strrchr);
  tcase_add_test(tc, test_s21_strstr);
  tcase_add_test(tc, test_s21_strspn);
  tcase_add_test(tc, test_s21_strcspn);
  tcase_add_test(tc, test_s21_strpbrk);
  tcase_add_test(tc, test_s21_strtok);

  suite_add_tcase(suite, tc);
  return suite;
}

int main(void) {
  Suite* suite = s21_string_suite();
  SRunner* runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return failed == 0 ? 0 : 1;
}