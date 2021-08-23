/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "gr.h"
#include "piggy.h"
#include "mono.h"
#include "hash.h"
#include "error.h"
#include "file.h"

void bm_read_all(FILE * fp);

#if 0
ubyte *BitmapBits = NULL;
ubyte *SoundBits = NULL;
#endif

typedef struct BitmapFile	{
	char			name[15];
} BitmapFile;

typedef struct SoundFile	{
	char			name[15];
} SoundFile;

hashtable AllBitmapsNames;
hashtable AllDigiSndNames;

int Num_bitmap_files = 0;
int Num_sound_files = 0;

digi_sound GameSounds[MAX_SOUND_FILES];
int SoundOffset[MAX_SOUND_FILES];
grs_bitmap GameBitmaps[MAX_BITMAP_FILES];

int Num_bitmap_files_new = 0;
int Num_sound_files_new = 0;
static BitmapFile AllBitmaps[ MAX_BITMAP_FILES ];
static SoundFile AllSounds[ MAX_SOUND_FILES ];

#define DBM_FLAG_LARGE 	128		// Flags added onto the flags struct in b
#define DBM_FLAG_ABM		64

int Piggy_bitmap_cache_size = 0;
int Piggy_bitmap_cache_next = 0;
ubyte * Piggy_bitmap_cache_data = NULL;
static int GameBitmapOffset[MAX_BITMAP_FILES];
static ubyte GameBitmapFlags[MAX_BITMAP_FILES];
ushort GameBitmapXlat[MAX_BITMAP_FILES];

#define PIGGY_BUFFER_SIZE (2048*1024)

ubyte bogus_data[64*64];
grs_bitmap bogus_bitmap;
ubyte bogus_bitmap_initialized=0;
digi_sound bogus_sound;

bitmap_index piggy_find_bitmap(char *name) {
	bitmap_index bmp;
	int i;

	bmp.index = 0;

	i = hashtable_search( &AllBitmapsNames, name );
	Assert( i != 0 );
	if ( i < 0 )
		return bmp;

	bmp.index = i;
	return bmp;
}

int piggy_find_sound(char *name) {
	int i;

	i = hashtable_search( &AllDigiSndNames, name );

	if ( i < 0 )
		return 255;

	return i;
}

bitmap_index piggy_register_bitmap(grs_bitmap * bmp, char * name, int in_file ) {
	bitmap_index temp;
	Assert( Num_bitmap_files < MAX_BITMAP_FILES );

	temp.index = Num_bitmap_files;


	if (!in_file)	{
		//if ( !BigPig )	gr_bitmap_rle_compress( bmp );
		Num_bitmap_files_new++;
	}

	strncpy( AllBitmaps[Num_bitmap_files].name, name, 12 );
	hashtable_insert( &AllBitmapsNames, AllBitmaps[Num_bitmap_files].name, Num_bitmap_files );
	GameBitmaps[Num_bitmap_files] = *bmp;
	if ( !in_file )	{
		GameBitmapOffset[Num_bitmap_files] = 0;
		GameBitmapFlags[Num_bitmap_files] = bmp->bm_flags;
	}
	Num_bitmap_files++;

	return temp;
}

int piggy_register_sound(digi_sound * snd, char * name, int in_file) {
	int i;

	Assert( Num_sound_files < MAX_SOUND_FILES );

	strncpy( AllSounds[Num_sound_files].name, name, 12 );
	hashtable_insert( &AllDigiSndNames, AllSounds[Num_sound_files].name, Num_sound_files );
	GameSounds[Num_sound_files] = *snd;
	if ( !in_file )	{
		SoundOffset[Num_sound_files] = 0;	
	}

	i = Num_sound_files;
   
	if (!in_file)
		Num_sound_files_new++;

	Num_sound_files++;
	return i;
}

#ifndef __WATCOMC__
#pragma pack(push,1)
#endif
typedef struct DiskBitmapHeader {
	char name[8];
	ubyte dflags;
	ubyte	width;	
	ubyte height;
	ubyte flags;
	ubyte avg_color;
	int offset;
} DiskBitmapHeader;

