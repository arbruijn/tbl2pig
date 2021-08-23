#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

void Error(char *msg, ...) {
    va_list vp;
    va_start(vp, msg);
    vfprintf(stderr, msg, vp);
    fprintf(stderr, "\n");
    va_end(vp);
    exit(1);
}

void _Assert(const char *expr, const char *file, int line) {
	Error("Assertion failed: %s on %s:%d", expr, file, line);
}
