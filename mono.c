#include <stdarg.h>
#include <stdio.h>
#include "mono.h"

void verbose_printf(int lvl, const char *msg, ...) {
    va_list vp;
    va_start(vp, msg);
    vprintf(msg, vp);
    va_end(vp);
}
