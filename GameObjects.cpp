#include <iostream>
#include <GL/glut.h>
#include "GameObjects.h"
#include "Random.h"

using namespace std;

/***********************************************
 *            Point Member Functions           *
 ***********************************************/

void Point::operator=(const Point& p){
	x = p.x;
	y = p.y;
	z = p.z;
}

ostream &operator<<( ostream &output, const Point &point ){
    output << "[" << point.x << ", " << point.y << ", " << point.z << "]";

    return output;
}

/***********************************************
 *           Object Member Functions           *
 ***********************************************/

Object::Object(){}

void Object::setMass(float mass){
	this->mass = mass;
}

void Object::setVelocities(Point vel){
	this->vel = vel;
}

void Object::setAceleration(Point acel){
	this->acel = acel;
}

void Object::setColor(float r, float g, float b){
	color[0] = r;
	color[1] = g;
	color[2] = b;
}



/***********************************************
 *           Sphere Member Functions           *
 ***********************************************/

Sphere::Sphere(){}

Sphere::Sphere(Point pos, float radius){
	this->pos = pos;
	this->radius = radius;
}

void Sphere::updatePosition(Point pos){
	this->pos = pos;
}

Point Sphere::getPosition(){
	return pos;
}

void Sphere::setRadius(float radius){
	this->radius = radius;
}

float Sphere::getRadius(){
	return radius;
}

void Sphere::invertVx(){
	vel.x *= -1;
}

void Sphere::update(float dt){
	vel.x += acel.x * dt;
	vel.y += acel.y * dt;
	vel.z += acel.z * dt;
	
	pos.x += vel.x * dt;
	pos.y += vel.y * dt;
	pos.z += vel.z * dt;
}

void Sphere::draw(){	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
	glColor4f(color[0], color[1], color[2], 1.0);
	glTranslatef(pos.x, pos.y, pos.z);
	glutSolidSphere(radius, 100, 100);
}



