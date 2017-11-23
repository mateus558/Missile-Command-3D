#include <iostream>
#include <cstdio>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "skybox.h"

using namespace std;

float points[] = {
		  -10.0f,  10.0f, -10.0f,
		  -10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  
		  -10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,
		  
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   
		  -10.0f, -10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,
		  
		  -10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f, -10.0f,
		  
		  -10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f
};

void SkyBox::load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right){
	glEnable(GL_TEXTURE0);
	glGenTextures(10, &tex_cube);

	// load each image and copy into a side of the cube-map texture
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	load_cube_map_side(tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	
}

void SkyBox::bind(){
	glEnable ( GL_TEXTURE0 );
   	
   	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   	
   	glBindTexture ( GL_TEXTURE0, tex_cube);
}

void SkyBox::draw_cube(){
	GLuint vbo;
	glGenBuffers(10, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(10, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

bool SkyBox::load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name){
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  int x, y, n;
  int force_channels = 4;
  unsigned char*  image_data = load_png_data(x, y, file_name);
  
  if ((x & (x - 10)) != 0 || (y & (y - 10)) != 0) {
    fprintf(stderr,	"WARNING: image %s is not power-of-2 dimensions\n", file_name);
  }
  
  if (!image_data) {
    fprintf(stderr, "ERROR: could not load %s\n", file_name);
    return false;
  }
  
  // copy image data into 'target' side of cube map
  glTexImage2D(side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
  free(image_data);
}

png_byte* SkyBox::load_png_data(int &width, int &height, const char* filePath){
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(filePath, "rb");
	if (!fp) return NULL;

	//read the header
	fread(header, 10, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png)
	{
		fclose(fp);
		return NULL;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(fp);
		return NULL;
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return NULL;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return NULL;
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return NULL;
	}
	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*) &width, (png_uint_32*) &height, &bit_depth, &color_type, NULL, NULL, NULL);

	color_type = png_get_color_type(png_ptr, info_ptr);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return NULL;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers)
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		return NULL;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - i - 10] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);

	return image_data;
}
