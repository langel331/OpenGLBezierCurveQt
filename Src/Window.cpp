#include <GL\glew.h>
#include <fstream>
#include "Window.h"
#include "Point.h"
#include <iostream>
#include <glm\glm.hpp>

static const int NUM_OF_SEGMENTS = 60;

GLfloat bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3) {
	// Cubic bezier Curve equation
	GLfloat point = (pow((1 - t), 3.0) * P0) + (3 * pow((1 - t), 2) * t * P1) + (3 * (1 - t) * t * t * P2) + (pow(t, 3) * P3);
	return point;
}

void Window::sendDatatoOpenGL(){	
	//curve segments
	static const int t = NUM_OF_SEGMENTS;
	GLfloat curvePts[t * 3];
	
	// control points
	Point start(0, -6, -4, 0);
	Point tan1(1, 2, 6, 0);
	Point tan2(2, -2, -4, 0);
	Point end(3, 6, 4, 0);

	for (int i = 0; i < t; i++){
		float position = (float)i / (float)t;
		GLfloat x = bezierCurve(position, start._x, tan1._x, tan2._x, end._x);
		GLfloat y = bezierCurve(position, start._y, tan1._y, tan2._y, end._y);
		// In our case, the z should be empty
		GLfloat z = bezierCurve(position, start._z, tan1._z, tan2._z, end._z);
		Point result(4, x, y, z);
		result.PrintPoint();
		curvePts[i * 3] = x;
		curvePts[i * 3 + 1] = y;
		curvePts[i * 3 + 2] = z;
	}
	//Vertex array object
	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//declare vertex buffer Id
	GLuint vertexBufferID;
	//Create vertex buffer
	glGenBuffers(1, &vertexBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(curvePts), curvePts, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//color array per vertex
	GLfloat ptColor[t * 3];
	for (int i = 0; i < t; i++) {
		ptColor[i * 3] = 0.0f;
		ptColor[i * 3 + 1] = 1.0f;
		ptColor[i * 3 + 2] = 0.5f;
		glm::vec3(ptColor[i * 3], ptColor[i * 3 + 1], ptColor[i * 3 + 2]);
	}
	//declare color buffer Id
	GLuint colorBufferID;
	//Create color buffer
	glGenBuffers(1, &colorBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(ptColor), ptColor, GL_STATIC_DRAW);
	//enable color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
}

//checks if shader file loaded
std::string readShaderCode(const char*fileName)
{
	//read shader file
	std::ifstream meInput(fileName);

	//throws error if failed to load
	if (!meInput.good())
	{
		std::cout << "File failed to load " << fileName;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType){
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
		return false;
	}
	return true;
}

//checks if shader compiled
bool Window::checkShaderStatus(GLuint shaderID){
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

//checks if program linked
bool Window::checkProgramStatus(GLuint programID){
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void Window::installShaders(){
	//create vertex shader object
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	//create fragment shader object
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//assign source files to corresponding shaders
	const char* adapter[1];
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	std::string temp1 = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp1.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	//compile shaders
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	//create program to link shaders
	programID = glCreateProgram();
	//attach shaders to program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//link program
	glLinkProgram(programID);
	//check for linking failure
	if (!checkProgramStatus(programID)) {
		std::cout << "Failed to link program\n";
		//return;
	}

	//Delete shaders
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	//install shaders
	glUseProgram(programID);

	//checks if shaders failed to compile
	if (!checkShaderStatus(vertexShaderID)) {
		std::cout << "Vertex shader failed to compile\n";
		//return;
	}
	if (!checkShaderStatus(fragmentShaderID)){
		std::cout << "Fragment shader failed to compile\n";
		//return;
	}
}


void  Window::initializeGL(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glewExperimental = true;
	glewInit();
	//glEnable(GL_DEPTH_TEST);
	sendDatatoOpenGL();
	installShaders();
}

//draws curve
void Window::paintGL() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	glUseProgram(programID);
	glBindVertexArray(vao);
	//glDrawElements(GL_LINE_STRIP, NUM_OF_SEGMENTS* 3, GL_UNSIGNED_SHORT, 0);
	glDrawArrays(GL_LINE_STRIP, 0, NUM_OF_SEGMENTS);
	update();
}

//scales curve to window size
void Window::resizeGL(int w, int h){

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho scales the matrix by multiplying the matrix
	if (w <= h) {
		glOrtho(-5.0, 5.0, -5.0*(GLfloat)h / (GLfloat)w, 5.0*(GLfloat)h / (GLfloat)w, -5.0, 5.0);
	}
	else {
		glOrtho(-5.0*(GLfloat)w / (GLfloat)h, 5.0*(GLfloat)w / (GLfloat)h, -5.0, 5.0, -5.0, 5.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

Window::~Window()
{
	glUseProgram(0);
	glDeleteProgram(programID);
}