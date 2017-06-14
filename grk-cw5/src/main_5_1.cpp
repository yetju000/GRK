#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"



GLuint programColor;
GLuint programTexture;
GLuint programTexture1;
GLuint texture_earth;
GLuint texture_ship;
GLuint texture_sun;
GLuint texture_planet;
GLuint skybox_tex;
GLuint texture;
GLuint programProc;
GLuint texture_potato;
Core::Shader_Loader shaderLoader;

int width = 900;
int height = 900;
int celownik = 0;                      //do celownika
int * windowWidth = (int*)&width;
int * windowHeight = (int*)&height;    

obj::Model shipModel;
obj::Model satModel;
obj::Model sphereModel;
obj::Model sunModel;
obj::Model potatoModel;


float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-8, 0, 0);
glm::vec3 cameraDir;

glm::vec3 SunVEC;
glm::vec3 PlanetVEC;
glm::vec3 EarthVEC;
glm::vec3 AsteroidVEC;
glm::vec3 PotatoVec;



glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));

int acceleration_val = 1;

void drawCrosshairOLD()
{
	glPushMatrix();
	glViewport(0, 0, *windowWidth, *windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, *windowWidth, *windowHeight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3ub(240, 240, 240);//white
	glLineWidth(2.0);
	glBegin(GL_LINES);
	//horizontal line														//CELOWNIK
	glVertex2i(*windowWidth / 2 - 7+celownik, *windowHeight / 2);
	glVertex2i(*windowWidth / 2 + 7+celownik, *windowHeight / 2);
	glEnd();
	//vertical line
	glBegin(GL_LINES);
	glVertex2i(*windowWidth / 2+celownik, *windowHeight / 2 + 7);
	glVertex2i(*windowWidth / 2+celownik, *windowHeight / 2 - 7);
	glEnd();

	glPopMatrix();
}




void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.02f;
	float moveSpeed = 0.06f;

	switch(key)
	{
	case 'q': cameraAngle -= angleSpeed; break;
	case 'e': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed*acceleration_val; 
		if (acceleration_val < 10)
		{
			acceleration_val++;
		}
		 break;
	case 's': cameraPos -= cameraDir * moveSpeed*acceleration_val;
		if (acceleration_val-1 > 0)
		{
			acceleration_val--;
		}	
			break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'z': cameraPos -= glm::cross(cameraDir, glm::vec3(0,0,1)) * moveSpeed; break;
	case 'x': cameraPos += glm::cross(cameraDir, glm::vec3(0, 0, 1)) * moveSpeed; break;

	case 'n': if(celownik>-100) celownik--; break;
	case 'm': if(celownik<100) celownik++; break;

	}

}



glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint text)
{
	GLuint program = programTexture;

	glUseProgram(program);

	Core::SetActiveTexture(text, "sampler", program, 0);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawProcObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint text)
{
	GLuint program = programTexture;

	glUseProgram(program);

	Core::SetActiveTexture(text, "sampler", program, 0);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}


void drawObjectBack(obj::Model * model, glm::mat4 modelMatrix, GLuint text)
{
	GLuint program = programTexture1;

	glUseProgram(program);

	Core::SetActiveTexture(text, "sampler", program, 0);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	float time = glutGet(GLUT_ELAPSED_TIME) / 10000.0f;
	SunVEC = glm::vec3(0, 0, 0);
	PlanetVEC = glm::vec3(-cos(time * 8) * 8, -1, sin(time * 8) * 8);
	EarthVEC = glm::vec3(cos(time * 8) * 5, 1, sin(time * 8) * 5);
	PotatoVec =  glm::vec3(cos(time * 3) * 2, 2, sin(time * 3) * 3);
	AsteroidVEC = glm::vec3(cos(time * 5) * 5, 3, sin(time * 8) * 5);
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.3f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.15f));
	glm::mat4 SunModelMatrix = glm::translate(SunVEC) * glm::scale(glm::vec3(3.5f)) * glm::rotate(time * 1, glm::vec3(0, 1, 0));
	glm::mat4 BackModelMatrix = glm::scale(glm::vec3(30.0f));
	glm::mat4 PlanetModelMatrix = glm::translate(PlanetVEC) * glm::translate(SunVEC) * glm::rotate(time * 20, glm::vec3(0, 2, 0)) * glm::scale(glm::vec3(1.0f));
	glm::mat4 EarthModelMatrix = glm::translate(EarthVEC) * glm::translate(SunVEC) * glm::rotate(time * 30, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(1.0f));
	glm::mat4 SatModelMatrix = glm::translate(AsteroidVEC) * glm::translate(EarthVEC) * glm::rotate(time * 30, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(2.0f));
	glm::mat4 PotatoModelMatrix = glm::translate(PotatoVec) * glm::translate(SunVEC) * glm::rotate(time * 10, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(1.0f));


	drawProcObjectTexture(&shipModel, shipModelMatrix, texture_ship);
	drawObjectTexture(&sphereModel, SunModelMatrix, texture_sun);
	drawObjectTexture(&sphereModel, PlanetModelMatrix, texture_planet);
	drawObjectTexture(&sphereModel, EarthModelMatrix, texture_earth);
	drawObjectTexture(&satModel, SatModelMatrix, texture_ship);
	drawObjectBack(&sphereModel, BackModelMatrix, skybox_tex);
	drawObjectBack(&potatoModel, PotatoModelMatrix, texture_potato);
	/////////////////////////////////////////////////////////////////
	glPushMatrix(); 
	glViewport(0, 0, *windowWidth, *windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, *windowWidth, *windowHeight, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);                              //do celownika
	glLoadIdentity();
	drawCrosshairOLD();
	glPopMatrix();
	///////////////////////////////////////////////////////////////



	glutSwapBuffers();

}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTexture1 = shaderLoader.CreateProgram("shaders/shader_tex1.vert", "shaders/shader_tex1.frag");
	programProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	sunModel = obj::loadModelFromFile("models/sun.obj");
	shipModel = obj::loadModelFromFile("models/ship_mark2.obj");
	satModel = obj::loadModelFromFile("models/mini_statek_GRK.obj");
	potatoModel = obj::loadModelFromFile("models/potato.obj");

	texture= Core::LoadTexture("textures/grid.png");
	texture_earth = Core::LoadTexture("textures/earth2.png");
	texture_ship = Core::LoadTexture("textures/star_destroyer.png");
	texture_sun = Core::LoadTexture("textures/sun.png");
	texture_planet = Core::LoadTexture("textures/planet.png");
	skybox_tex = Core::LoadTexture("textures/Starscape.png");
	texture_potato = Core::LoadTexture("textures/potato.png");

}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programTexture1);
	shaderLoader.DeleteProgram(programProc);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(900, 900);
	glutCreateWindow("OpenGaLaktyka");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
