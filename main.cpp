#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <GL/glut.h>

#include <cstring>
#include "GameObjects.h"
#include "Random.h"
#include "utils.h"

using namespace std;

int width = 1080, height = 720, level = 1, launchedEnemies = 0, nMissilesRain = 0;
int score = 0, score1 = 7500, citiesExplodedItr = 0, enemyExplodedItr = 0, enemyExplodedNum = 0, batteriesExplodedItr = 0;
int menu = 0, citiesExplodedNum = 0;
int nMissiles = 10;
int distOrigem = 100;
string playerName;
Button startGame, scoreScreen, back;
float Dt;
bool endLevel, fullscreen = false, endGame = false, scoreSaved = false, paused = false;

GLfloat cor_luz[]		= { 1.0f, 1.0f, 1.0f, 1.0};
GLfloat cor_luz_spec[]	= { 1.0f, 1.0f, 1.0f, 1.0};
GLfloat cor_luz_amb[]	= { 0.1, 0.1, 0.1, 1.0};
GLfloat posicao_luz[]   = { 50.0, 50.0, 50.0, 1.0};

vector<Battery> batteries(3);
vector<City> cities(6);
vector<Missile> firedMissiles;
vector<Missile> enemyMissiles(10);
vector<bool> enemyLaunched(enemyMissiles.size(), false);
vector<Explosion> explosions;
vector<Score> scores;

GLsizei MOUSEx=width/2, MOUSEy=height/2;
GLfloat SIDE=8; // Tamanho da mira

bool compare_score(Score a, Score b){
	return a.score < b.score;
}

void restart_game();
void init_batteries();
void init_cities();
void init_enemies();
void init_game();
void init_scores();
void init();
void drawTerrain();
void output(int x, int y, float r, float g, float b, int font, const char *string);
void saveScore();
void readScores();
void display();
void launchEnemyMissiles(int n);
void verifyExplosionCollision(vector<Object> &obj);
void idle();
void reshape (int w, int h);
void keyboard (unsigned char key, int x, int y);
void motion(int x, int y );
int findNearestBattery(int x, int y);
void mouse(int button, int state, int x, int y);
void specialKey(int key, int x, int y);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (width, height);
	glutInitWindowPosition (100, 100);
	glutCreateWindow("Autist Missile Combat");

	init ();
	glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutPassiveMotionFunc( motion );
	glutKeyboardFunc(keyboard);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMouseFunc(mouse);
	glutSpecialFunc(specialKey);
	glutIdleFunc(idle);
	glutMainLoop();


	return 0;
}

void restart_game(){
	int i;

	for(i = 0; i < batteries.size(); i++){
		batteries[i].reload();
	}

	enemyMissiles.clear();
	cities.clear();
	playerName.clear();
	enemyLaunched.clear();
	explosions.clear();
	cities.resize(6);
	enemyMissiles.resize(10);
	enemyLaunched.resize(10, false);

	init_cities();
	init_enemies();

	endGame = scoreSaved = false;
	nMissilesRain = 0;
	citiesExplodedNum = 0;
	enemyExplodedNum = 0;
	launchedEnemies = 0;
	score = 0;
	level = 1;
}

//Inicializa baterias
void init_batteries(){
	int i = 0, n = batteries.size(), pos;
	float r = Random::floatInRange(0, 1), g = Random::floatInRange(0, 1), b = Random::floatInRange(0, 1);

	for(i = 0; i < n; i++){

		if(i == 0) pos = 60; else if(i == 1) pos = -80; else pos = -240;
    batteries[i].setColor(r,g,b);
		batteries[i].updatePosition(Point(i*(width/2 + 80) + pos ,height - 100, 0));
		batteries[i].init();
	}
}

//Inicializa cidades
void init_cities(){
	int i, n = cities.size();

	for(i = 0; i < n/2; i++){
		cities[i].updatePosition(Point((i+1)*(width/2-60)/3, height - 90, 0));
	}
	for(i = n/2; i < n; i++){
		cities[i].updatePosition(Point((i - n/2 + 1)*(width - 50 - width/2)/3 + width/2 - 90, height - 90, 0));
	}
}

//Inicializa todos mísseis inimigos da fase
void init_enemies(){
	int i, n = enemyMissiles.size();
	float r = Random::floatInRange(0, 1), g = Random::floatInRange(0, 1), b = Random::floatInRange(0, 1);

	for(i = 0; i < n; i++){
		Point p = Point(Random::intInRange(0, width), -5, 0);

		enemyMissiles[i].setColor(r, g, b);
		enemyMissiles[i].updatePosition(p);
		enemyMissiles[i].setFrom(p);
		enemyMissiles[i].setVelocities(Point(0.05 + 0.02 * (level-1), 0.05 + 0.02 * (level-1), 0));
	}

	launchEnemyMissiles(4);
}


