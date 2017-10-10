/*
  Name:        main.cpp
  Copyright:   Version 1.0
  Authors:      Mateus Coutinho Marim <mateus.marim@ice.ufjf.br>
  				Edmar Philipe Ribeiro <edmarzinho_jf@hotmail.com>
  Last Update:	30/08/2017
  Date:        23/08/2017
  Description: D1 Jogo das bolas
*/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include "GameObjects.h"
#include "Random.h"
#include "utils.h"

#define N_SPHERES 100
#define OBJECTIVE 50

using namespace std;

// Globals
int i, j, lifes = 15, collided = 0;
int width  = 1000;
int height = 500;
float Dt = 0.0, Dti = 0.0;
int score = 0.0f;
float coordinates[2][2]={{0,height},{width,height}};
int n_balls = 0, t_balls = 0, n_itr_collided = 0;
Point mousep;
float max_score = 0.0;
bool moused= false, gameover = false, endgame = false;

/*
t_balls -> bolas que já cairam
n_balls -> numero de bolas que vão começar a cair
Dt -> Intervalo de tempo para cairem mais bolas
Dti -> tempo passado para cair mais bolas
fallen[i] -> Bolinhas que já atingiram o plano inferior na tela
falling[i] -> Bolinhas que estão caindo
http://alunosonline.uol.com.br/matematica/equacao-geral-reta.html
http://brasilescola.uol.com.br/matematica/equacao-reduzida-reta.htm
*/

Sphere player;
vector<Sphere> spheres(N_SPHERES);
vector<bool> falling(N_SPHERES, false), fallen(N_SPHERES, false);

bool PointLineCollision(float x, float y){

    float fx = ((float)((coordinates[0][1]-coordinates[1][1])*x + (coordinates[0][0]*coordinates[1][1] - coordinates[1][0]*coordinates[0][1]))/(-1*(coordinates[1][0]-coordinates[0][0])));
    
   	if(y >= fx)
    	return true;
    else
    	return false;
}
void initBalls(){
	//Inicializa bolas do jogo
	for(i = 0; i < N_SPHERES; i++){
		int y = Random::intInRange(35.0, 200.0);
		int x = Random::intInRange(20.0, width - 20.0);
		int vy = Random::floatInRange(100.0f, 200.0f); 
		int vx = Random::floatInRange(-100.0f, 100.0f);
		float colorRed = Random::floatInRange(0.0, 1.0); 
		float radius = Random::floatInRange(10.0, 40.0);
		spheres[i].updatePosition(Point(x, -y, 0.0f));
		spheres[i].setColor(colorRed, 0.0f, 0.0f);
		spheres[i].setRadius(radius);
		spheres[i].setMass(1.0f);
		spheres[i].setVelocities(Point(vx, vy, 0.0f));
		spheres[i].setAceleration(Point(0.0f, 9.8f, 0.0f));
	}
}

void makeBallsFall(){
	//Número de bolas para cair entre 1 e 5
	n_balls = Random::intInRange(1, 5); 

	//Se o jogador colidiu com mais bolas que o objetivo, o jogo acaba
	if(collided >= OBJECTIVE){
		endgame = true;
	}else{
		t_balls += n_itr_collided;
	}
	
	//Define as n_balls bolinhas que vão cair
	for(i = 0; i < n_balls; i++){
		//Pega um indice aleatório
   		int k = Random::intInRange(0, N_SPHERES); 
   		
   		//Enquanto não acha um indice de uma bolinha que não está caindo ou já atingiu o plano inferior
   		while(falling[k] && !fallen[k]){
   			k = Random::intInRange(0, N_SPHERES);
   		}
   		//A bolinha de indice k é definida como uma das que vão cair
   		falling[k] = true;
	}
}

void idle()
{
 	float t, dt;
 	static float tLast = 0;
 	
 	t = glutGet(GLUT_ELAPSED_TIME);
 	t /= 1000;
 	dt = t - tLast;
 	Dti += dt;
 	
 	if(!gameover && !endgame){
	 	//Se o tempo para uma nova queda de bolinhas foi atingido
	 	if(Dti >= Dt){
	 		//Faça bolinhas cairem
	 		makeBallsFall();
	 		//Define o próximo intervalo de tempo para cairem novas bolinhas
	   		Dt = Random::floatInRange(0.8, 1.5);
	   		Dti = 0.0;
	 	}
	 	n_itr_collided = 0;
	 	//Atualiza a posição das bolinhas pela física do objeto
	 	for(i = 0; i < N_SPHERES; i++){
	 		//Se a bolinha está caindo e ainda não tocou o plano inferior
			if(falling[i] && !fallen[i]){
				spheres[i].update(dt);
			
				Point p = spheres[i].getPosition();
				//Verifica se a bola i atingiu o plano inferior, perde vida se acontecer
				if(Collision::PointLineCollision(coordinates, p.x, p.y+spheres[i].getRadius())){
				//if(abs(p.y - height) < spheres[i].getRadius()){
					falling[i] = false;
					fallen[i] = true;
					n_itr_collided++;
					lifes--;
					//Se vidas = 0, então gameover, perdeu playboy
					if(lifes <= 0) gameover = true;
				}
				//Se as bolas atingem as paredes, a velocidade em x inverte
				if(p.x < spheres[i].getRadius() || p.x > width - spheres[i].getRadius()){
					spheres[i].invertVx();
				}
				//Se uma bola colide com o jogador, ganha pontos e a bola não é desenhada
				if(Collision::SphereCollision(player, spheres[i])){
					fallen[i] = true;
					collided++;
					score += 100.0/spheres[i].getRadius();
				}
			}
		}
	}
	tLast = t;
	
   	glutPostRedisplay();
}

