#include <iostream>
#include <GL/glut.h>

#include "GameObjects.h"
#include "Random.h"
#include "utils.h"

using namespace std;

Point minCoordi(0.0f, 0.0f, -100.0f), maxCoordi(1.0f, 1.0f, 100.0f);

/***********************************************
 *           Object Member Functions           *
 ***********************************************/

Object::Object(){}

void Object::load3DModel(const char* file){
	model_3d.readFromFile(file);
	model_3d.unitize();
	model_3d.computeNormals();
}

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

void Object::setScale(float x, float y, float z){
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;
}

void Object::updatePosition(Point pos){
	this->pos = pos;
}

Point Object::getPosition(){
	return pos;
}

void Object::invertVx(){
	vel.x *= -1;
}

void Object::explode(){
	exploded = true;
}

bool Object::isExploding(){
	if(exploded){
		exploded = false;
		done = true;
		return true;
	}
	return false;
}

bool Object::isDone(){
	return done;
}

void Terrain::draw(){
	glPushMatrix();
		glFrontFace(GL_CW);
		glColor3f(color[0], color[1], color[2]);
		glTranslatef(pos.x, pos.y+0.12, pos.z-0.5);
		glRotatef(360, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		glScalef(scale[0], scale[1], scale[2]);

		model_3d.draw(FLAT_SURFACE);
	glPopMatrix();
}

/***********************************************
 *           Missile Member Functions          *
 ***********************************************/

void Missile::setGoal(Point pos){
	goal_pos = pos;
}

void Missile::setFrom(Point from){
	this->from = from;
}

void Missile::reset(){
	fired = false;
	exploded = false;
	done = false;
}

void Missile::update(float dt){
	if(!done){
		pos.x += vel.x * dt * tx;
		pos.y += vel.y * dt * ty;

		if(pos.distance(goal_pos) <= 0.01){
			fired = false;
			exploded = true;
		}
	}
}

void Missile::fire(){
	tx = goal_pos.x - from.x;
	ty = goal_pos.y - from.y;
	fired = true;
}

vector<Missile> Missile::divide(int level){
	int n = level, chance = 2 + (level * 2);
	vector<Missile> missiles;

	if(pos.y > 300){
		if(Random::floatInRange(0, 100) < chance){
			missiles.resize(n);
		}
	}

	return missiles;
}

void Missile::drawTarget(float sidex, float sidey){
	if(!done){
		glColor4f(Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), 0.0);
		glBegin(GL_LINES);
			glVertex2f(goal_pos.x-sidex, goal_pos.y-sidey);
		    glVertex2f(goal_pos.x+sidex, goal_pos.y+sidey);
		    glVertex2f(goal_pos.x+sidex, goal_pos.y-sidey);
		    glVertex2f(goal_pos.x-sidex, goal_pos.y+sidey);
		glEnd();
	}
}

