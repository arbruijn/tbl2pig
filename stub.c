#include "types.h"
#include "gr.h"
#include "vecmat.h"
#include "robot.h"
#include "polyobj.h"

int load_polygon_model(char *filename,int n_textures,int first_texture,robot_info *r) {
	N_polygon_models++;
	return N_polygon_models-1;
}

int read_model_guns(char *filename,vms_vector *gun_points, vms_vector *gun_dirs, int *gun_submodels) {
	return 1;
}

void gr_remap_bitmap_good(grs_bitmap * bmp, ubyte * palette, int transparent_color, int super_transparent_color) {
}

int gr_find_closest_color(int r, int g, int b) {
	return 1;
}

