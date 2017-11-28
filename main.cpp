#define GL_GLEXT_PROTOTYPES
#define PEI_SOUND 0

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>

#include "GameObjects.h"
#include "Random.h"
#include "utils.h"
#include "3DPlyModel.h"
#include "glcTexture.h"
#include "glcSound.h"
#include "skybox.h"

using namespace std;

int width = 1080, height = 760, level = 1, launchedEnemies = 0, nMissilesRain = 0;
int score = 0, score1 = 7500, citiesExplodedItr = 0, enemyExplodedItr = 0, enemyExplodedNum = 0, batteriesExplodedItr = 0;
int menu = 0, citiesExplodedNum = 0;
int nMissiles = 10;
int distOrigem = 80;
string playerName;
Button startGame, scoreScreen, back;
float Dt;
bool endLevel, fullscreen = false, endGame = false, scoreSaved = false, paused = false, isOrtho = false;
float angleCam = 0.0f;
Point minCoord(0.0f, 0.0f, -100.0f), maxCoord(1.0f, 1.0f, 100.0f);
Point camScale(2, 2, 1);
Point eye(0.56, 0.4, -0.41), center(.56, 0.4, .74504);
GLfloat cor_luz[]		= { 1.0f, 1.0f, 1.0f, 1.0};
GLfloat posicao_luz[]   = { maxCoord.x/3, maxCoord.y/3, 1.0, 1.0};

float map[16];
float forwardz = 0.0, terrainz = -.5;
Point skypos(0.869999, -0.64, -1.4);
Point skyscale(.91, .76, -1.6);
Point skyrot(0, 0, 0);

glcSound *sound = NULL;
glcTexture *textureManager = NULL;
SkyBox skybox("Models/skybox.ply");
Terrain terrain;
Background menu_back, score_back;

vector<Battery> batteries(3);
vector<City> cities(6);
vector<Missile> firedMissiles;
vector<Missile> enemyMissiles(10);
vector<bool> enemyLaunched(enemyMissiles.size(), false);
vector<Explosion> explosions;
vector<Score> scores;

GLfloat MOUSEx=maxCoord.x/2, MOUSEy=maxCoord.y/2;
GLfloat SIDEX = 10, SIDEY = 10; // Tamanho da mira

bool compare_score(Score a, Score b){
	return a.score < b.score;
}

bool sort_explosion_tex(string a, string b){
	int i1 = a.find(string(".")), i2 = b.find(string("."));
	string c = a, d = b;
	
	c.erase(i1, c.size());
	d.erase(i2, d.size());
	
	return stoi(c) < stoi(d);
}

float convert_range(float amin, float amax, float bmin, float bmax, float input);
void restart_game();
void init_batteries();
void init_cities();
void init_enemies();
void init_game();
void init_scores();
void init();
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

void setMaterial(void)
{
	GLfloat objeto_ambient[]   = { 0.19225, 0.19225, 0.19225, 1.0 };
	GLfloat objeto_difusa[]    = { 0.50754, 0.50754, 0.50754, 1.0 };
	GLfloat objeto_especular[] = { 0.508273, 0.508273, 0.508273, 1.0 };
	GLfloat objeto_brilho[]    = { 0.4 * 128.0};

	// Define os parametros da superficie a ser iluminada
	glMaterialfv(GL_FRONT, GL_AMBIENT, objeto_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, objeto_difusa);
	glMaterialfv(GL_FRONT, GL_SPECULAR, objeto_especular);
	glMaterialfv(GL_FRONT, GL_SHININESS, objeto_brilho);
}

float convert_range(float amin, float amax, float bmin, float bmax, float input){
	return bmin + ((bmax - bmin) / (amax - amin)) * (input - amin);
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
	int i = 0, n = batteries.size();
	float x, y, z, pos, v = 1;
	float r = Random::floatInRange(0, 1), g = Random::floatInRange(0, 1), b = Random::floatInRange(0, 1);

	for(i = 0; i < n; i++){
		x = y = 0;

		if(i == 0) pos = convert_range(0, width, minCoord.x, maxCoord.x, 60); else if(i == 1) pos = convert_range(0, width, minCoord.x, maxCoord.x, -80); else pos = convert_range(0, width, minCoord.x, maxCoord.x, -240);
		if(i == 0) x += .02; else if(i == 1) x -= .05; else x -= .228;

		x += convert_range(0, width, minCoord.x, maxCoord.x, i*(width/2 + 80) + pos);
		y += convert_range(0, height, minCoord.y, maxCoord.y, height - 100);

		batteries[i].setVelocities(Point(v, v, 0));
		batteries[i].updatePosition(Point(x, y, 0));
		batteries[i].init();
		batteries[i].load3DModel("Models/PAC3.ply");
		batteries[i].setColor(1, 1, 1);
		batteries[i].setScale(0.05, 0.05, 0.05);
	}
}

