#ifndef __WATCOMC__
#include <string.h>
#include "error.h"

void _splitpath(const char *path, char *drive, char *dir, char *file, char *ext) {
	const char *p;
	if (drive || dir || strchr(path, '/') || strchr(path, '\\') || strchr(path, ':'))
		Error("_splitpath unsupported arguments path=%s", path);
	if (!(p = strrchr(path, '.')))
		p = path + strlen(path);
	if (file) {
		memcpy(file, path, p - path);
		file[p - path] = 0;
	}
	if (ext)
		strcpy(ext, p);
}

void _makepath(char *path, const char *drive, const char *dir, const char *file, const char *ext) {
	if (drive || dir)
		Error("_makepath unsupported arguments");
	*path = 0;
	if (file)
		strcpy(path, file);
	if (ext && *ext) {
		if (*ext != '.')
			strcat(path, ".");
		strcat(path, ext);
	}
}
#endif
