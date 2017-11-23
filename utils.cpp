#include "utils.h"
#include <GL/glut.h>

using namespace std;

namespace Drawing{
	void drawText(float x, float y, float z, float r, float g, float b, int font, const char *string){
		glColor3f( r, g, b );
		glRasterPos3f(x, y, z);
		int len, i;
		len = (int)strlen(string);

		for (i = 0; i < len; i++) {
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18 , string[i]);
		}
	}

	void drawEllipse(float x, float y, float z, float r, float g, float b, float xradius, float yradius, int npartes){
		glColor4f(r, g, b, 0.0f);
		glBegin(GL_POLYGON);
			for(int ii = 0; ii < npartes; ii++)
			{
				float theta = 2.0f * 3.1415926f * float(ii) / float(npartes);//get the current angle

				float xi = xradius * cosf(theta);//calculate the x component
				float yi = yradius * sinf(theta);//calculate the y component

				glVertex3f(xi + x, yi + y, z);//output vertex

			}
		glEnd();
	}
}