//Lança n mísseis inimigos
void launchEnemyMissiles(int n){
	int i, k = (launchedEnemies + n < enemyMissiles.size())?launchedEnemies + n:enemyMissiles.size(), j, batOrCity = 0, id;
	Point goal, from;
	vector<City*> notDoneCity;
	vector<Battery*> notDoneBat;

	//Pega apenas as cidades que não foram destruidas
	for(i = 0; i < cities.size(); i++){
		if(!cities[i].isDone()){
			notDoneCity.push_back(NULL);
			notDoneCity[notDoneCity.size()-1] = &cities[i];
		}
	}

	//Pega apenas as baterias que não foram destruidas
	for(i = 0; i < batteries.size(); i++){
		if(!batteries[i].isDone()){
			notDoneBat.push_back(NULL);
			notDoneBat[notDoneBat.size()-1] = &batteries[i];
		}
	}

	if(notDoneCity.size() == 0) return ;

	//Lança n mísseis
	for(i = launchedEnemies; i < k; i++){
		if(!enemyLaunched[i]){
			batOrCity = Random::intInRange(0, 1);

			id = (batOrCity)?Random::intInRange(0, notDoneCity.size()-1):Random::intInRange(0, notDoneBat.size()-1);
			goal = (batOrCity)?notDoneCity[id]->getPosition():notDoneBat[id]->getPosition();

			if(!batOrCity){
				goal.y -= 15;
			}

			enemyMissiles[i].setGoal(goal);
			enemyMissiles[i].fire();
			enemyLaunched[i] = true;
		}
	}
	launchedEnemies = k;
	nMissilesRain++;
}

void init_game(){
	//Inicia o seed do gerador aleatório
	Random::init();

	init_batteries();
	init_cities();
	init_enemies();
}

void init_scores(){
	int i, n;

	readScores();
	n = scores.size();

	//Seta posições dos scores
	for(i = 0; i < scores.size(); i++){
		scores[i].updatePosition(Point(width/2-50, 150 + 20*i, 0));
	}
}

void init(void)
{	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
	glEnable(GL_CULL_FACE); // Habilita Backface-Culling  
	glEnable(GL_LIGHTING);                 // Habilita luz
	glEnable(GL_LIGHT0);                   // habilita luz 0
	glEnable(GL_NORMALIZE);
	// Posicao da fonte de luz. Ultimo parametro define se a luz sera direcional (0.0) ou tera uma posicional (1.0)
	glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
	
	switch(menu){
		case 0:
			startGame.updatePosition(Point(50, 0, 0));
			startGame.setSize(100, 19);
			startGame.setColor(0.0, 1.0, 0.0);
			startGame.setText("Start Game");

			scoreScreen.updatePosition(Point(width/2, height/2 + 50, 0));
			scoreScreen.setSize(100, 19);
			scoreScreen.setColor(0.0, 1.0, 0.0);
			scoreScreen.setText("Score");

			back.updatePosition(Point(width-50, height - 50, 0));
			back.setSize(50, 19);
			back.setColor(0.0, 1.0, 0.0);
			back.setText("Beck");
			break;
		case 1:

			break;
		case 2:

			break;
	}
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
}

template <class T>
void verifyExplosionCollision(vector<T> &obj, int &n){
	int i, j, size = obj.size();

	for(i = 0; i < size; i++){
		if(!obj[i].isDone()){
			//Verifica colisão dos mísseis com as explosões
			for(auto itr = explosions.begin(); itr != explosions.end(); itr++){
				if((*itr).isColliding(obj[i])){
					n++;
					obj[i].explode();
					break;
				}
			}
			//Cria uma nova explosão se o míssel estiver explodindo
			if(obj[i].isExploding()){
				Explosion e;

				e.updatePosition(obj[i].getPosition());
				explosions.push_back(e);
			}
		}
	}
}

