#include <iostream>
#include <GL/glut.h>

#include "GameObjects.h"
#include "Random.h"
#include "utils.h"

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

		if(pos.distance(goal_pos) <= 3){
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
	int n = level, chance = 20 + (level * 2);
	vector<Missile> missiles;

	if(pos.y > 300){
		if(Random::floatInRange(0, 100) < chance){
			missiles.resize(n);
		}
	}

	return missiles;
}

void Missile::drawTarget(int side){
	if(!done){
		glColor4f(Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), 0.0);
		glBegin(GL_LINES);
			glVertex2f(goal_pos.x-side, goal_pos.y-side);
		    glVertex2f(goal_pos.x+side, goal_pos.y+side);
		    glVertex2f(goal_pos.x+side, goal_pos.y-side);
		    glVertex2f(goal_pos.x-side, goal_pos.y+side);
		glEnd();
	}
}

void Missile::draw(){
	if(done || exploded) return;
	glColor4f(color[0], color[1], color[2], 0.0);

	if(fired){
		glBegin(GL_LINES);
			glVertex3f(from.x, from.y, 0.0f);
			glVertex3f(pos.x, pos.y, 0.0f);
		glEnd();

		glColor4f(Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), 0.0);
		glBegin(GL_QUADS);
			glVertex3f(pos.x-1.0, pos.y+1.0, 0.0);
			glVertex3f(pos.x+1.0, pos.y+1.0, 0.0);
			glVertex3f(pos.x+1.0, pos.y-1.0, 0.0);
			glVertex3f(pos.x-1.0, pos.y-1.0, 0.0);
		glEnd();
	}else if(!fired && (!exploded && !done)){
		glColor4f(color[0], color[1], color[2], 0.0);
		glBegin(GL_QUADS);
			glVertex3f(pos.x-4.0, pos.y+4.0, 0.0);
			glVertex3f(pos.x+4.0, pos.y+4.0, 0.0);
			glVertex3f(pos.x+4.0, pos.y-4.0, 0.0);
			glVertex3f(pos.x-4.0, pos.y-4.0, 0.0);
		glEnd();
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
			missiles[k].updatePosition(Point(pos.x + j*10 - i*5, pos.y - i*10 , 0));
			missiles[k].setVelocities(Point(1.5, 1.5, 0));
			k++;
		}
	}

	pos = Point(pos.x + 15, pos.y - 15, 0);

}

void Battery::reload(){
	int i;

	missiles.clear();
	missiles.resize(10);

	pos.x -= 15;
	pos.y += 15;

	init();
}

Missile Battery::fire(Point from, Point goal){
	missiles[0].updatePosition(from);
	missiles[0].setGoal(goal);
	missiles[0].setFrom(from);
	missiles[0].fire();
	Missile m = *missiles.begin();
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

	if(!done)
		for(i = 0; i < n; i++){
			missiles[i].draw();
		}

	if(n == 0 || done){
		Drawing::drawText(pos.x-18, pos.y + 50, 0, 0, 1, 1, "OUT");
	}else if(n <= 3){
		Drawing::drawText(pos.x-18, pos.y + 50, 0, 0, 1, 1, "LOW");
	}
}

/***********************************************
 *            City Member Functions            *
 ***********************************************/

void City::update(float dt){

}

void City::draw(){
	Drawing::drawEllipse(pos.x, pos.y, 0, 0.0, 0.5, 1.0, 30, 20, 8);
	Drawing::drawEllipse(pos.x, pos.y, 1, 0, 0, 1, 18, 10, 8);
}

/***********************************************
 *            Explosion Member Function        *
 ***********************************************/

void Explosion::draw(){
	Random::init();
	Drawing::drawEllipse(pos.x, pos.y, 6, Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), Random::floatInRange(0.0, 1.0), initRadius, initRadius, Random::intInRange(5, 15));
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

	initRadius += signal * 15 * dt;

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

void Button::setSize(int x, int y){
	size.x = x;
	size.y = y;
}

void Button::setText(string text){
	this->text = text;
}

bool Button::clicked(int x, int y){
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
	glColor3f(color[0], color[1], color[2]);

	glBegin(GL_POLYGON);
		glVertex3f(pos.x- size.x, pos.y - size.y, 0);
		glVertex3f(pos.x+ size.x, pos.y - size.y, 0);
		glVertex3f(pos.x+ size.x, pos.y + size.y, 0);
		glVertex3f(pos.x- size.x, pos.y + size.y, 0);
	glEnd();

   	Drawing::drawText(pos.x - size.x/2, pos.y + size.y/3, 0, 0, 0, 1, text.c_str());
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
	Drawing::drawText(pos.x, pos.y, 1, 0, 0, 1, text.c_str());
	Drawing::drawText(pos.x + 150, pos.y, 1, 0, 0, 1, to_string(score).c_str());
}

void Score::update(float dt){

}

void Score::operator=(const Score& p){
	this->pos = p.pos;
	this->text = p.text;
	this->score = p.score;
}
