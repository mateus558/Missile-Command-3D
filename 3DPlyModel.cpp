#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glut.h>

#include "3DPlyModel.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b)) 

using namespace std;

void PlyModel::readFromFile(const char* fileName){
	bool dimSet = 0, endWhile = false;
	int np, nf, i, j, dim;
	char deli = 32;
	ifstream plyFile(fileName);
	string str, item, end("end_header");	
	vector<string> items;
	
	plyFile >> str;
	
	if(str != string("ply")){
		cerr << "Wrong file format. (Ply loading)" << endl;
		return;
	}
	
	while(getline(plyFile, str)){
		istringstream ss(str);
		
		endWhile = true;
		for(i = 0; i < end.size(); i++){
			if(str[i] != end[i]){
				endWhile = false;
				break;
			}
		}
		if(endWhile) break;
		
		while(getline(ss, item, deli)){
			items.push_back(item);
		}
		
		for(auto itr = items.begin(); itr != items.end(); itr++){
			if(*(itr) == string("format") && *(itr+1) != string("ascii")){
				cerr << "Codification not supported. (Ply loading)" << endl;
				return;
			}
			
			if(*(itr) == string("vertex")){
				np = stoi(*(itr+1));
				points.resize(np);
			}
			if(*(itr) == string("face")){
				nf = stoi(*(itr+1));
				faces.resize(nf);
			}
			if(*(itr) == string("nx")){
				hasNormals = true;
				normals.resize(np);
			}
		}
		items.clear();
	}
	
	for(i = 0; i < np; i++){
		getline(plyFile, str);
		istringstream ss(str);
		
		while(getline(ss, item, deli)){
			items.push_back(item);
		}

		points[i] = Point(stod(items[0]), stod(items[1]), stod(items[2]));
		if(hasNormals){
			normals[i] = Point(stod(items[3]), stod(items[4]), stod(items[5]));
		}
		
		items.clear();
	}
	
	for(i = 0; i < nf; i++){
		getline(plyFile, str);
		istringstream ss(str);
				
		while(getline(ss, item, deli)){
			items.push_back(item);
		}
		
		if(!dimSet){
			dim = stoi(items[0]);
			facePoints = dim;
			dimSet = true;
		}
		
		faces[i].resize(dim);
		
		for(j = 0; j < dim; j++){
			faces[i][j] = stoi(items[j+1]);
		}
		items.clear();
	}
	
	plyFile.close();
}

void PlyModel::normalize(){
	int i, n = points.size();
	
	for(i = 0; i < n; i++){
		points[i].normalize();
	}
	normalized = true;
}

void PlyModel::unitize(){
	int i, n = points.size();
	float cx, cy, cz, scaleFactor, w, h, d;
	
	if(!centralized){
		for(i = 0; i < n; i++){
			if(points[i].x < min.x){
				min.x = points[i].x;
			}
			if(points[i].y < min.y){
				min.y = points[i].y;
			}
			if(points[i].z < min.z){
				min.z = points[i].z;
			}
			if(points[i].x > max.x){
				max.x = points[i].x;
			}
			if(points[i].y > max.y){
				max.y = points[i].y;
			}
			if(points[i].z > max.z){
				max.z = points[i].z;
			}
		}
		
		cx = (max.x - min.x) / 2.0f;
		cy = (max.y - min.y) / 2.0f;
		cz = (max.z - min.z) / 2.0f;
	
		w = fabs(max.x) + fabs(min.x);
		h = fabs(max.y) + fabs(min.y);
		d = fabs(max.z) + fabs(min.z);
	
		scaleFactor = 2.0f / MAX(MAX(w, h), d);
		
		for(i = 0; i < n; i++){
			points[i].x -= min.x + (max.x - min.x) / 2.0f;
			points[i].y -= min.y + (max.y - min.y) / 2.0f;
			points[i].z -= min.z + (max.z - min.z) / 2.0f;
		}
		
		scale(scaleFactor);		
	
		centralized = !centralized;
	}	
}

void PlyModel::scale(float scaleFactor){
	int i, j, n = points.size();
	
	for(i = 0; i < n; i++){
		points[i] *= scaleFactor;
	}
	normalized = false;
}

void PlyModel::computeNormals(){
	int i, nf = faces.size();
	Point vec, vec1;
			
	if(!hasNormals && !normalComputed){
		normals.resize(nf);
	
		for(i = 0; i < nf; i++){  
			vec.x = points[faces[i][0]].x - points[faces[i][1]].x;
			vec.y = points[faces[i][0]].y - points[faces[i][1]].y;
			vec.z = points[faces[i][0]].z - points[faces[i][1]].z;
			
			vec1.x = points[faces[i][0]].x - points[faces[i][2]].x;
			vec1.y = points[faces[i][0]].y - points[faces[i][2]].y;
			vec1.z = points[faces[i][0]].z - points[faces[i][2]].z;
			
			normals[i] = Point(vec.y * vec1.z - vec.z * vec1.y,
							   vec.z * vec1.x - vec.x * vec1.z,
							   vec.x * vec1.y - vec.y * vec1.x);
		}
		
		normalComputed = true;
	}
}

void PlyModel::draw(DrawMode t){
	int i, j, nf = faces.size(), np = points.size(), ns;
	
	switch(t){
		case WIREFRAME:
			glColor3f(1, 1, 1);
			for(i = 0; i < nf; i++){
				glBegin(GL_LINE_LOOP);
				for(j = 0; j < facePoints; j++){
					glVertex3f(points[faces[i][j]].x, points[faces[i][j]].y, points[faces[i][j]].z);
				}
				glEnd();	
			}
			break;
		case FLAT_SURFACE:
			glColor3f(0,.3,0);
			for(i = 0; i < nf; i++){
				if(!hasNormals && normalComputed){
					glNormal3f(normals[i].x, normals[i].y, normals[i].z);
				}
				glBegin(GL_TRIANGLE_FAN);
					for(j = 0; j < facePoints; j++){
						if(hasNormals){	
							glNormal3f(normals[faces[i][j]].x, normals[faces[i][j]].y, normals[faces[i][j]].z);
						}
						glVertex3f(points[faces[i][j]].x, points[faces[i][j]].y, points[faces[i][j]].z);
					}
				glEnd();
			}
			break;
		default:
			cerr << "Invalid draw mode." << endl;
			break;
	}
}	

int PlyModel::getTrianglesNumber(){
	return (facePoints == 3) ? faces.size() : faces.size()*2;
}