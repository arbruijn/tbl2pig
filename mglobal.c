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

#include "types.h"
#include "inferno.h"
#include "gr.h"
#include "bm.h"
#include "object.h"
#include "vclip.h"
#include "effects.h"
#include "polyobj.h"
#include "wall.h"
#include "textures.h"
#include "game.h"
#include "multi.h"

#include "hostage.h"
#include "powerup.h"
#include "laser.h"
#include "sounds.h"
#include "aistruct.h"
#include "robot.h"
#include "weapon.h"
#include "gauges.h"
#include "player.h"
#include "fuelcen.h"
#include "endlevel.h"
#include "cntrlcen.h"
#include "compbit.h"

int descent_critical_error;

ubyte gr_palette[256*3];

int Num_effects;
eclip Effects[MAX_EFFECTS];

wall Walls[MAX_WALLS];					// Master walls array
int Num_walls=0;							// Number of walls

wclip WallAnims[MAX_WALL_ANIMS];		// Wall animations
int Num_wall_anims;

ubyte Sounds[MAX_SOUNDS];
ubyte AltSounds[MAX_SOUNDS];

int Num_total_object_types;

byte	ObjType[MAX_OBJTYPE];
byte	ObjId[MAX_OBJTYPE];
fix	ObjStrength[MAX_OBJTYPE];

//for each model, a model number for dying & dead variants, or -1 if none
int Dying_modelnums[MAX_POLYGON_MODELS];
int Dead_modelnums[MAX_POLYGON_MODELS];

//right now there's only one player ship, but we can have another by 
//adding an array and setting the pointer to the active ship.
player_ship only_player_ship,*Player_ship=&only_player_ship;

//----------------- Miscellaneous bitmap pointers ---------------
int					Num_cockpits = 0;
bitmap_index		cockpit_bitmap[N_COCKPIT_BITMAPS];

//---------------- Variables for wall textures ------------------
int 					Num_tmaps;
tmap_info 			TmapInfo[MAX_TEXTURES];

//---------------- Variables for object textures ----------------

int					First_multi_bitmap_num=-1;

bitmap_index		ObjBitmaps[MAX_OBJ_BITMAPS];
ushort				ObjBitmapPtrs[MAX_OBJ_BITMAPS];		// These point back into ObjBitmaps, since some are used twice.

//----------------- Variables for video clips -------------------
int 					Num_vclips = 0;
vclip 				Vclip[VCLIP_MAXNUM];		// General purpose vclips.

int N_powerup_types = 0;
powerup_type_info Powerup_info[MAX_POWERUP_TYPES];


int	N_robot_types = 0;
int	N_robot_joints = 0;

//	Robot stuff
robot_info Robot_info[MAX_ROBOT_TYPES];

//Big array of joint positions.  All robots index into this array

jointpos Robot_joints[MAX_ROBOT_JOINTS];

// Texture map stuff

int NumTextures = 0;
bitmap_index Textures[MAX_TEXTURES];		// All textures.

weapon_info Weapon_info[MAX_WEAPON_TYPES];
int	N_weapon_types=0;

//------------- Globaly used hostage variables --------------------------------------------------
int 					N_hostage_types = 0;			  				// Number of hostage types
int 					Hostage_vclip_num[MAX_HOSTAGE_TYPES];	//vclip num for each tpye of hostage
hostage_data 		Hostages[MAX_HOSTAGES];						// Data for each hostage in mine
vclip Hostage_face_clip[MAX_HOSTAGES];


polymodel Polygon_models[MAX_POLYGON_MODELS];	// = {&bot11,&bot17,&robot_s2,&robot_b2,&bot11,&bot17,&robot_s2,&robot_b2};
int N_polygon_models = 0;

vms_vector controlcen_gun_points[MAX_CONTROLCEN_GUNS];
vms_vector controlcen_gun_dirs[MAX_CONTROLCEN_GUNS];
int	N_controlcen_guns;

vms_vector	Gun_pos[MAX_CONTROLCEN_GUNS], Gun_dir[MAX_CONTROLCEN_GUNS];

bitmap_index Gauges[MAX_GAUGE_BMS];   // Array of all gauge bitmaps.

int station_modelnum,exit_modelnum,destroyed_exit_modelnum;
