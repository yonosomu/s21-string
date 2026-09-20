#include <check.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "../s21_string.h"

START_TEST(test_s21_strerror_known) {
    static const int known_codes[] = {
        0,
#ifdef EPERM
        EPERM,
#endif
#ifdef ENOENT
        ENOENT,
#endif
#ifdef ESRCH
        ESRCH,
#endif
#ifdef EINTR
        EINTR,
#endif
#ifdef EIO
        EIO,
#endif
#ifdef EBADF
        EBADF,
#endif
#ifdef EAGAIN
        EAGAIN,
#endif
#ifdef ENOMEM
        ENOMEM,
#endif
#ifdef EACCES
        EACCES,
#endif
#ifdef EINVAL
        EINVAL,
#endif
#ifdef ERANGE
        ERANGE,
#endif
#ifdef EDEADLK
        EDEADLK,
#endif
    };

    size_t count = sizeof(known_codes) / sizeof(known_codes[0]);

    for (size_t i = 0; i < count; i++) {
        ck_assert_ptr_nonnull(s21_strerror(known_codes[i]));
        ck_assert_str_eq(s21_strerror(known_codes[i]), strerror(known_codes[i]));
    }
}
END_TEST

START_TEST(test_s21_strerror_unknown) {
    int bad_codes[] = {
        -1,
        -999999,
        999999,
        INT_MAX,
        INT_MIN,
    };

    size_t count = sizeof(bad_codes) / sizeof(bad_codes[0]);

    for (size_t i = 0; i < count; i++) {
        ck_assert_ptr_nonnull(s21_strerror(bad_codes[i]));
        ck_assert_str_eq(s21_strerror(bad_codes[i]), strerror(bad_codes[i]));
    }
}
END_TEST

START_TEST(test_s21_strerror_stability) {
    ck_assert_ptr_nonnull(s21_strerror(EINVAL));
    ck_assert_ptr_eq(s21_strerror(EINVAL), s21_strerror(EINVAL));

    ck_assert_ptr_nonnull(s21_strerror(INT_MAX));
    ck_assert_ptr_eq(s21_strerror(INT_MAX), s21_strerror(INT_MAX));
}
END_TEST

Suite *s21_strerror_suite(void) {
    Suite *suite = suite_create("s21_strerror");
    TCase *tc = tcase_create("core");

    tcase_set_timeout(tc, 10);

    tcase_add_test(tc, test_s21_strerror_known);
    tcase_add_test(tc, test_s21_strerror_unknown);
    tcase_add_test(tc, test_s21_strerror_stability);

    suite_add_tcase(suite, tc);
    return suite;
}

int main(void) {
    Suite *suite = s21_strerror_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return failed == 0 ? 0 : 1;
}