#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "types.h"
#include "gr.h"
#include "piggy.h"
#include "error.h"

extern ushort GameBitmapXlat[MAX_BITMAP_FILES];
int piggy_init_file(FILE *Piggy_fp);
int bm_read_tbl(FILE *InfoFile);
void bm_write_all(FILE *fp);

char buf[65536];

int main() {
	FILE *tbl, *out;
	int pos, newpos;
	size_t len;
	
	FILE *Piggy_fp;
	char *filename = "descent.pig";
	char *tblfilename = "bitmaps.tbl";
	char *outfilename = "new.pig";

	printf("tbl2pig v0.9 - Write bitmaps.tbl to Descent 1 pig file\n\n");
	
	printf("Reading %s...\n", filename);
	
	Piggy_fp = fopen( filename, "rb" );
	if (Piggy_fp == NULL) {
		Error("Error opening %s: %s\n", filename, strerror(errno));
		return 1;
	}
	
	piggy_init_file(Piggy_fp);

	fseek(Piggy_fp, 0, SEEK_SET);
	pos = 0;
	Assert(fread(&pos, 1, sizeof(pos), Piggy_fp));
	//bm_read_all(Piggy_fp);
	//Assert(fread(GameBitmapXlat, sizeof(ushort)*MAX_BITMAP_FILES, 1, Piggy_fp));
	
	printf("Reading %s...\n", tblfilename);

	if (!(tbl = fopen(tblfilename, "r")))
		Error("Error opening %s: %s\n", tblfilename, strerror(errno));
	bm_read_tbl(tbl);
	fclose(tbl);


	printf("Writing %s...\n", outfilename);

	if (!(out = fopen(outfilename, "wb")))
		Error("Error creating %s: %s\n", outfilename, strerror(errno));

	fwrite(&pos, sizeof(int), 1, out);
	bm_write_all(out);
	fwrite( GameBitmapXlat, sizeof(ushort)*MAX_BITMAP_FILES, 1, out );

	newpos = ftell(out);
	if (newpos != pos) {
		fclose(out);
		remove(outfilename);
		Error("Unexpected data size change, aborted.");
	}
	
	fseek(Piggy_fp, newpos, SEEK_SET);
	while ((len = fread(buf, 1, sizeof(buf), Piggy_fp)) > 0)
		fwrite(buf, 1, len, out);
	
	fclose(out);
	
	printf("\nDone.\n");
	
	return 0;
}