typedef struct DiskSoundHeader {
	char name[8];
	int length;
	int data_length;
	int offset;
} DiskSoundHeader;
#ifndef __WATCOMC__
#pragma pack(pop)
#endif

int piggy_init_file(FILE *Piggy_fp)
{
	int sbytes = 0;
	char temp_name_read[16];
	char temp_name[32];
	grs_bitmap temp_bitmap;
	digi_sound temp_sound;
	DiskBitmapHeader bmh;
	DiskSoundHeader sndh;
	int header_size, N_bitmaps, N_sounds;
	int i,size;
	int Pigdata_start;

	hashtable_init( &AllBitmapsNames, MAX_BITMAP_FILES );
	hashtable_init( &AllDigiSndNames, MAX_SOUND_FILES );

	#if 0
	if ( FindArg( "-nosound" ) || (digi_driver_board<1) )		{
		mprintf(( 0, "Not loading sound data!!!!!\n" ));
	}
	#endif
	
	for (i=0; i<MAX_SOUND_FILES; i++ )	{
		GameSounds[i].length = 0;
		GameSounds[i].data = NULL;
		SoundOffset[i] = 0;
	}

	for (i=0; i<MAX_BITMAP_FILES; i++ )		{
		GameBitmapXlat[i] = i;
		GameBitmaps[i].bm_flags = BM_FLAG_PAGED_OUT;
	}

	if ( !bogus_bitmap_initialized )	{
		int i;
		ubyte c;
		bogus_bitmap_initialized = 1;
		memset( &bogus_bitmap, 0, sizeof(grs_bitmap) );
		bogus_bitmap.bm_w = bogus_bitmap.bm_h = bogus_bitmap.bm_rowsize = 64;
		bogus_bitmap.bm_data = bogus_data;
		c = gr_find_closest_color( 0, 0, 63 );
		for (i=0; i<4096; i++ ) bogus_data[i] = c;
		c = gr_find_closest_color( 63, 0, 0 );
		// Make a big red X !
		for (i=0; i<64; i++ )	{
			bogus_data[i*64+i] = c;
			bogus_data[i*64+(63-i)] = c;
		}
		piggy_register_bitmap( &bogus_bitmap, "bogus", 1 );
		bogus_sound.length = 64*64;
		bogus_sound.data = bogus_data;
		GameBitmapOffset[0] = 0;
	}

	#if 0
	if ( FindArg( "-bigpig" ))
		BigPig = 1;

	if ( FindArg( "-lowmem" ))
		piggy_low_memory = 1;

	if ( FindArg( "-nolowmem" ))
		piggy_low_memory = 0;

	if (piggy_low_memory)
		digi_lomem = 1;

	if ( (i=FindArg( "-piggy" )) )	{
		filename	= Args[i+1];
		mprintf( (0, "Using alternate pigfile, '%s'\n", filename ));
	}
	#endif
	
#ifndef SHAREWARE
	Assert( fread( &Pigdata_start, sizeof(int), 1, Piggy_fp ) );

#ifdef EDITOR
	if ( FindArg("-nobm") )
#endif
	{
		bm_read_all( Piggy_fp );	// Note connection to above if!!!
		fread( GameBitmapXlat, sizeof(ushort)*MAX_BITMAP_FILES, 1, Piggy_fp );
	}
#else

	Pigdata_start = 0;
#endif

	fseek( Piggy_fp, Pigdata_start, SEEK_SET );
	size = filelength(fileno(Piggy_fp)) - Pigdata_start;
	//mprintf( (0, "\nReading data (%d KB) ", size/1024 ));

	Assert( fread( &N_bitmaps, sizeof(int), 1, Piggy_fp ) );
	size -= sizeof(int);
	Assert( fread( &N_sounds, sizeof(int), 1, Piggy_fp ) );
	size -= sizeof(int);

	header_size = (N_bitmaps*sizeof(DiskBitmapHeader)) + (N_sounds*sizeof(DiskSoundHeader));

	#if 0
	y = 189;

	gr_set_curfont( Gamefonts[GFONT_SMALL] );	
	gr_set_fontcolor(gr_find_closest_color_current( 20, 20, 20 ),-1 );
	gr_printf( 0x8000, y-10, "%s...", TXT_LOADING_DATA );
	#endif

	for (i=0; i<N_bitmaps; i++ )	{
		Assert( fread( &bmh, sizeof(DiskBitmapHeader), 1, Piggy_fp ) );
		//size -= sizeof(DiskBitmapHeader);
		memcpy( temp_name_read, bmh.name, 8 );
		temp_name_read[8] = 0;
		if ( (bmh.dflags & DBM_FLAG_ABM) )	
			sprintf( temp_name, "%s#%d", temp_name_read, bmh.dflags & 63 );
		else
			strcpy( temp_name, temp_name_read );
		memset( &temp_bitmap, 0, sizeof(grs_bitmap) );
		if ( bmh.dflags & DBM_FLAG_LARGE )
			temp_bitmap.bm_w = temp_bitmap.bm_rowsize = bmh.width+256;
		else
			temp_bitmap.bm_w = temp_bitmap.bm_rowsize = bmh.width;
		temp_bitmap.bm_h = bmh.height;
		temp_bitmap.bm_flags = BM_FLAG_PAGED_OUT;
		temp_bitmap.avg_color = bmh.avg_color;
		temp_bitmap.bm_data = Piggy_bitmap_cache_data;

		GameBitmapFlags[i+1] = 0;
		if ( bmh.flags & BM_FLAG_TRANSPARENT ) GameBitmapFlags[i+1] |= BM_FLAG_TRANSPARENT;
		if ( bmh.flags & BM_FLAG_SUPER_TRANSPARENT ) GameBitmapFlags[i+1] |= BM_FLAG_SUPER_TRANSPARENT;
		if ( bmh.flags & BM_FLAG_NO_LIGHTING ) GameBitmapFlags[i+1] |= BM_FLAG_NO_LIGHTING;
		if ( bmh.flags & BM_FLAG_RLE ) GameBitmapFlags[i+1] |= BM_FLAG_RLE;

		GameBitmapOffset[i+1] = bmh.offset + header_size + (sizeof(int)*2) + Pigdata_start;
		Assert( (i+1) == Num_bitmap_files );
		piggy_register_bitmap( &temp_bitmap, temp_name, 1 );
	}

	for (i=0; i<N_sounds; i++ )	{
		Assert( fread( &sndh, sizeof(DiskSoundHeader), 1, Piggy_fp ) );
		//size -= sizeof(DiskSoundHeader);
		temp_sound.length = sndh.length;
		temp_sound.data = (ubyte *)(sndh.offset + header_size + (sizeof(int)*2)+Pigdata_start);
		SoundOffset[Num_sound_files] = sndh.offset + header_size + (sizeof(int)*2)+Pigdata_start;
		memcpy( temp_name_read, sndh.name, 8 );
		temp_name_read[8] = 0;
		piggy_register_sound( &temp_sound, temp_name_read, 1 );
		sbytes += sndh.length;
		//mprintf(( 0, "%d bytes of sound\n", sbytes ));
	}

	#if 0
	SoundBits = malloc( sbytes + 16 );
	if ( SoundBits == NULL )
		Error( "Not enough memory to load DESCENT.PIG sounds\n" );

#ifdef EDITOR
	Piggy_bitmap_cache_size	= size - header_size - sbytes + 16;
	Assert( Piggy_bitmap_cache_size > 0 );
#else
	Piggy_bitmap_cache_size = PIGGY_BUFFER_SIZE;
#endif
	BitmapBits = malloc( Piggy_bitmap_cache_size );
	if ( BitmapBits == NULL )
		Error( "Not enough memory to load DESCENT.PIG bitmaps\n" );
	Piggy_bitmap_cache_data = BitmapBits;	
	Piggy_bitmap_cache_next = 0;
	#endif
	
	//mprintf(( 0, "\nBitmaps: %d KB   Sounds: %d KB\n", Piggy_bitmap_cache_size/1024, sbytes/1024 ));

	//atexit(piggy_close_file);

	return 0;
}
