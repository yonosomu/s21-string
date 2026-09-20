#include "s21_sprintf.h"

#include <check.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define BUFFER_SIZE 1024

typedef struct {
    int total;
    int failed;
} SprintfResult;

typedef struct {
    const char *format;
    int value;
    const char *name;
} IntTest;

typedef struct {
    const char *format;
    unsigned int value;
    const char *name;
} UIntTest;

typedef struct {
    const char *format;
    double value;
    const char *name;
} DoubleTest;

typedef struct {
    const char *format;
    const char *value;
    const char *name;
} StringTest;

typedef struct {
    const char *format;
    char value;
    const char *name;
} CharTest;



SprintfResult result_init() {
    SprintfResult result = {0, 0};
    return result;
}

void result_add(SprintfResult *result, int failed) {
    result->total++;
    if (failed) result->failed++;
}

SprintfResult result_merge(SprintfResult first, SprintfResult second) {
    SprintfResult result;
    result.total = first.total + second.total;
    result.failed = first.failed + second.failed;
    return result;
}



int compare_result(const char *name,
                    int expected_ret,
                    int actual_ret,
                    const char *expected,
                    const char *actual) {
    int failed = 0;
    if (expected_ret != actual_ret ||
        memcmp(expected, actual, (size_t)expected_ret + 1) != 0) {
        printf("FAIL: %s\n", name);
        printf("  expected return: %d\n", expected_ret);
        printf("  actual return:   %d\n", actual_ret);
        printf("  expected: \"%s\"\n", expected);
        printf("  actual:   \"%s\"\n\n", actual);
        failed = 1;
    }
    return failed;
}

static int run_int_test(const IntTest *test) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    int expected_ret = sprintf(expected, test->format, test->value);
    int actual_ret = s21_sprintf(actual, test->format, test->value);
    return compare_result(test->name, expected_ret, actual_ret,
                          expected, actual);
}

static int run_uint_test(const UIntTest *test) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    int expected_ret = sprintf(expected, test->format, test->value);
    int actual_ret = s21_sprintf(actual, test->format, test->value);
    return compare_result(test->name, expected_ret, actual_ret,
                          expected, actual);
}

static int run_double_test(const DoubleTest *test) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    int expected_ret = sprintf(expected, test->format, test->value);
    int actual_ret = s21_sprintf(actual, test->format, test->value);
    return compare_result(test->name, expected_ret, actual_ret,
                          expected, actual);
}

static int run_string_test(const StringTest *test) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    int expected_ret = sprintf(expected, test->format, test->value);
    int actual_ret = s21_sprintf(actual, test->format, test->value);
    return compare_result(test->name, expected_ret, actual_ret,
                          expected, actual);
}

static int run_char_test(const CharTest *test) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    int expected_ret = sprintf(expected, test->format, test->value);
    int actual_ret = s21_sprintf(actual, test->format, test->value);
    return compare_result(test->name, expected_ret, actual_ret,
                          expected, actual);
}


// %c

