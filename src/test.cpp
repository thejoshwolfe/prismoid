
#include <stdio.h>
#include <stdlib.h>

#define fail(msg) _fail(__FILE__, __LINE__, __func__, (msg))
static void _fail(const char * filename, int line_number, const char * function_name, const char * msg) {
    // gcc compile error format, so that eclipse will give us red squiggly underlines where the fail was called.
    fprintf(stderr, "%s:%d: %s: %s\n", filename, line_number, function_name, msg);
    abort();
}

#define assert(condition) _assert((condition), __FILE__, __LINE__, __func__)
static void _assert(bool condition, const char * filename, int line_number, const char * function_name) {
    if (!condition)
        _fail(filename, line_number, function_name, "assertion failure");
}

int main() {
    assert(true);
}