void init(void)
{
	Random::init();
	glClearColor (0.5, 0.5, 0.5, 0.0);
	Point p(width/2, height/2, 0);

	//Configurações iniciais do jogador (Posição, raio e cor).
	player.updatePosition(p);
	player.setColor(0.0f, 0.0f, 1.0f);
	player.setRadius(20.0f);

	//Faz um número aleatório de bolas começarem a cair
	makeBallsFall();

	Dt = Random::floatInRange(0.1, 1.0);

	//Inicializa bolas do jogo
	initBalls();

	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
}

void output(int x, int y, float r, float g, float b, int font, const char *string)
{
	glColor3f( r, g, b );
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);

	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
	}
}

void display(void)
{
	float sphereSize = 100.0;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0.0, width, height, 0.0, -sphereSize, sphereSize);


	//Desenha o jogador
	player.draw();
	if(!gameover && !endgame){
		//Desenha as bolinhas
		for(i = 0; i < N_SPHERES; i++){
			//Se estão caindo e não atingiram o plano inferior
			if(falling[i] && !fallen[i])
				spheres[i].draw();
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	output(50, 50, 0, 0, 0, 1, string("Score: " + to_string(score)).c_str());
	output(230, 50, 0, 0, 0, 1, string("Lifes: " + to_string(lifes)).c_str());
	string str = string("Goal: Get ") + to_string(OBJECTIVE);
	output(50, 70, 0, 0, 0, 1, string(str + " balls").c_str());
	output(230, 70, 0, 0, 0, 1, string("Colleted Balls: " + to_string(collided)).c_str());
	
	if(lifes == 0 || endgame){
		output(700, 50, 0, 0, 0, 1, "Press b to restart the game...");
	}
	
	if(endgame && lifes > 0){
		output(width/2, height/2, 0, 0, 0, 1, "Muitos Parabains!");
	}

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	width = w;
	height = h;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard (unsigned char key, int x, int y)
{
	if(tolower(key) == 27) exit(0);
	if(tolower(key) == 'b' && (gameover || endgame)){
		gameover = false;
		endgame = false;
		lifes = 9;
		score = 0;
		collided = 0;
		initBalls();
	}
}

// Motion callback
void motion(int x, int y )
{	
	
	if(moused){
		Point p = Point(x-mousep.x, y-mousep.y, 0.0f);
		
		//
		if(p.x < player.getRadius()){
			p.x =player.getRadius();
		}if(p.x > width - player.getRadius()){
			p.x = width - player.getRadius();
		}
		if (p.y < player.getRadius()){
		    p.y = player.getRadius();
		}
		if (p.y > height - player.getRadius()){
		    p.y =  height - player.getRadius();
		
		}

		player.updatePosition(p);

		glutPostRedisplay();
	}
}
void mouse(int button, int state, int x, int y)
{
	
	if (button == GLUT_LEFT_BUTTON)
	{
		Point p = player.getPosition();
	
		float dist = sqrt(pow(p.x-x, 2) + pow(p.y-y,2));
	
		if (state == GLUT_DOWN) //botao esquerdo do mouse pressionado
		{// Conversao de escala dos eixos do mouse para valores do ortho                    
		    //so seleciona o jogador clicando dentro do seu raio
		    if(dist <= 20.0f){
			 	mousep = Point(x-p.x, y-p.y, 0.0f);
			    moused = true;
			}	
		}
		else //botao esquerdo do mouse solto
		{
		    moused = false;
		}
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (width, height);
	glutInitWindowPosition (100, 100);
	glutCreateWindow("Get the balls simulator V1.0");
	
	init ();
	glutDisplayFunc(display);
//	glutReshapeFunc(reshape);
	glutMotionFunc( motion );
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutMainLoop();
	
	return 0;
}