SprintfResult test_char_basic(void) {
    const CharTest tests[] = {
        {"%c", 'A', "c: letter"},
        {"%c", '0', "c: digit"},
        {"%c", ' ', "c: space"},
        {"%c", '\n', "c: newline"},
        {"%c", '!', "c: punctuation"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_char_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_char_width(void) {
    const CharTest tests[] = {
        {"%1c", 'A', "c: width 1"},
        {"%3c", 'A', "c: width 3"},
        {"%5c", 'A', "c: width 5"},
        {"%10c", 'A', "c: width 10"},
        {"%-3c", 'A', "c: left width 3"},
        {"%-10c", 'A', "c: left width 10"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_char_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_char(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_char_basic());
    result = result_merge(result, test_char_width());

    return result;
}


// %d

static SprintfResult test_decimal_values(void) {
    const IntTest tests[] = {
        {"%d", 0, "d: zero"},
        {"%d", 1, "d: one"},
        {"%d", -1, "d: minus one"},
        {"%d", 42, "d: 42"},
        {"%d", -42, "d: minus 42"},
        {"%d", 123456789, "d: large"},
        {"%d", INT_MAX, "d: INT_MAX"},
        {"%d", INT_MIN, "d: INT_MIN"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_decimal_flags(void) {
    const IntTest tests[] = {
        {"%+d", 42, "d: plus positive"},
        {"%+d", -42, "d: plus negative"},
        {"% d", 42, "d: space positive"},
        {"% d", -42, "d: space negative"},
        {"%-d", 42, "d: minus"},
        {"%+-d", 42, "d: plus minus"},
        {"%- d", 42, "d: minus space"},
        {"%+ d", 42, "d: plus space"},
        {"%-+ d", 42, "d: all flags"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_decimal_width(void) {
    const IntTest tests[] = {
        {"%1d", 42, "d: width 1"},
        {"%2d", 42, "d: width 2"},
        {"%5d", 42, "d: width 5"},
        {"%10d", 42, "d: width 10"},
        {"%10d", -42, "d: negative width"},
        {"%-5d", 42, "d: left width 5"},
        {"%-10d", 42, "d: left width 10"},
        {"%5d", 123456, "d: value wider than width"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_decimal_precision(void) {
    const IntTest tests[] = {
        {"%.0d", 0, "d: zero precision zero"},
        {"%.0d", 42, "d: precision zero"},
        {"%.1d", 42, "d: precision 1"},
        {"%.2d", 42, "d: precision 2"},
        {"%.5d", 42, "d: precision 5"},
        {"%.10d", 42, "d: precision 10"},
        {"%5.2d", 42, "d: width precision"},
        {"%10.5d", 42, "d: large width precision"},
        {"%-10.5d", 42, "d: minus width precision"},
        {"%+10.5d", 42, "d: plus width precision"},
        {"% 10.5d", 42, "d: space width precision"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_decimal_length(void) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    SprintfResult result = result_init();

    short h_value = -12345;
    long l_value = -1234567890L;
    int expected_ret;
    int actual_ret;

    expected_ret = sprintf(expected, "%hd", h_value);
    actual_ret = s21_sprintf(actual, "%hd", h_value);
    result_add(&result, compare_result("d: short",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%hu", (unsigned short)54321);
    actual_ret = s21_sprintf(actual, "%hu", (unsigned short)54321);
    result_add(&result, compare_result("u: unsigned short",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%ld", l_value);
    actual_ret = s21_sprintf(actual, "%ld", l_value);
    result_add(&result, compare_result("d: long",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%lu",
                           (unsigned long)4000000000UL);
    actual_ret = s21_sprintf(actual, "%lu",
                             (unsigned long)4000000000UL);
    result_add(&result, compare_result("u: unsigned long",
                                       expected_ret, actual_ret,
                                       expected, actual));

    return result;
}

static SprintfResult test_decimal(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_decimal_values());
    result = result_merge(result, test_decimal_flags());
    result = result_merge(result, test_decimal_width());
    result = result_merge(result, test_decimal_precision());
    result = result_merge(result, test_decimal_length());

    return result;
}


// %u

static SprintfResult test_unsigned_values(void) {
    const UIntTest tests[] = {
        {"%u", 0U, "u: zero"},
        {"%u", 1U, "u: one"},
        {"%u", 42U, "u: 42"},
        {"%u", 123456789U, "u: large"},
        {"%u", UINT_MAX, "u: UINT_MAX"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_uint_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_unsigned_width(void) {
    const UIntTest tests[] = {
        {"%1u", 42U, "u: width 1"},
        {"%5u", 42U, "u: width 5"},
        {"%10u", 42U, "u: width 10"},
        {"%-5u", 42U, "u: left width 5"},
        {"%-10u", 42U, "u: left width 10"},
        {"%5u", 123456U, "u: value wider than width"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_uint_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_unsigned_precision(void) {
    const UIntTest tests[] = {
        {"%.0u", 0U, "u: zero precision zero"},
        {"%.0u", 42U, "u: precision zero"},
        {"%.1u", 42U, "u: precision 1"},
        {"%.5u", 42U, "u: precision 5"},
        {"%.10u", 42U, "u: precision 10"},
        {"%10.5u", 42U, "u: width precision"},
        {"%-10.5u", 42U, "u: minus width precision"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_uint_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_unsigned(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_unsigned_values());
    result = result_merge(result, test_unsigned_width());
    result = result_merge(result, test_unsigned_precision());

    return result;
}


// %f

static SprintfResult test_float_values(void) {
    const DoubleTest tests[] = {
        {"%f", 0.0, "f: zero"},
        {"%f", -0.0, "f: negative zero"},
        {"%f", 1.0, "f: one"},
        {"%f", -1.0, "f: minus one"},
        {"%f", 1.5, "f: 1.5"},
        {"%f", -1.5, "f: minus 1.5"},
        {"%f", 3.141592653589793, "f: pi"},
        {"%f", 12345.6789, "f: large"},
        {"%f", -12345.6789, "f: negative large"},
        {"%f", 0.0000001, "f: small"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_float_precision(void) {
    const DoubleTest tests[] = {
        {"%.0f", 3.14159, "f: precision 0"},
        {"%.1f", 3.14159, "f: precision 1"},
        {"%.2f", 3.14159, "f: precision 2"},
        {"%.3f", 3.14159, "f: precision 3"},
        {"%.6f", 3.14159, "f: precision 6"},
        {"%.10f", 3.14159, "f: precision 10"},
        {"%.20f", 3.14159, "f: precision 20"},
        {"%.2f", -3.14159, "f: negative rounding"},
        {"%.2f", 999.999, "f: rounding"},
        {"%.0f", 999.999, "f: rounding zero precision"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_float_width(void) {
    const DoubleTest tests[] = {
        {"%1f", 3.14, "f: width 1"},
        {"%5f", 3.14, "f: width 5"},
        {"%10f", 3.14, "f: width 10"},
        {"%-10f", 3.14, "f: left width"},
        {"%10.2f", 3.14, "f: width precision"},
        {"%-10.2f", 3.14, "f: left width precision"},
        {"%10.2f", -3.14, "f: negative width precision"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_float_flags(void) {
    const DoubleTest tests[] = {
        {"%+f", 3.14, "f: plus"},
        {"%+f", -3.14, "f: plus negative"},
        {"% f", 3.14, "f: space"},
        {"% f", -3.14, "f: space negative"},
        {"%+10.2f", 3.14, "f: plus width precision"},
        {"% 10.2f", 3.14, "f: space width precision"},
        {"%-+10.2f", 3.14, "f: minus plus"},
        {"%- 10.2f", 3.14, "f: minus space"},
        {"%-+ 10.2f", 3.14, "f: all flags"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_float_length(void) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    SprintfResult result = result_init();

    int expected_ret = sprintf(expected, "%lf", 3.14159);
    int actual_ret = s21_sprintf(actual, "%lf", 3.14159);

    result_add(&result, compare_result("f: l",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%10.2lf", 3.14159);
    actual_ret = s21_sprintf(actual, "%10.2lf", 3.14159);

    result_add(&result, compare_result("f: l width precision",
                                       expected_ret, actual_ret,
                                       expected, actual));

    return result;
}

static SprintfResult test_float(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_float_values());
    result = result_merge(result, test_float_precision());
    result = result_merge(result, test_float_width());
    result = result_merge(result, test_float_flags());
    result = result_merge(result, test_float_length());

    return result;
}


// %s

static SprintfResult test_string_values(void) {
    const StringTest tests[] = {
        {"%s", "", "s: empty"},
        {"%s", "a", "s: one character"},
        {"%s", "abc", "s: abc"},
        {"%s", "hello", "s: hello"},
        {"%s", "hello world", "s: spaces"},
        {"%s", "1234567890", "s: digits"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_string_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_string_width(void) {
    const StringTest tests[] = {
        {"%1s", "abc", "s: width 1"},
        {"%3s", "abc", "s: width 3"},
        {"%5s", "abc", "s: width 5"},
        {"%10s", "abc", "s: width 10"},
        {"%-5s", "abc", "s: left width 5"},
        {"%-10s", "abc", "s: left width 10"},
        {"%2s", "abcdef", "s: value wider than width"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_string_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_string_precision(void) {
    const StringTest tests[] = {
        {"%.0s", "abcdef", "s: precision 0"},
        {"%.1s", "abcdef", "s: precision 1"},
        {"%.2s", "abcdef", "s: precision 2"},
        {"%.3s", "abcdef", "s: precision 3"},
        {"%.10s", "abc", "s: precision greater"},
        {"%10.3s", "abcdef", "s: width precision"},
        {"%-10.3s", "abcdef", "s: left width precision"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_string_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_string(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_string_values());
    result = result_merge(result, test_string_width());
    result = result_merge(result, test_string_precision());

    return result;
}


// %%

static SprintfResult test_percent(void) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    SprintfResult result = result_init();
    int expected_ret;
    int actual_ret;

    expected_ret = sprintf(expected, "%%");
    actual_ret = s21_sprintf(actual, "%%");

    result_add(&result, compare_result("percent: basic",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "100%%");
    actual_ret = s21_sprintf(actual, "100%%");

    result_add(&result, compare_result("percent: text",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%%%%");
    actual_ret = s21_sprintf(actual, "%%%%");

    result_add(&result, compare_result("percent: four percent",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "abc%%def");
    actual_ret = s21_sprintf(actual, "abc%%def");

    result_add(&result, compare_result("percent: inside text",
                                       expected_ret, actual_ret,
                                       expected, actual));

    return result;
}


// Other

static SprintfResult test_mixed_formats(void) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    SprintfResult result = result_init();
    int expected_ret;
    int actual_ret;

    expected_ret = sprintf(expected,
                           "int=%d str=%s float=%.2f char=%c uint=%u",
                           -42, "hello", 3.14, 'A', 100U);

    actual_ret = s21_sprintf(actual,
                             "int=%d str=%s float=%.2f char=%c uint=%u",
                             -42, "hello", 3.14, 'A', 100U);

    result_add(&result, compare_result("mixed: all specifiers",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%d %d %d %d",
                           1, 2, 3, 4);

    actual_ret = s21_sprintf(actual, "%d %d %d %d",
                             1, 2, 3, 4);

    result_add(&result, compare_result("mixed: multiple d",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%s %s %s",
                           "one", "two", "three");

    actual_ret = s21_sprintf(actual, "%s %s %s",
                             "one", "two", "three");

    result_add(&result, compare_result("mixed: multiple s",
                                       expected_ret, actual_ret,
                                       expected, actual));

    return result;
}

static SprintfResult test_text_around_specifiers(void) {
    char expected[BUFFER_SIZE];
    char actual[BUFFER_SIZE];
    SprintfResult result = result_init();
    int expected_ret;
    int actual_ret;

    expected_ret = sprintf(expected, "abc%d", 42);
    actual_ret = s21_sprintf(actual, "abc%d", 42);

    result_add(&result, compare_result("text: before specifier",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "%dabc", 42);
    actual_ret = s21_sprintf(actual, "%dabc", 42);

    result_add(&result, compare_result("text: after specifier",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "abc%ddef", 42);
    actual_ret = s21_sprintf(actual, "abc%ddef", 42);

    result_add(&result, compare_result("text: both sides",
                                       expected_ret, actual_ret,
                                       expected, actual));

    expected_ret = sprintf(expected, "abc%d%%def%s",
                           42, "test");

    actual_ret = s21_sprintf(actual, "abc%d%%def%s",
                             42, "test");

    result_add(&result, compare_result("text: mixed",
                                       expected_ret, actual_ret,
                                       expected, actual));

    return result;
}

static SprintfResult test_combinations(void) {
    const IntTest tests[] = {
        {"%+10.5d", 42, "combo: d plus"},
        {"%-10.5d", 42, "combo: d minus"},
        {"% 10.5d", 42, "combo: d space"},
        {"%-+10.5d", 42, "combo: d minus plus"},
        {"%- 10.5d", 42, "combo: d minus space"},
        {"%+ 10.5d", 42, "combo: d plus space"},
        {"%-+ 10.5d", 42, "combo: d all flags"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_float_combinations(void) {
    const DoubleTest tests[] = {
        {"%+10.3f", 12.3456, "combo: f plus"},
        {"%-10.3f", 12.3456, "combo: f minus"},
        {"% 10.3f", 12.3456, "combo: f space"},
        {"%-+10.3f", 12.3456, "combo: f minus plus"},
        {"%- 10.3f", 12.3456, "combo: f minus space"},
        {"%+ 10.3f", 12.3456, "combo: f plus space"},
        {"%-+ 10.3f", 12.3456, "combo: f all flags"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_edge_decimal(void) {
    const IntTest tests[] = {
        {"%d", INT_MIN, "edge: INT_MIN"},
        {"%d", INT_MAX, "edge: INT_MAX"},
        {"%.0d", 0, "edge: zero precision"},
        {"%.20d", 42, "edge: large precision"},
        {"%2d", 123456789, "edge: value > width"},
        {"%20d", 1, "edge: width 20"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_int_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_edge_unsigned(void) {
    const UIntTest tests[] = {
        {"%u", 0U, "edge: unsigned zero"},
        {"%u", UINT_MAX, "edge: UINT_MAX"},
        {"%.0u", 0U, "edge: unsigned zero precision"},
        {"%.20u", 42U, "edge: unsigned large precision"},
        {"%2u", UINT_MAX, "edge: unsigned > width"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_uint_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_edge_float(void) {
    const DoubleTest tests[] = {
        {"%.0f", 0.0, "edge: float zero"},
        {"%.0f", -0.0, "edge: negative zero"},
        {"%.20f", 3.141592653589793, "edge: high precision"},
        {"%.10f", 0.0000001, "edge: small number"},
        {"%.2f", 999999.999, "edge: rounding"},
        {"%.2f", -999999.999, "edge: negative rounding"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_double_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_edge_string(void) {
    const StringTest tests[] = {
        {"%20s", "", "edge: empty string width"},
        {"%-20s", "", "edge: empty string left width"},
        {"%.0s", "hello", "edge: string precision zero"},
        {"%.20s", "hello", "edge: precision > string"},
        {"%2s", "very long string", "edge: string > width"}
    };

    SprintfResult result = result_init();
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        result_add(&result, run_string_test(&tests[i]));
    }

    return result;
}

static SprintfResult test_edges(void) {
    SprintfResult result = result_init();

    result = result_merge(result, test_edge_decimal());
    result = result_merge(result, test_edge_unsigned());
    result = result_merge(result, test_edge_float());
    result = result_merge(result, test_edge_string());

    return result;
}



static void print_summary(SprintfResult result) {
    printf("\n");
    printf("========================================\n");
    printf("        s21_sprintf TEST RESULTS\n");
    printf("========================================\n");
    printf("Total tests: %d\n", result.total);
    printf("Passed:      %d\n", result.total - result.failed);
    printf("Failed:      %d\n", result.failed);
    printf("========================================\n");
}


START_TEST(test_existing_sprintf) {
    SprintfResult result = result_init();

    result = result_merge(result, test_char());
    result = result_merge(result, test_decimal());
    result = result_merge(result, test_unsigned());
    result = result_merge(result, test_float());
    result = result_merge(result, test_string());
    result = result_merge(result, test_percent());
    result = result_merge(result, test_mixed_formats());
    result = result_merge(result, test_text_around_specifiers());
    result = result_merge(result, test_combinations());
    result = result_merge(result, test_float_combinations());
    result = result_merge(result, test_edges());

    print_summary(result);

    ck_assert_int_eq(result.failed, 0);
}
END_TEST

int main(void) {
    Suite *suite = suite_create("s21_sprintf_existing");
    TCase *tc = tcase_create("formats");
    tcase_add_test(tc, test_existing_sprintf);
    suite_add_tcase(suite, tc);
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed != 0;
}
