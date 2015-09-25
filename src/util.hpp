#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <new>

#define DEFINE_GDB_PY_SCRIPT(script_name) \
    asm("\
        .pushsection \".debug_gdb_scripts\", \"MS\",@progbits,1\n\
        .byte 1 /* Python */\n\
        .asciz \"" script_name "\"\n\
        .popsection \n\
    ");

void panic(const char * str) __attribute__ ((noreturn));

// create<MyClass>(a, b) is equivalent to: new MyClass(a, b)
template<typename T, typename... Args>
static inline T * create(Args... args) {
    T * ptr = reinterpret_cast<T*>(malloc(sizeof(T)));
    if (!ptr)
        panic("allocate: out of memory");
    new (ptr) T(args...);
    return ptr;
}

// allocate<MyClass>(10) is equivalent to: new MyClass[10]
// calls the default constructor for each item in the array.
template<typename T>
static inline T * allocate(size_t count) {
    T * ptr = reinterpret_cast<T*>(malloc(count * sizeof(T)));
    if (!ptr)
        panic("allocate: out of memory");
    for (size_t i = 0; i < count; i++)
        new (&ptr[i]) T;
    return ptr;
}

// Pass in a pointer to an array of old_count items.
// You will get a pointer to an array of new_count items
// where the first old_count items will have the same bits as the array you passed in.
// Calls the default constructor on all the new elements.
// The returned pointer may not be equal to the given pointer,
// and no methods are called in the event of copying/moving the bits to a new buffer
// (no default constructor, no assignment operator, and no destructor, as you would expect from a manual implementation).
// This means you can't count on any pointers to these elements remaining valid after this call.
// If new_count is less than old_count, i.e. this is a shrinking operation, behavior is undefined.
// if old_count is 0, the old pointer may be null.
template<typename T>
static inline T * reallocate(T * old, size_t old_count, size_t new_count) {
    T * new_ptr = reinterpret_cast<T*>(realloc(old, new_count * sizeof(T)));
    if (!new_ptr)
        panic("reallocate: out of memory");
    for (size_t i = old_count; i < new_count; i++)
        new (&new_ptr[i]) T;
    return new_ptr;
}

// calls destructors and frees the memory.
// the count parameter is only used to call destructors of array elements.
// provide a count of 1 if this is not an array,
// or a count of 0 to skip the destructors.
// if count is 0, ptr may be null.
template<typename T>
static inline void destroy(T * ptr, size_t count) {
    for (size_t i = 0; i < count; i++)
        ptr[i].T::~T();
    free(ptr);
}


template <typename T>
static inline T min(T a, T b) {
    return a < b ? a : b;
}
template <typename T>
static inline T max(T a, T b) {
    return a < b ? b : a;
}
template <typename T>
static inline T clamp(T value, T min, T max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

template <typename T>
static inline int sign(T value) {
    if (value > 0)
        return 1;
    else if (value < 0)
        return -1;
    else
        return 0;
}
template <typename T>
static inline T euclidean_mod(T a, T base) {
    if (a < 0)
        return (a % base + base) % base;
    else
        return a % base;
}

#endif