void idle()
{
	int i, nmissiles = firedMissiles.size(), n, numEnemyMissiles = enemyMissiles.size();
 	int j, k, size;
 	bool someExplosion = false, batOrCity = false;
 	float t, dt;
 	static float tLast = 0;


 	t = glutGet(GLUT_ELAPSED_TIME);
 	t /= 1000;

 	if(paused){
 		tLast = t;
 		return ;
 	}

 	dt = t - tLast;

	switch(menu){
		case 0:

			break;
		case 1:
			Dt += dt;
			endLevel = true;
			endGame = true;

			//A cada 3.5 segundos inicia uma chuva de mísseis inimigos
			if(Dt >= 3.5 && launchedEnemies < numEnemyMissiles){
				if(nMissilesRain > 0){
					n = 4/2*nMissilesRain;
				}else{
					n = 4;
				}
				launchEnemyMissiles(n);
				Dt = 0.0f;
			}


			//Atualiza os mísseis aliados lançados
			for(i = 0; i < nmissiles; i++){

				firedMissiles[i].update(dt);

				if(firedMissiles[i].isExploding()){
					Explosion e;

					e.updatePosition(firedMissiles[i].getPosition());
					explosions.push_back(e);
				}
			}

			//Atualiza os mísseis inimigos lançados
			for(i = 0; i < numEnemyMissiles; i++){
				if(enemyLaunched[i] && !enemyMissiles[i].isDone()){
					enemyMissiles[i].update(dt);

				}
			}

			//Verifica se algum objeto está sendo atingido pelas explosões
			verifyExplosionCollision(enemyMissiles, enemyExplodedItr);
			verifyExplosionCollision(cities, citiesExplodedItr);
			verifyExplosionCollision(batteries, batteriesExplodedItr);

			score += enemyExplodedItr * 25;
			enemyExplodedNum += enemyExplodedItr + citiesExplodedItr + batteriesExplodedItr;
			citiesExplodedNum += citiesExplodedItr;
			enemyExplodedItr = 0;
			citiesExplodedItr = 0;
			batteriesExplodedItr = 0;

			//Verifica se alguma explosão terminou e remove da lista
			for(auto itr = explosions.begin(); itr != explosions.end(); ){
				(*itr).update(dt);

				if((*itr).isFinished()){
					itr = explosions.erase(itr);
				}else{
					itr++;
				}
			}

			//Verifica se todos mísseis inimigos explodiram (fim do level)
			for(i = 0; i < nMissiles; i++){
				if(!enemyMissiles[i].isDone()){
					endLevel = false;
				}
			}

			//Verifica se todas cidades foram destruídas (fim de jogo)
			for(i = 0; i < cities.size(); i++){
				if(!cities[i].isDone()){
					endGame = false;
				}
			}

			//Computa a pontuação no level e inicia o próximo nivel
			if(endLevel && explosions.size() == 0){
				float r = Random::floatInRange(0, 1), g = Random::floatInRange(0, 1), b = Random::floatInRange(0, 1);
				firedMissiles.clear();
				explosions.clear();
				for(i = 0; i < batteries.size(); i++){
					score += batteries[i].ammo() * 5;
					batteries[i].setColor(r,g,b);
					batteries[i].reload();
				}

				//reinicia os mísseis
				enemyMissiles.clear();
				enemyMissiles.resize(++nMissiles);
				enemyLaunched.resize(nMissiles, 0);
				for(i = 0; i < nMissiles; i++){
				//	Point p = Point(Random::intInRange(0, width), -5, 0);
					enemyLaunched[i] = 0;

				}
				init_enemies();
				for(i = 0; i < cities.size(); i++){
					if(!cities[i].isDone()){
						score += 100;
					}
				}

				level++;
				launchedEnemies = 0;
			 	enemyExplodedNum = 0;
				nMissilesRain = 0;
				endLevel = true;
			}

			break;
		case 2:
			break;
		default:
			break;
	}
	tLast = t;

   	glutPostRedisplay();
}

//desenha o cursor
void drawSquade()
{
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    	glVertex3f(MOUSEx, (MOUSEy-SIDE), 0);
        glVertex3f(MOUSEx, (MOUSEy+SIDE), 0);
        glVertex3f((MOUSEx-SIDE), MOUSEy, 0);
        glVertex3f((MOUSEx+SIDE), MOUSEy, 0);
    glEnd();
    glFlush();
}

//desenha o terreno
void drawTerrain(){
	glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
		glVertex3f(0, height - height/6, -10);
		glVertex3f(width, height - height/6, -10);
		glVertex3f(width, height, -10);
		glVertex3f(0, height, -10);
	glEnd();

}

void saveScore(){
	ofstream output("scores", ios_base::app | ios_base::out);

	cout << "Enter your initials: ";
	cin >> playerName;
	output << playerName << " " << score << endl;

	output.close();
}

void readScores(){
	if(!scores.empty()) scores.clear();
	int points;
	string name;
	ifstream input("scores");

	if(!input.is_open()){
		cout << "Could not open scores." << endl;
		return;
	}

	while(input >> name >> points){
		Score s;

		s.setText(name);
		s.setScore(points);
		scores.push_back(s);
	}

	//Ordena os scores do mais alto para o mais baixo
	sort(scores.rbegin(), scores.rend(), compare_score);

	input.close();
}

