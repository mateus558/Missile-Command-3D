#ifndef GAMEOBJECTS_INCLUDED
#define GAMEOBJECTS_INCLUDED

#include <vector>
#include <cmath>
#include <string>
#include <AL/al.h>
#include <AL/alc.h>

#include "3DPlyModel.h"
#include "Point.h"
#include "Random.h"
#include "utils.h"
#include "glcTexture.h"

//Classe base para os objetos de jogo
class Object{
protected:
	float mass;
	Point vel, acel;
	//Cor RGB do objeto
	float color[3], scale[3];
	bool exploded = false, done = false, gouraud = false;
	//Posição atual e anterior do objeto
	Point pos, lastPos;
	PlyModel model_3d;
	
public:
	Object();
	
	void load3DModel(const char* file);
	void useGouraud(bool flag){gouraud = flag; model_3d.activateGouraud(flag);}
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
		Missile() : goal_pos(-1, -1, -1) {
			this->load3DModel("Models/Missile.ply");
		
		
		}
		void setGoal(Point pos);
		void setFrom(Point from);
		void fire();
		void reset();
		std::vector<Missile> divide(int level);
		void drawTarget(float sidex, float sidey);
		void draw();
		void isEnemy(bool flag);
		void update(float dt);
		
		void operator=(const Missile&);
	private:
		float tx, ty, angle = 0.0;
		bool fired = false, enemy = true, hasAngle = false;
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
		Explosion(){z = Random::floatInRange(0.1, 0.2);}
		bool isFinished();
		void update(float dt);
		bool isColliding(Object obj);
		void draw();
		float getPercent(){return percent;}
	private:
		bool finished = false;
		int signal = 1; 
		float percent = 0.0;
		GLfloat z = 0.0;
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

class Background : public Object {
	public:
		void loadTexture(const char* file);
		void update(float dt){}
		void draw();
	private:
		bool translated = false;
		GLuint texture;
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
