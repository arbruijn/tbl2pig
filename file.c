#ifndef __WATCOMC__

#include <unistd.h>
int filelength(int fd) {
	off_t pos, ret;
	if ((pos = lseek(fd, 0, SEEK_CUR)) == -1 ||
		(ret = lseek(fd, 0, SEEK_END)) == -1 ||
		lseek(fd, pos, SEEK_SET) == -1)
		return -1;
	return (int)ret;
}

#endif
