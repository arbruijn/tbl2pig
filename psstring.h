#ifndef PSSTRING_H_
#define PSSTRING_H_

#ifndef __WATCOMC__
#include <ctype.h>
#define stricmp(a, b) strcasecmp(a, b)
#define strnicmp(a, b, n) strncasecmp(a, b, n)
#define itoa(n, buf, size) (snprintf(buf, size, "%d", n), buf)
#define strrev(a) do { char *s = a; int l = strlen(s); char *e = s + l; l >>= 1; while (l--) { char t = *--e; *e = *s; *s++ = t; } } while (0)
#if !defined(__EMSCRIPTEN__) && !defined(__WATCOMC__)
#define strlwr(a) for (char *p = a; *p; p++) *p = tolower(*p);
#endif
#endif

#endif
