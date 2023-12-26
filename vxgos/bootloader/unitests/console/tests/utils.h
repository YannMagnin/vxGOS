#ifndef UNITESTS_UTILS_H
#define UNITESTS_UTILS_H

/* _creq() : cr_assert_eq wrapper */
#define _creq(callable, val, msg, ...)                      \
    do {                                                    \
        __auto_type _a = callable;                          \
        cr_expect_eq(                                       \
            _a, val, "(%d != %d) "msg, _a, val              \
            __VA_OPT__(,) __VA_ARGS__                       \
        );                                                  \
    } while (0)

#endif /* UNITESTS_UTILS_H */
