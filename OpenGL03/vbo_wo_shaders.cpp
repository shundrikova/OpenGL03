#include <windows.h>
#include <GL\glew.h>
#include <gl\freeglut.h>
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#include <iostream>

GLuint Program;
GLint Attrib_vertex;
GLint Attrib_color;
GLint Unif_matrix; 
GLint Unif_color;
GLuint VBO_vertex;
GLuint VBO_color;
GLuint VBO_element;
GLuint VAO;

GLint Indices_count;
glm::mat4 Matrix_projection;

double rotate_x = 0;
double rotate_y = 0;
double rotate_z = 0;

bool rotation = false;

//! �������
struct vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

void initGL() {
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
}

//! �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������
void checkOpenGLerror()
{
	setlocale(LC_ALL, "Russian");
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//! ������������� VBO
void initVBO()
{
	//! ������� ���� 
	vertex vertices[] = {
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f } };

	//! ����� ����
	vertex colors[] = {
		{ 1.0f, 0.5f, 1.0f },
		{ 1.0f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f } };

	//! ������� ������ 
	GLint indices[] = {
		0, 4, 5, 0, 5, 1,
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 7, 6, 4, 6, 5,
		3, 0, 1, 3, 1, 2 };
/*
	vertex vertices[] = {
			{ -1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f,  0.0f },
			{ 0.5f, 0.0f, 1.0f },
			{ 0.5f, 1.0f, 0.5f },
			 };
	
	vertex colors[] = {
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			 };

	GLint indices[] = {
			0,1,2,
			0,3,2,
			0,1,3,
			1,2,3 };*/

	// ������� ����� ��� ������ 
	glGenBuffers(1, &VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// ������� ����� ��� ������ ������ 
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	// ������� ����� ��� �������� ������ 
	glGenBuffers(1, &VBO_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	Indices_count = sizeof(indices) / sizeof(indices[0]);
	checkOpenGLerror();
}


//! ������������ ������
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO_element);
	glDeleteBuffers(1, &VBO_vertex);
	glDeleteBuffers(1, &VBO_color);
}

void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Enable_Light0(void) {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);

	GLfloat position[] = { -2.0, 0.0, 2.0, 1.0 };
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
}

//! ���������
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();

	glRotatef(rotate_x, 1.0, 0.0, 0.0);
	glRotatef(rotate_y, 0.0, 1.0, 0.0);
	glRotatef(rotate_z, 0.0, 0.0, 1.0);
	glScalef(0.5, 0.5, 0.5);

	Enable_Light0();

	//! ��������� ����� � ��������� ������ ����� ��� ������ � �� ������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);

	//! �������
	//! �������� ������ ��������� ��� ������
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//! �����
	//! �������� ������ ��������� ��� ������
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//! �������� ������ �� ����������(������)
	glDrawElements(GL_TRIANGLES, Indices_count, GL_UNSIGNED_INT, 0);

	glPopMatrix();

	//! ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_vertex);

	//! ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_color);

	checkOpenGLerror();

	glutSwapBuffers();
}

void specialKeys(int key, int x, int y) {

	/*if (key == GLUT_KEY_CTRL_L)
		rotation = !rotation;
	if (rotation)*/
		switch (key) {
		case GLUT_KEY_UP: rotate_x += 5; break;
		case GLUT_KEY_DOWN: rotate_x -= 5; break;
		case GLUT_KEY_RIGHT: rotate_y += 5; break;
		case GLUT_KEY_LEFT: rotate_y -= 5; break;
		case GLUT_KEY_PAGE_UP: rotate_z += 5; break;
		case GLUT_KEY_PAGE_DOWN: rotate_z -= 5; break;
		}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple shaders");

	//! ����������� ����� ������������� ��������
	GLenum glew_status = glewInit();
	if (GLEW_OK != glew_status)
	{
		//! GLEW �� ���������������������
		std::cout << "Error: " << glewGetErrorString(glew_status) << "\n";
		return 1;
	}

	//! ��������� ����������� OpenGL 2.0
	if (!GLEW_VERSION_2_0)
	{
		//! OpenGl 2.0 ��������� �� ��������
		std::cout << "No support for OpenGL 2.0 found\n";
		return 1;
	}

	//! �������������
	initGL();
	initVBO();
	//initShader();

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render);
	glutSpecialFunc(specialKeys);
	glutMainLoop();

	freeVBO();
}