void Missile::draw(){
	if(done || exploded) return;

	if(fired){
		glDisable(GL_LIGHTING);
		glColor4f(color[0], color[1], color[2], 0.0);

		glBegin(GL_LINES);
			glVertex3f(from.x, from.y, 0.0f);
			glVertex3f(pos.x, pos.y, 0.0f);
		glEnd();
		glEnable(GL_LIGHTING);

		glColor4f(Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), 0.0);

		glPushMatrix();
			glTranslatef(pos.x, pos.y, pos.z);
			glutSolidSphere(0.004, 30, 30);
		glPopMatrix();
	}else if(!fired && (!exploded && !done)){
		glDisable(GL_LIGHTING);
		glColor4f(color[0], color[1], color[2], 0.0);
		glBegin(GL_QUADS);
			glVertex3f(pos.x-0.004, pos.y+0.004, 0.0);
			glVertex3f(pos.x+0.004, pos.y+0.004, 0.0);
			glVertex3f(pos.x+0.004, pos.y-0.004, 0.0);
			glVertex3f(pos.x-0.004, pos.y-0.004, 0.0);
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

void Missile::operator=(const Missile& p){
	pos = p.pos;
	goal_pos = p.goal_pos;
	fired = p.fired;
}

/***********************************************
 *           Battery Member Functions          *
 ***********************************************/

Battery::Battery(){
	missiles.resize(10);
}

void Battery::init(){
	int i, j, k;

	exploded = done = false;
	k = 0;
	for(i = 0; i < 4; i++){
		for(j = i; j < 4; j++){
			missiles[k].setColor(color[0], color[1], color[2]);
			missiles[k].updatePosition(Point(pos.x + j*1.009 - i*1.0045, pos.y - i*1.009 , 0));
			missiles[k].setVelocities(Point(vel.x, vel.y, 0));
			k++;
		}
	}

	//pos = Point(pos.x + 0.01, pos.y - 0.01, 0);

}

void Battery::reload(){
	int i;

	missiles.clear();
	missiles.resize(10);

	init();
}

Missile Battery::fire(Point from, Point goal){
	missiles[0].updatePosition(from);
	missiles[0].setGoal(goal);
	missiles[0].setFrom(from);
	missiles[0].fire();
	Missile m = *missiles.begin();
	m.setColor(color[0], color[1], color[2]);
	missiles.erase(missiles.begin());

	if(missiles.size() == 0){
		empty = true;
	}

	return m;
}

bool Battery::isEmpty(){
	return (missiles.size() == 0);
}

int Battery::ammo(){
	return (done)?0:missiles.size();
}

void Battery::update(float dt){

}

void Battery::draw(){
	int i, n = missiles.size();

	if(!done){
		glPushMatrix();
			glColor3f(color[0], color[1], color[2]);
			glTranslatef(pos.x, pos.y, pos.z);
			glScalef(scale[0], scale[1], scale[2]);
			glRotatef(180, 1, 0, 0);
			glRotatef(-90, 0, 1, 0);
			
			model_3d.draw(FLAT_SURFACE);
		glPopMatrix();
	}
}

/***********************************************
 *            City Member Functions            *
 ***********************************************/

void City::update(float dt){

}

void City::draw(){
	glPushMatrix();
		glColor3f(color[0], color[1], color[2]);
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(scale[0], scale[1], scale[2]);
		glRotatef(180, 1, 0, 0);
		glRotatef(90, 0, 1, 0);
		model_3d.draw(FLAT_SURFACE);
	glPopMatrix();
}

/***********************************************
 *            Explosion Member Function        *
 ***********************************************/

void Explosion::draw(){
	Random::init();
	glPushMatrix();
		glColor3f(1.0, 0.0, 0.0);
		//glColor3f(Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0));
		glTranslatef(pos.x, pos.y, pos.z);

		glutSolidSphere(initRadius, 100, 100);
	glPopMatrix();
}

bool Explosion::isColliding(Object obj){
	if(pos.distance(obj.getPosition()) <= initRadius){
		return true;
	}
	return false;
}

void Explosion::update(float dt){
	if(initRadius >= finalRadius){
		signal = -1;
	}

	initRadius += signal * .05 * dt;

	if(initRadius <= 0 && signal == -1){
		finished = true;
		return ;
	}else if(initRadius < 0){
		initRadius = 1;
	}else{

	}
}

bool Explosion::isFinished(){
	return finished;
}

/***********************************************
 *           Button Member Function            *
 ***********************************************/

void Button::setSize(float x, float y){
	size.x = x;
	size.y = y;
}

void Button::setText(string text){
	this->text = text;
}

bool Button::clicked(float x, float y){
	if(x > (pos.x - size.x) && x < (pos.x + size.x)){
		if(y > (pos.y - size.y) && y < (pos.y + size.y)){
			return true;
		}
	}

	return false;
}

void Button::update(float dt){

}

void Button::draw(){
	glDisable(GL_LIGHTING);
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_POLYGON);
		glVertex3f(pos.x + size.x, pos.y - size.y, 0);
		glVertex3f(pos.x - size.x, pos.y - size.y, 0);
		glVertex3f(pos.x - size.x, pos.y + size.y, 0);
		glVertex3f(pos.x + size.x, pos.y + size.y, 0);

	glEnd();

   	Drawing::drawText(pos.x - size.x/2, pos.y + size.y/3, 10, 0, 0, 0, 1, text.c_str());
	glEnable(GL_LIGHTING);
}

/***********************************************
 *           Score Member Function            *
 ***********************************************/

void Score::setText(string text){
	this->text = text;
}

void Score::setScore(int score){
	this->score = score;
}

void Score::draw(){
	glDisable(GL_LIGHTING);
	Drawing::drawText(pos.x, pos.y, 1, 10, 0, 0, 1, text.c_str());
	Drawing::drawText(pos.x + .1, pos.y, 10, 1, 0, 0, 1, to_string(score).c_str());
	glEnable(GL_LIGHTING);
}

void Score::update(float dt){

}

void Score::operator=(const Score& p){
	this->pos = p.pos;
	this->text = p.text;
	this->score = p.score;
}