//Inicializa cidades
void init_cities(){
	int i, n = cities.size();
	float x, y;

	for(i = 0; i < n/2; i++){
		x = convert_range(0, width, minCoord.x, maxCoord.x, (i+1)*(width/2-60)/3) - .025;
		y = convert_range(0, height, minCoord.y, maxCoord.y, height - 90);
		cities[i].updatePosition(Point(x, y, 0));
	}
	for(i = n/2; i < n; i++){
		x = convert_range(0, width, minCoord.x, maxCoord.x, (i - n/2 + 1)*(width - 50 - width/2)/4 + width/2 - 90) + 0.08;
		y = convert_range(0, height, minCoord.y, maxCoord.y, height - 90);
		cities[i].updatePosition(Point(x, y, 0));
	}
	for(i = 0; i < n; i++){
		cities[i].load3DModel("Models/city.ply");
		cities[i].setColor(1, 1, 1);
		cities[i].setScale(.03, .03, .03);
	}
	
}

//Inicializa todos mísseis inimigos da fase
void init_enemies(){
	int i, n = enemyMissiles.size();
	float r = Random::floatInRange(0, 1), g = Random::floatInRange(0, 1), b = Random::floatInRange(0, 1);

	for(i = 0; i < n; i++){
		Point p = Point(Random::floatInRange(0, maxCoord.x), -.1, 0);

		enemyMissiles[i].setColor(r, g, b);
		enemyMissiles[i].updatePosition(p);
		enemyMissiles[i].setFrom(p);
		enemyMissiles[i].setVelocities(Point(0.03 + 0.02 * (level-1), 0.03 + 0.02 * (level-1), 0));
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
				goal.y -= convert_range(0, height, minCoord.y, maxCoord.y, 15);
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
		scores[i].updatePosition(Point(maxCoord.x/2-.1, 0.3 + 0.05*i, 0));
	}
}

