#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <iostream>
#include <cmath>
#include <cstring>

namespace Drawing {
	void drawText(float x, float y, float r, float g, float b, int font, const char *string);
	void drawEllipse(float x, float y, float z, float r, float g, float b, float xradius, float yradius, int npartes);
}

#endif
