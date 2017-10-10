#ifndef GAMEOBJECTS_INCLUDED
#define GAMEOBJECTS_INCLUDED

#include <cmath>

//Estrutura para usar um ponto
struct Point{
	float x, y, z;
	
	Point(){
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	
	Point(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	float distance(Point p){
		return sqrt(pow(p.x - x, 2) + pow(p.y - y, 2) + pow(p.z - z, 2));
	}
	
	void operator=(const Point&);
	friend std::ostream &operator<<( std::ostream &output, const Point &point );
};

//Classe base para os objetos de jogo
class Object{
protected:
	float mass;
	Point vel, acel;
	//Cor RGB do objeto
	float color[3];
	//Posição atual e anterior do objeto
	Point pos, lastPos;
	
public:
	Object();

	//Define a massa
	void setMass(float mass);
	//Define a velocidades em x, y e z
	void setVelocities(Point vel);
	//Define a aceleração em x, y e z
	void setAceleration(Point acel);
	//Define a cor RGB do objeto
	void setColor(float r, float g, float b);
	//Atualiza variaveis do objeto
	virtual void update(float dt) = 0;
	//Função para desenhar um objeto
	virtual void draw() = 0;
};

class Sphere: public Object{
private:
	Point pos;
	float radius;
	
public:
	Sphere();
	Sphere(Point pos, float radius);
	
	void invertVx();
	//Atualiza a posição do objeto
	void updatePosition(Point pos);
	//Retorna a posição atual do objeto
	Point getPosition();
	//Define o raio da esfera
	void setRadius(float radius);
	//Retorna o raio da esfera
	float getRadius();
	void update(float dt);
	//Desenha a esfera
	void draw();
};

#endif