void init(void)
{
	float c = 0.0;
	int i;
	vector<string> exp_tex;
	string path("Textures/Explosion/");
	
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "failed to initialize GLEW!" << endl;
	}
	std::cout << "Using GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);               // Habilita Z-buffer
	glEnable(GL_CULL_FACE); // Habilita Backface-Culling
	glEnable(GL_LIGHTING);                 // Habilita luz
	glEnable(GL_LIGHT0);                   // habilita luz 0
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

	glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);

	SIDEX = convert_range(0, width, minCoord.x, maxCoord.x, SIDEX);
	SIDEY = convert_range(0, height, minCoord.y, maxCoord.y, SIDEY);

	sound = new glcSound();
    sound->SetNumberOfSounds(2);
    sound->AddSound(PEI_SOUND, "Sound/pei.wav");

	textureManager = new glcTexture();							//Inicializa o gerenciador de texturas
	textureManager->SetNumberOfTextures(21);					//Seta o número de texturas a serem gerenciadas
	textureManager->SetWrappingMode(GL_REPEAT);					//Seta o modo de wrapping de texturas
	
	/********************************************
	 * 		Início da criação de texturas		*
	 ********************************************/
	
	textureManager->CreateTexture("Textures/PAC.png", 0);	
	textureManager->CreateTexture("Textures/city.png", 1);	
	textureManager->CreateTexture("Textures/terrain.png", 2);
	textureManager->CreateTexture("Textures/exp2_1.png", 3);
	textureManager->CreateTexture("Textures/missile.png", 20);
	
	//Pega as texturas das explosões
	exp_tex = list_datasets(false);
	sort(exp_tex.begin(), exp_tex.end(), sort_explosion_tex);	//ordena os estados da explosão
	for(i = 4; i < 20; i++, c += 6.25){
		textureManager->CreateTexture(string(path + exp_tex[i-4]).c_str(), i);
		map[i-4] = c;	//Mapeia cada textura a um estado da explosão
	}
	
	/********************************************
	 * 		 	Fim da criação de texturas		*
	 ********************************************
	 * 	    Início da configuração do terreno	*
	 ********************************************/
	
	terrain.useGouraud(false);
	cout << "Loading Terrain..." << endl;
	terrain.load3DModel("Models/terrain.ply");
	cout << "Terrain loaded..." << endl;
	terrain.updatePosition(Point(0.5, .7, 0));
	terrain.setScale(0.65, 0.65, 0.5);
	terrain.setColor(.5, .5, 0);
	
	/********************************************
	 * 		 Fim da configuração do terreno		*
	 ********************************************
	 * 	    Início da configuração da skybox	*
	 ********************************************/
	
	skybox.load_skybox("Textures/Skybox/skybox_texture.png");	//Carrega skybox	
	//skybox.center_of(&terrain);
	skybox.center_of(eye);

	/********************************************
	 * 		 Fim da configuração do skybox		*
	 ********************************************
	 * 	    Início da configuração dos menus	*
	 ********************************************/

	menu_back.loadTexture("Textures/menu.png");
	score_back.loadTexture("Textures/score.png");
	switch(menu){
		case 0:
			startGame.updatePosition(Point(maxCoord.x/2, maxCoord.y/2, -0.01));
			startGame.setSize(.07, .03);
			startGame.setColor(0.0, 1.0, 0.0);
			startGame.setText("Start Game");

			scoreScreen.updatePosition(Point(maxCoord.x/2, maxCoord.y/2 + .1, -.01));
			scoreScreen.setSize(0.07, 0.03);
			scoreScreen.setColor(0.0, 1.0, 0.0);
			scoreScreen.setText("Score");

			back.updatePosition(Point(maxCoord.x-.1, maxCoord.y - .1, -0.01));
			back.setSize(.07, .03);
			back.setColor(0.0, 1.0, 0.0);
			back.setText("Beck");
			break;
		case 1:

			break;
		case 2:

			break;
	}
}



