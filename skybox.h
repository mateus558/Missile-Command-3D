#ifndef SKYBOX_INCLUDED
#define SKYBOX_INCLUDED

#include <GL/glut.h>
#include "glcTexture.h"
#include "3DPlyModel.h"

class SkyBox {
	public:
		SkyBox(const char* skybox_model);
		void load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right);
		void load_skybox(const char* texture);
		void draw_skybox();
			
	private:
		glcTexture *textureManager;
		PlyModel model;
		int nTextures = 0;
};

#endif
