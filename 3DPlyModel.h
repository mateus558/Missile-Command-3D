#ifndef PLYMODEL_H_INCLUDED
#define PLYMODEL_H_INCLUDED

#include <vector>
#include <string>
#include "Point.h"

enum  DrawMode {WIREFRAME = 0, FLAT_SURFACE = 1};

class PlyModel {
public:
	PlyModel(){
		min = Point(10000, 10000, 10000);
		max = Point(-10000, -10000, -10000);
	};
	void readFromFile(const char* fileName);
	void computeNormals();
	void normalize();
	void scale(float scaleFactor);
	void draw(DrawMode t);
	void unitize();
	void miniScale();
	int getTrianglesNumber();
	
private:
	bool hasNormals = false, normalComputed = false, normalized = false, minMaxAxisComputed = false;
	bool centralized = false;
	int facePoints = 0;
	Point min, max;

	std::vector<Point> points;
	std::vector<Point> normals;
	std::vector<Point> colors;
	std::vector<std::vector<int> > faces;
};

#endif