void partMissiles(){
	int i, n = enemyMissiles.size(), j, k;
	Point goal, from;
	vector<Missile> lanced;

	for(i = 0; i < n; i++){
		if(!enemyMissiles[i].isDone()){
			lanced = enemyMissiles[i].divide(level);

			if(!lanced.empty()){
				k = enemyMissiles.size() - 1;
				enemyMissiles.insert(enemyMissiles.end(), lanced.begin(), lanced.end());

				vector<City*> notDoneCity;
				vector<Battery*> notDoneBat;

				//Pega apenas as cidades que não foram destruidas
				for(j = 0; j < cities.size(); j++){
					if(!cities[j].isDone()){
						notDoneCity.push_back(NULL);
						notDoneCity[notDoneCity.size()-1] = &cities[j];
					}
				}

				//Pega apenas as baterias que não foram destruidas
				for(j = 0; j < batteries.size(); j++){
					if(!batteries[j].isDone()){
						notDoneBat.push_back(NULL);
						notDoneBat[notDoneBat.size()-1] = &batteries[j];
					}
				}

				if(notDoneCity.size() == 0) return ;
			}
		}
	}
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

//desenha o cursor
void drawSquade()
{		
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor4f(0.2, 0.0, 0.0, 1.0);		
	glBegin(GL_LINES);
		glVertex3f(MOUSEx, (MOUSEy-SIDEY), -0.1);
	    glVertex3f(MOUSEx, (MOUSEy+SIDEY), -0.1);
	    glVertex3f((MOUSEx-SIDEX), MOUSEy, -0.1);
	    glVertex3f((MOUSEx+SIDEX), MOUSEy, -0.1);
	glEnd();
	glEnable(GL_LIGHTING);
}

void drawGrid(){
	float i;

	for(i = 0; i < 1; i += 0.1){
		glBegin(GL_LINE_LOOP);
			glVertex3f(i, 0, 0);
			glVertex3f(i, 1, 0.5);
		glEnd();
	}

	for(i = 0; i < 1; i += 0.1){
		glBegin(GL_LINE_LOOP);
			glVertex3f(0, i, 0);
			glVertex3f(1, i, 0.5);
		glEnd();
	}
}

void display(void)
{
	int i, ncities = cities.size(), nbatteries = batteries.size(), nmissiles = firedMissiles.size();
	int numEnemyMissiles = enemyMissiles.size(), n;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	switch(menu){
		case 0:{
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			glOrtho(minCoord.x, maxCoord.x, maxCoord.y, minCoord.y, minCoord.z, maxCoord.z);
			gluLookAt (0.0, 0.0, distOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity ();
			
			glPushMatrix();
			glLoadIdentity();
			glPushAttrib(GL_ENABLE_BIT);
				menu_back.draw();			
				drawSquade();
				startGame.draw();
				scoreScreen.draw();
			glPopAttrib();
			glPopMatrix();

			break;
		}
		case 1:{
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			
			if(!isOrtho){
				gluPerspective(angleCam, (GLfloat) width/(GLfloat) height, .1, 100.0); 

				glScalef(camScale.x,camScale.y, camScale.z);
				glTranslatef(.06, .06, forwardz);
				glRotatef(-5, 1, 0, 0);

				gluLookAt (eye.x, eye.y, eye.z, center.x, center.y, center.z, 0.0, -1.0, 0.0);
			}else{
				glOrtho(minCoord.x, maxCoord.x, maxCoord.y, minCoord.y, minCoord.z, maxCoord.z);
				
				gluLookAt (0.0, 0.0, distOrigem, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			}

			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity ();			
			
			glPushMatrix();
			glLoadIdentity();				
				//Desenha skybox
				skybox.draw_skybox(center, eye, skypos, skyscale, skyrot);
				
				//Desenha terreno com textura
				textureManager->Bind(2);
				terrain.draw();
				
				//Corrige e desenha a mira
				if(!isOrtho) glRotatef(5, 1, 0, 0);						
				drawSquade();					
				if(!isOrtho) glRotatef(-5, 1, 0, 0);
				
				//Desenhas as baterias com textura
				textureManager->Bind(0);
				for(i = 0; i < nbatteries; i++){
					batteries[i].draw();
				}
				
				//Desenha as cidades com textura
				textureManager->Bind(1);
				for(i = 0; i < ncities; i++){
					if(!cities[i].isDone()){
						cities[i].draw();
					}
				}
				
				//Desenha as explosões com as texturas correspondentes ao seu estado atual
				for(auto itr = explosions.begin(); itr != explosions.end(); itr++){
					float p = (*itr).getPercent() * 100.0;

					for(i = 15; i >= 0; i--){
						if(map[i] < p) break;
					}

					textureManager->Bind(i + 4);
					(*itr).draw();
				}
				
				//Desenha os mísseis e os targets
				for(i = 0; i < nmissiles; i++){
					firedMissiles[i].drawTarget(SIDEX, SIDEY);
					textureManager->Bind(20);
					firedMissiles[i].draw();
				}
				
				
				for(i = 0; i < numEnemyMissiles; i++){
					if(enemyLaunched[i] && !enemyMissiles[i].isDone()){
						textureManager->Bind(20);
						enemyMissiles[i].draw();
					}
				}
			
				textureManager->Disable();			
			
				glRotatef(5, 1, 0, 0);
			glPopMatrix();
			
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			glOrtho(minCoord.x, maxCoord.x, maxCoord.y, minCoord.y, minCoord.z, maxCoord.z);
			gluLookAt (0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity ();
			glPushMatrix();
			glLoadIdentity();
				glDisable(GL_LIGHTING);

				string levelTag = "Level " + to_string(level);
				Drawing::drawText(maxCoord.x/3, .03, 0, 1, 0, 0, 1, string(to_string(score)).c_str());
				Drawing::drawText(2*maxCoord.x/3, .03, 0, 1, 0, 0, 1, string(to_string(score1)).c_str());
				Drawing::drawText(.005, .03, 0, 0, 1, 0, 1, levelTag.c_str());

				if(endGame){
					Drawing::drawText(maxCoord.x/2, maxCoord.y/2, 0, 1, 0, 0, 1, "THE END");
					Drawing::drawText(maxCoord.x/2, maxCoord.y/2 + .1, 0, 0, 1, 0, 1, "Enter your name initials in the terminal.");
				}
				glEnable(GL_LIGHTING);
			glPopMatrix();
			break;
		}
		case 2:{
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();

			glOrtho(minCoord.x, maxCoord.x, maxCoord.y, minCoord.y, minCoord.z, maxCoord.z);
			gluLookAt (0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity ();
			
			glPushMatrix();
			glLoadIdentity();
				score_back.draw();
				drawSquade();
			
				n = (scores.size() > 10)?10:scores.size();
				glDisable(GL_LIGHTING);

				Drawing::drawText(maxCoord.x/2 - .07, .25, 10, 0, 1, 0, 1, "SCORES");

				for(auto itr = scores.begin(); itr != (scores.begin()+10); itr++){
					(*itr).draw();
				}

				back.draw();
				glEnable(GL_LIGHTING);
			glPopMatrix();
			break;
		}
		default:
			break;
	}

	if(endGame && !scoreSaved){
		saveScore();
		scoreSaved = true;
	}

	glutSwapBuffers();
	glutPostRedisplay();
}


void reshape (int w, int h){

}

// Motion callback
void motion(int x, int y )
{
	if (y > 550 && menu == 1){
	 	y=550;
	}
	MOUSEx=convert_range(0, width, minCoord.x, maxCoord.x, x);
	MOUSEy=convert_range(0, height, minCoord.y, maxCoord.y, y);
}

//Retorna o id da bateria mais próxima da posição (x, y)
int findNearestBattery(float x, float y){
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
	MOUSEx=convert_range(0, width, minCoord.x, maxCoord.x, MOUSEx);
	MOUSEy=convert_range(0, height, minCoord.y, maxCoord.y, MOUSEy);

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

				p.x += .025;
				p.y -= .004;
				if(!batteries[id].isEmpty() && !batteries[id].isDone()){
					Missile m = batteries[id].fire(p, Point(MOUSEx, MOUSEy, 0));
					sound->PlaySound(PEI_SOUND);
					
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
			delete textureManager;
			exit(0);
			break;
		case 32:
			paused = !paused;
			break;
		case '+' :
			eye.z -= 0.01;
			break;
		case 'p':
			isOrtho = !isOrtho;
			break;
		case '-' :
			eye.z += 0.01;
			break;
		case 'r':
			restart_game();
			break;
		case 'm':
			menu = 0;
			restart_game();
			break;
		case 'w':
			skyscale.y += .01;
			break;
		case 'a':
			skyscale.x -= .01;
			break;
		case 's':
			skyscale.y -= .01;
			break;
		case 'd':
			skyscale.x += .01;
			break;
		case 'q':
			skyscale.z -= .1f;
			break;
		case 'e':
			skyscale.z += .1f;
			break;
		case '8':
			center.y += .01;
			break;
		case '4':
			center.x -= .01;
			break;
		case '2':
			center.y -= .01;
			break;
		case '6':
			center.x += .01;
			break;
		case '3':
			center.z -= .1;
			break;
		case '9':
			center.z += .1;
			break;
		case '7':
			terrainz += .1;
			break;
		case '1':
			terrainz -= .1;
			/*if(angleCam < 1E-4 && angleCam > -1E-4){
				angleCam = 0;
			}*/
			break;
		case 't':
			skypos.y += .01;
			break;
		case 'f':
			skypos.x -= .01;
			break;
		case 'g':
			skypos.y -= .01;
			break;
		case 'h':
			skypos.x += .01;
			break;
		case 'y':
			skypos.z -= .1f;
			break;
		case 'u':
			skypos.z += .1f;
			break;
		case 'b':
			forwardz -= .01;
			break;
		case 'n':
			forwardz += .01;
			break;
		case '5':
			cout << eye << " " << center << " " << angleCam << endl;
			cout << "Pos: "<< skypos << endl;
			cout <<"scale: " << skyscale << endl;
			cout << "Rot: "<< skyrot << endl;
			break;
		default:
			break;
	}
	/*cout << "Pos: "<< skypos << endl;
	cout <<"scale: " << skyscale << endl;
	cout << "Rot: "<< skyrot << endl;
*/
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
