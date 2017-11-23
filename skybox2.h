#ifndef SKYBOX_INCLUDED
#define SKYBOX_INCLUDED

#include <GL/glut.h>
#include "png.h"

class SkyBox {
	public:
		SkyBox(){};
		void load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right);
		void bind();
		void draw_cube();
			
	private:
		GLuint tex_cube;
		bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
		png_byte* load_png_data(int &width, int &height, const char* filePath);
};

#endif
