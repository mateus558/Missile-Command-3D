#ifndef SKYBOX_INCLUDED
#define SKYBOX_INCLUDED

#include <GL/glut.h>
#include "png.h"
#include "glcTexture.h"
#include "3DPlyModel.h"
#include "GameObjects.h"
#include "shader.h"

class SkyBox {
	public:
		SkyBox(const char* skybox_model);
		void load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right);
		void load_skybox(const char* texture);
		void center_of(Object *obj);
		void center_of(Point p);
		void draw_skybox(Point center, Point eye, Point pos, Point scale, Point rot);
		GLuint png_texture_load(const char * file_name, int * width, int * height);
			
	private:
		GLuint sides[6];
		PlyModel model;
		Shader *shader;
		
		int nTextures = 0;
};

#endif
