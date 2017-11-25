#ifndef GAMEOBJECTS_INCLUDED
#define GAMEOBJECTS_INCLUDED

#include <vector>
#include <cmath>
#include <string>

#include "3DPlyModel.h"
#include "Point.h"
#include "utils.h"
#include "glcTexture.h"

//Classe base para os objetos de jogo
class Object{
protected:
	float mass;
	Point vel, acel;
	//Cor RGB do objeto
	float color[3], scale[3];
	bool exploded = false, done = false;
	//Posição atual e anterior do objeto
	Point pos, lastPos;
	PlyModel model_3d;
	
public:
	Object();
	
	void load3DModel(const char* file);
	void invertVx();
	//Atualiza a posição do objeto
	void updatePosition(Point pos);
	//Retorna a posição atual do objeto
	Point getPosition();
	//Define a massa
	void setMass(float mass);
	//Define a velocidades em x, y e z
	void setVelocities(Point vel);
	//Define a aceleração em x, y e z
	void setAceleration(Point acel);
	//Define a cor RGB do objeto
	void setColor(float r, float g, float b);
	void setScale(float x, float y, float z);
	void explode();
	bool isExploding();
	bool isDone();
	
	PlyModel getModel(){return model_3d;}
	
	//Atualiza variaveis do objeto
	virtual void update(float dt){};
	//Função para desenhar um objeto
	virtual void draw(){};
};

class Terrain : public Object {
	public:
	void draw();
	private:
};

class Missile : public Object {
	public: 
		Missile() : goal_pos(-1, -1, -1) {};
		void setGoal(Point pos);
		void setFrom(Point from);
		void fire();
		void reset();
		std::vector<Missile> divide(int level);
		void drawTarget(float sidex, float sidey);
		void draw();
		void update(float dt);
		
		void operator=(const Missile&);
	private:
		float tx, ty;
		bool fired = false;
		Point goal_pos, from;
};

class Battery : public Object {
	public:
		Battery();
		void init();
		Missile fire(Point from, Point goal);
		bool isEmpty();
		int ammo();
		void reload();
		void update(float dt);
		void draw();
	private:
		bool empty = false;
		std::vector<Missile> missiles;
};

class City : public Object {
	public:
		void update(float dt);
		void draw();
	private:
};

class Explosion : public Object {
	public:
		bool isFinished();
		void update(float dt);
		bool isColliding(Object obj);
		void draw();
		float getPercent(){return percent;}
	private:
		GLUquadric *quad;
		bool finished = false;
		int signal = 1; 
		float percent = 0.0;
		float finalRadius = 0.05, initRadius = 0.01;
};

class Button : public Object {
	public:
		void setSize(float x, float y);
		void setText(std::string text);
		bool clicked(float x, float y);
		void draw();
		void update(float dt);
	private:
		std::string text;
		Point size;
};

class Score : public Object {
	public:
		int score = 0;
				
		void setText(std::string text);
		void setScore(int score);
		void draw();
		void update(float dt);
		void operator=(const Score&);
	private:
		std::string text;

};

#endif
