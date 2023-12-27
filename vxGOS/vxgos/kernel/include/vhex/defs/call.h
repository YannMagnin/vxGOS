#ifndef __VHEX_DEFS_CALL__
#define __VHEX_DEFS_CALL__ 1

#include <vhex/defs/types.h>

/* vhex_call_arg_t: All types of arguments allowed in an indirect call

   Because a function call cannot be easily pieced together, there are
   restrictions on what arguments can be passed. The following union lists all
   of the available types. Other types can be used if casted, mainly pointers;
   see the description of VHEX_CALL() for details. */
typedef union {
    /* 32-bit integers */
    int i;
    unsigned int u;
    int32_t i32;
    uint32_t u32;
    uintptr_t up32;
    intptr_t ip32;
    /* 32-bit floating-point */
    float f;

    /* Pointers to most common types, in all possible const/volatile
       qualifications */
    #define POINTER(type, name) \
    type *name; \
    type const *name ## _c; \
    type volatile *name ## _v; \
    type volatile const *name ## _cv;

    POINTER(void, pv)
    POINTER(char, pc)
    POINTER(unsigned char, puc)
    POINTER(short, ps)
    POINTER(unsigned short, pus)
    POINTER(int, pi)
    POINTER(unsigned int, pui)
    POINTER(int8_t, pi8)
    POINTER(uint8_t, pu8)
    POINTER(int16_t, pi16)
    POINTER(uint16_t, pu16)
    POINTER(int32_t, pi32)
    POINTER(uint32_t, pu32)
    POINTER(int64_t, pi64)
    POINTER(uint64_t, pu64)
    POINTER(long long int, pll)
    POINTER(unsigned long long int, pull)
    POINTER(float, pf)
    POINTER(double, pd)

    #undef POINTER
} vhex_call_arg_t;

/* vhex_call_t: Indirect call with up to 4 register arguments */
struct vhex_call {
    void *function;
    vhex_call_arg_t args[4];
};
typedef struct vhex_call vhex_call_t;

/* VHEX_CALL(): Build an callback from function and arguments */
#define VHEX_CALL(fct, ...)                                     \
    ((vhex_call_t){                                             \
        .function = (void*)fct,                                 \
        .args = { __VA_OPT__(VHEX_CALL_ARGS1(__VA_ARGS__)) }    \
    })
#define VHEX_CALL_ARGS1(a1, ...) \
    VHEX_CALL_ARG(a1), __VA_OPT__(VHEX_CALL_ARGS2(__VA_ARGS__))
#define VHEX_CALL_ARGS2(a2, ...) \
    VHEX_CALL_ARG(a2), __VA_OPT__(VHEX_CALL_ARGS3(__VA_ARGS__))
#define VHEX_CALL_ARGS3(a3, ...) \
    VHEX_CALL_ARG(a3), __VA_OPT__(VHEX_CALL_ARGS4(__VA_ARGS__))
#define VHEX_CALL_ARGS4(a4, ...) \
    ({                                                      \
        __VA_OPT__(                                         \
            _Static_assert(                                 \
                0, \                                        \
                "VHEX_CALL: too many arguments (maximum 4)" \
            );                                              \
        )                                                   \
        VHEX_CALL_ARG(a4)                                   \
    })

#ifdef __cplusplus
 /* use Lephe' tricks to avoid ALE linter error */
 #define VHEX_CALL_ARG(expr) ({                     \
    auto __arg = (expr);                            \
    (*reinterpret_cast<vhex_call_arg_t *>(&__arg)); \
 })
#else
 /* GCC extension: Cast to union */
 #define VHEX_CALL_ARG(expr) (vhex_call_arg_t)(expr)
#endif

/* VHEX_CALL_NULL: Empty function call */
#define VHEX_CALL_NULL ((vhex_call_t){ .function = NULL, .args = {} })

/* vhex_call(): Perform an indirect call */
static VINLINE int vhex_call(vhex_call_t callback)
{
    return ((int(*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t))callback.function)(
        callback.args[0].up32,
        callback.args[1].up32,
        callback.args[2].up32,
        callback.args[3].up32
    );
}

//---
// Predefined indirect calls
//
// * VHEX_CALL_SET(pointer_to_int) will create an indirect call that sets
//   (*pointer_to_int) to 1 when invoked. Returns 0.
//
// * VHEX_CALL_INC(pointer_to_int) will create an indirect call that increments
//   (*pointer_to_int) when invoked. Returns 0.
//
// * VHEX_CALL_SET_STOP(pointer_to_int) is like VHEX_CALL_SET() but it returns
//   1 (TIMER_STOP) so it can be used to set a pointer just once in a timer.
//
// * VHEX_CALL_INC_STOP(pointer_to_int) similarly returns 1 (TIMER_STOP).
//---

/* VHEX_CALL_SET(): Callback that sets an integer to 1
   This is defined as a function to make sure the pointer is to an int. */
VUNUSED static int VHEX_CALL_SET_function(int volatile *pointer)
{
    (*pointer) = 1;
    return 0;
}
#define VHEX_CALL_SET(pointer) \
    VHEX_CALL(&VHEX_CALL_SET_function, pointer)

/* VHEX_CALL_INC(): Callback that increments an integer */
VUNUSED static int VHEX_CALL_INC_function(int volatile *pointer)
{
    (*pointer)++;
    return 0;
}
#define VHEX_CALL_INC(pointer) \
    VHEX_CALL(&VHEX_CALL_INC_function, pointer)

/* VHEX_CALL_SET_STOP(): Same as VHEX_CALL_SET(), but returns TIMER_STOP */
VUNUSED static int VHEX_CALL_SET_STOP_function(int volatile *pointer)
{
    (*pointer) = 1;
    return 1;
}
#define VHEX_CALL_SET_STOP(pointer) \
    VHEX_CALL(&VHEX_CALL_SET_STOP_function, pointer)

/* VHEX_CALL_INC(): Callback that increments an integer */
VUNUSED static int VHEX_CALL_INC_STOP_function(int volatile *pointer)
{
    (*pointer)++;
    return 1;
}
#define VHEX_CALL_INC_STOP(pointer) \
    VHEX_CALL(&VHEX_CALL_INC_STOP_function, pointer)

#endif /* __VHEX_DEFS_CALL__ */
