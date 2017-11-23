#include <iostream>
#include <cstdio>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "skybox.h"

using namespace std;
float points[][6][3] = {
		  {-10.0f,  10.0f, -10.0f,
		  -10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f},
		  
		  {-10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f},
		  
		   {10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f},
		   
		  {-10.0f, -10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f},
		  
		  {-10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f, -10.0f},
		  
		  {-10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f}
};
/*float points[][6][3] = {
		  {0.0f,  1.0f, -100.0f,
		  0.0f, 0.0f, -100.0f,
		   1.0f, 0.0f, -100.0f,
		   1.0f, 0.0f, -100.0f,
		   1.0f,  1.0f, -100.0f,
		  0.0f,  1.0f, -100.0f},
		  
		  {0.0f, 0.0f,  100.0f,
		  0.0f, 0.0f, -100.0f,
		  0.0f,  1.0f, -100.0f,
		  0.0f,  1.0f, -100.0f,
		  0.0f,  1.0f,  100.0f,
		  0.0f, 0.0f,  100.0f},
		  
		   {1.0f, 0.0f, -100.0f,
		   1.0f, 0.0f,  100.0f,
		   1.0f,  1.0f,  100.0f,
		   1.0f,  1.0f,  100.0f,
		   1.0f,  1.0f, -100.0f,
		   1.0f, 0.0f, -100.0f},
		   
		  {0.0f, 0.0f,  100.0f,
		  0.0f,  1.0f,  100.0f,
		   1.0f,  1.0f,  100.0f,
		   1.0f,  1.0f,  100.0f,
		   1.0f, 0.0f,  100.0f,
		  0.0f, 0.0f,  100.0f},
		  
		  {0.0f,  1.0f, -100.0f,
		   1.0f,  1.0f, -100.0f,
		   1.0f,  1.0f,  100.0f,
		   1.0f,  1.0f,  100.0f,
		  0.0f,  1.0f,  100.0f,
		  0.0f,  1.0f, -100.0f},
		  
		  {0.0f, 0.0f, -100.0f,
		  0.0f, 0.0f,  100.0f,
		   1.0f, 0.0f, -100.0f,
		   1.0f, 0.0f, -100.0f,
		  0.0f, 0.0f,  100.0f,
		   1.0f, 0.0f,  100.0f}
};*/

SkyBox::SkyBox(const char* skybox_model){
	textureManager = new glcTexture();
	textureManager->SetWrappingMode(GL_CLAMP_TO_EDGE);
	
	model.readFromFile(skybox_model);
	model.unitize();
	model.computeNormals();
}

void SkyBox::load_skybox(const char* front, const char* back, const char* top, const char* bottom, const char* left, const char* right){
	textureManager->SetNumberOfTextures(6);

	textureManager->CreateTexture(front, 0);
	textureManager->CreateTexture(back, 1);
	textureManager->CreateTexture(top, 2);
	textureManager->CreateTexture(bottom, 3);
	textureManager->CreateTexture(left, 4);
	textureManager->CreateTexture(right, 5);
	nTextures = 6;
}

void SkyBox::load_skybox(const char* texture){
	textureManager->CreateTexture(texture, 0);
	
	nTextures = 1;
}

void SkyBox::draw_skybox(){
	vector<vector<int> > faces = model.getFaces();
	vector<Point> points = model.getPoints();
	vector<Point> normals = model.getNormals();
	vector<UV> uv_coordinates = model.getUVCoords();
	int nf = 6, facePoints = 4, i, j, k = 0;
	
	glDepthMask(0); 
	glPushMatrix();

		glColor3f(1, 1, 1);
		glScalef(0.6, 0.6, 0.6);
		glTranslatef(1, 1, -1);
		glRotatef(90, 1, 0, 0);
		
		for(i = 0; i < nf; i++){
			if(nTextures == 1) textureManager->Bind(0);
			if(nTextures > 1)textureManager->Bind(i);
		
			glBegin(GL_QUADS);
				for(j = 0; j < facePoints; j++){
					glNormal3f(normals[faces[i][j]].x, normals[faces[i][j]].y, normals[faces[i][j]].z);
					glTexCoord2f(uv_coordinates[faces[i][j]].u, uv_coordinates[faces[i][j]].v);
					glVertex3f(points[faces[i][j]].x, points[faces[i][j]].y, points[faces[i][j]].z);
				}
			glEnd();
		}
	glPopMatrix();
	glDepthMask(1); 
	/*int i, j;
	for(i = 0; i < 6; i++){
		textureManager->Bind(i);
		glBegin(GL_TRIANGLE_FAN);
			if(i == 0){
				glNormal3f(0.0, 0.0, -1.0);
			}else if(i == 1){
				glNormal3f(0.0, 0.0, 1.0);
			}else if(i == 2){
				glNormal3f(0.0, -1.0, 0.0);
			}else if(i == 3){
				glNormal3f(0.0, 1.0, 0.0);
			}else if(i == 4){
				glNormal3f(1.0, 0.0, 0.0);
			}else if(i == 5){
				glNormal3f(-1.0, 0.0, 0.0);
			}
			for(j = 0; j < 6; j++){
				glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
			}
		glEnd();
	}*/
}

