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
GLint  Attrib_vertex;
GLint Attrib_color;
GLint Unif_matrix;
GLuint VBO_vertex; 
GLuint VBO_color; 
GLuint VBO_element;

GLint Indices_count; 
glm::mat4 Matrix_projection;

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

//! ������� ������ ���� �������
void shaderLog(unsigned int shader)
{
	int   infologLen = 0;
	int   charsWritten = 0;
	char *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		delete[] infoLog;
	}
}

//! �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������
void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

//! ������������� ��������
void initShader()
{
	//! �������� ��� ��������
	const GLchar* vsSource = 
		"attribute vec3 coord;\n"
		"attribute vec3 color;\n"
		"varying vec3 var_color;\n"
		"uniform mat4 matrix;\n"
		"void main() {\n"
		"  gl_Position = matrix * vec4(coord, 1.0);\n"
		"  var_color = color;\n"
		"}\n";
	const char* fsSource =
		"varying vec3 var_color;\n"
		"void main() {\n"
		"  gl_FragColor = vec4(var_color, 1.0);\n"
		"}\n";

	//! ���������� ��� �������� ��������������� ��������
	GLuint vShader, fShader;

	//! ������� ��������� ������
	vShader = glCreateShader(GL_VERTEX_SHADER);
	//! �������� �������� ���
	glShaderSource(vShader, 1, &vsSource, NULL);
	//! ����������� ������
	glCompileShader(vShader);

	std::cout << "vertex shader \n";
	shaderLog(vShader);

	//! ������� ����������� ������
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//! �������� �������� ���
	glShaderSource(fShader, 1, &fsSource, NULL);
	//! ����������� ������
	glCompileShader(fShader);

	std::cout << "fragment shader \n";
	shaderLog(fShader);

	//! ������� ��������� � ����������� ������� � ���
	Program = glCreateProgram();
	glAttachShader(Program, vShader);
	glAttachShader(Program, fShader);

	//! ������� ��������� ���������
	glLinkProgram(Program);

	//! ��������� ������ ������
	int link_ok;
	glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		std::cout << "error attach shaders \n";
		return;
	}
	///! ���������� ID �������� �� ��������� ��������� 
	const char* attr_name = "coord";
	Attrib_vertex = glGetAttribLocation(Program, attr_name);
	if (Attrib_vertex == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
		return;
	}

	const char* unif_name = "matrix";
	Unif_matrix = glGetUniformLocation(Program, unif_name);
	if (Unif_matrix == -1)
	{
		std::cout << "could not bind uniform " << unif_name << std::endl;
		return;
	}

	//! ���������� ID �������
	const char* attr_name2 = "color";
	Attrib_color = glGetAttribLocation(Program, attr_name2);
	if (Attrib_color == -1)
	{
		std::cout << "could not bind uniform " << attr_name2 << std::endl;
		return;
	}

	checkOpenGLerror();
}

//! ������������� VBO
void initVBO()
{
	//! ������� ���� 
	vertex vertices[] = { 
		{-1.0f, -1.0f, -1.0f}, 
		{1.0f, -1.0f, -1.0f}, 
		{1.0f, 1.0f, -1.0f}, 
		{-1.0f, 1.0f, -1.0f}, 
		{-1.0f, -1.0f, 1.0f}, 
		{1.0f, -1.0f, 1.0f}, 
		{1.0f, 1.0f, 1.0f}, 
		{-1.0f, 1.0f, 1.0f} }; 

	//! ����� ����
	vertex colors[] = { 
		{1.0f, 0.5f, 1.0f}, 
		{1.0f, 0.5f, 0.5f}, 
		{0.5f, 0.5f, 1.0f}, 
		{0.0f, 1.0f, 1.0f}, 
		{1.0f, 0.0f, 1.0f}, 
		{1.0f, 1.0f, 0.0f}, 
		{1.0f, 0.0f, 1.0f}, 
		{0.0f, 1.0f, 1.0f} }; 

	//! ������� ������ 
	GLint indices[] = { 
		0, 4, 5, 0, 5, 1, 
		1, 5, 6, 1, 6, 2, 
		2, 6, 7, 2, 7, 3, 
		3, 7, 4, 3, 4, 0,
		4, 7, 6, 4, 6, 5, 
		3, 0, 1, 3, 1, 2 }; 

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

//! ������������ ��������
void freeShader()
{
	//! ��������� ����, �� ��������� �������� ���������
	glUseProgram(0);
	//! ������� ��������� ���������
	glDeleteProgram(Program);
}

//! ������������ ������
void freeVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
	glDeleteBuffers(1, &VBO_element); 
	glDeleteBuffers(1, &VBO_element); 
	glDeleteBuffers(1, &VBO_color);
}

void resizeWindow(int width, int height)
{
	glViewport(0, 0, width, height);

	height = height > 0 ? height : 1; 
	const GLfloat aspectRatio = (GLfloat)width / (GLfloat)height; 
	
	Matrix_projection = glm::perspective(45.0f, aspectRatio, 1.0f, 200.0f); 
	Matrix_projection = glm::translate(Matrix_projection, glm::vec3(0.0f, 0.0f, -5.5f)); 
	Matrix_projection = glm::rotate(Matrix_projection, 45.0f, glm::vec3(.8f, -1.0f, .0f));
	Matrix_projection = glm::scale(Matrix_projection, glm::vec3(1.7f, 0.7f, 1.0f));
}

//! ���������
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//! ������������� ��������� ��������� �������
	glUseProgram(Program);

	glUniformMatrix4fv(Unif_matrix, 1, GL_FALSE, &Matrix_projection[0][0]);

	//! ��������� ����� � ��������� ������ ����� ��� ������ � �� ������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_element);

	//! �������
	//! �������� ������ ��������� ��� ������
	glEnableVertexAttribArray(Attrib_vertex);
	//! ���������� VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	//! �������� pointer 0 ��� ������������ ������, �� ��������� ��� ������ � VBO
	glVertexAttribPointer(Attrib_vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//! �����
	//! �������� ������ ��������� ��� ������
	glEnableVertexAttribArray(Attrib_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(Attrib_color, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//! �������� ������ �� ����������(������)
	glDrawElements(GL_TRIANGLES, Indices_count, GL_UNSIGNED_INT, 0);

	//! ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_vertex);

	//! ��������� ������ ���������
	glDisableVertexAttribArray(Attrib_color);

	checkOpenGLerror();

	glutSwapBuffers();
}

void specialKeys(int key, int x, int y) {

	if (key == GLUT_KEY_CTRL_L)
		rotation = !rotation;
	if (rotation)
	switch (key) {
	case GLUT_KEY_UP: 
		Matrix_projection = glm::rotate(Matrix_projection, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f)); 
		break;
	case GLUT_KEY_DOWN: 
		Matrix_projection = glm::rotate(Matrix_projection, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f)); 
		break;
	case GLUT_KEY_RIGHT: 
		Matrix_projection = glm::rotate(Matrix_projection, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f)); 
		break;
	case GLUT_KEY_LEFT: 
		Matrix_projection = glm::rotate(Matrix_projection, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f)); 
		break;
	case GLUT_KEY_PAGE_UP: 
		Matrix_projection = glm::rotate(Matrix_projection, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f)); 
		break;
	case GLUT_KEY_PAGE_DOWN: 
		Matrix_projection = glm::rotate(Matrix_projection, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f)); 
		break;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
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
	initShader();

	glutReshapeFunc(resizeWindow);
	glutDisplayFunc(render);
	glutSpecialFunc(specialKeys);
	glutMainLoop();

	freeVBO();
}