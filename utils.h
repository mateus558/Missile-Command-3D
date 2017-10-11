#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <iostream>
#include <cmath>
#include <cstring>

namespace Drawing {
	void drawText(int x, int y, float r, float g, float b, int font, const char *string);
	void drawEllipse(int x, int y, int z, float r, float g, float b, int xradius, int yradius, int npartes);
}

#endif
