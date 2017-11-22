#ifndef PLYMODEL_H_INCLUDED
#define PLYMODEL_H_INCLUDED

#include <vector>
#include <string>
#include "Point.h"

enum  DrawMode {WIREFRAME = 0, FLAT_SURFACE = 1};

struct Material {
	bool hasMaterial = false;
	GLfloat *ambient, *diffuse, *specular;
	GLfloat *shininess;
};

struct UV {
	float u, v;
	UV(){}
	UV(float u, float v){
		this->u = u;
		this->v = v;
	}
};

class PlyModel {
public:
	PlyModel(){
		min = Point(10000, 10000, 10000);
		max = Point(-10000, -10000, -10000);
	};
	void setMaterial(GLfloat ambient[], GLfloat diffuse[], GLfloat specular[], GLfloat *shininess);
	void readFromFile(const char* fileName);
	void computeNormals();
	void normalize();
	void scale(float scaleFactor);
	void draw(DrawMode t);
	void unitize();
	void setCoordinatesLimits(Point cmin, Point cmax);
	int getTrianglesNumber();
	
private:
	bool hasNormals = false, normalComputed = false, normalized = false, minMaxAxisComputed = false, hasTexture = false;
	bool centralized = false;
	int facePoints = 0;
	Point min, max;
	Point coorMin, coordMax;
	
	Material mat;
	std::vector<Point> points;
	std::vector<Point> normals;
	std::vector<UV> uv_coordinates;
	std::vector<Point> colors;
	std::vector<std::vector<int> > faces;

	void findMinMaxLimits();
};

#endif