void display(void)
{
	int i, ncities = cities.size(), nbatteries = batteries.size(), nmissiles = firedMissiles.size();
	int numEnemyMissiles = enemyMissiles.size(), n;

	/*glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0.0, width, height, 0.0, -30, 30);
	*/
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, (GLfloat) width/(GLfloat) height, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt (0.0, 0.0, distOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	drawSquade();
	//glutSolidSphere(20, 20, 20);
	switch(menu){
		case 0:
			startGame.draw();
			scoreScreen.draw();
			break;
		case 1:{
			drawTerrain();

			for(i = 0; i < nbatteries; i++){
				batteries[i].draw();
			}

			for(i = 0; i < ncities; i++){
				if(!cities[i].isDone())
					cities[i].draw();
			}

			for(i = 0; i < nmissiles; i++){
				firedMissiles[i].draw();
				firedMissiles[i].drawTarget(SIDE);
			}

			for(i = 0; i < numEnemyMissiles; i++){
				if(enemyLaunched[i] && !enemyMissiles[i].isDone())
					enemyMissiles[i].draw();
			}

			for(auto itr = explosions.begin(); itr != explosions.end(); itr++){
				(*itr).draw();
			}

			string levelTag = "Level " + to_string(level);
			Drawing::drawText(width/3, 50, 1, 0, 0, 1, string(to_string(score)).c_str());
			Drawing::drawText(2*width/3, 50, 1, 0, 0, 1, string(to_string(score1)).c_str());
			Drawing::drawText(20, 50, 0, 1, 0, 1, levelTag.c_str());

			if(endGame){
				Drawing::drawText(width/2, height/2, 1, 0, 0, 1, "THE END");
				Drawing::drawText(width/2, height/2 + 50, 0, 1, 0, 1, "Enter your name initials in the terminal.");
			}
			break;
			}
		case 2:
			n = (scores.size() > 10)?10:scores.size();

			Drawing::drawText(width/2 -30, 100, 0, 1, 0, 1, "SCORES");

			for(auto itr = scores.begin(); itr != (scores.begin()+10); itr++){
				(*itr).draw();
			}

			back.draw();
			break;
		default:
			break;
	}

	glutSwapBuffers();
	if(endGame && !scoreSaved){
		saveScore();
		scoreSaved = true;
	}
}


void reshape (int w, int h){

}

// Motion callback
void motion(int x, int y )
{
	MOUSEx=x;
	MOUSEy=y;

	if (MOUSEy > 550 && menu == 1){
	 MOUSEx=x;
	 MOUSEy=550;
	}
}

//Retorna o id da bateria mais próxima da posição (x, y)
int findNearestBattery(int x, int y){
	int i, n = batteries.size();
	float dist = 100000, batId = -1, d = -1;
	Point p;

	for(i = 0; i < n; i++){
		if(batteries[i].isEmpty() || batteries[i].isDone()) continue;

	    p = batteries[i].getPosition();
		d = p.distance(Point(x, y, 0));

		if(d < dist){
			dist = d;
			batId = i;
		}
	}

	return batId;
}

void mouse(int button, int state, int x, int y){
	int id;
	Point p;

	MOUSEx=x;
	MOUSEy=y;

	if(MOUSEy > 550 && menu == 1){
		MOUSEx=x;
		MOUSEy=550;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		switch(menu){
			case 0:
				if(startGame.clicked(MOUSEx, MOUSEy)){
					menu = 1;
					init_game();
				}

				if(scoreScreen.clicked(MOUSEx, MOUSEy)){
					menu = 2;
					init_scores();
				}

				break;
			case 1:
				if(MOUSEy > 550) break;
				id = findNearestBattery(MOUSEx, MOUSEy);

				if(id == -1) return ;

				p = batteries[id].getPosition();

				p.y -= 20;
				if(!batteries[id].isEmpty() && !batteries[id].isDone()){
					Missile m = batteries[id].fire(p, Point(MOUSEx, MOUSEy, 0));

					firedMissiles.push_back(m);
				}

				break;

			case 2:
				if(back.clicked(MOUSEx, MOUSEy)){
					menu = 0;
				}
				break;

			default:
				break;
		}
	}
}

void keyboard (unsigned char key, int x, int y)
{
	switch(key){
		case 27:
			exit(0);
			break;
		case 32:
			if(!paused){
				paused = true;
			}else paused = false;
			break;
		case '+' :
			distOrigem--;
			cout << distOrigem << endl;
	    	// if(distOrigem<20) distOrigem=20;
		break;
		case '-' :
			distOrigem++;
			cout << distOrigem << endl;
 			//if(distOrigem>180) distOrigem=180;
		break;
		case 'r':
			restart_game();
			break;
		case 'm':
			menu = 0;
			restart_game();
			break;
		default:
			break;
	}
}

void specialKey(int key, int x, int y){
	switch(key){
		case GLUT_KEY_F11:
			if(!fullscreen){
				glutFullScreen();
				fullscreen = true;
			}else{
				glutReshapeWindow(width, height);
				fullscreen = !fullscreen;
			}
			break;
		default:
			break;
	}
}
