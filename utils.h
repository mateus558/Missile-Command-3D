#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#ifdef __unix__
    #include <dirent.h>
#elif _WIN32
    #include <windows.h>
#endif
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <cstring>
#include <GL/gl.h>
#include "png.h"

namespace Drawing {
	void drawText(float x, float y, float z, float r, float g, float b, int font, const char *string);
	void drawEllipse(float x, float y, float z, float r, float g, float b, float xradius, float yradius, int npartes);
	GLvoid draw_circle(const GLfloat radius,const GLuint num_vertex, const GLfloat z);
}

GLuint png_texture_load(const char * file_name, int * width, int * height);
std::vector<std::string> list_datasets(bool list);

#endif
