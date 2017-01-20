#include <GL\glew.h>
#include <fstream>
#include "Window.h"
#include "Point.h"
#include <iostream>
#include <glm\glm.hpp>
#include <QtGui\qkeyevent>


static const int NUM_OF_SEGMENTS = 60;
glm::vec3 curvePts[NUM_OF_SEGMENTS];

//constructor
Window::Window(){
	clickCount = 0;
}

//returns result of Cubic bezier Curve equation
GLfloat Window::bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3) {
	GLfloat point = (pow((1 - t), 3.0) * P0) + (3 * pow((1 - t), 2) * t * P1) + (3 * (1 - t) * t * t * P2) + (pow(t, 3) * P3);
	return point;
}

void Window::mousePressEvent(QMouseEvent* event) {
	//mouse position
	mousePos = mapTo(event->pos().x(), event->pos().y());
	isMousePressed = true;
	
	//click 4 times to assign positions for control points
	if (event->button() == Qt::LeftButton && clickCount < 4) {
		
		ctrlPt[clickCount].x = mousePos.x;
		ctrlPt[clickCount].y = mousePos.y;
		ctrlPt[clickCount].z = 0.0f;

		std::cout << "(" << ctrlPt[clickCount].x << ", " << ctrlPt[clickCount].y << ")\n";
		sendDatatoOpenGL();
		clickCount++;
	}
	sendDatatoOpenGL();
}

void Window::mouseMoveEvent(QMouseEvent* event) {
	//mouse position
	mousePos = mapTo(event->pos().x(), event->pos().y());
	isMousePressed = true;

	// update position of existing control point if clicked and dragged
	if (event->button() == Qt::LeftButton && clickCount == 4) {
		setMouseTracking(true);
	}

	float maxDist = 0.1f;
	if (abs(ctrlPt[0].x - mousePos.x) < maxDist && abs(ctrlPt[0].y - mousePos.y) < maxDist)
		ctrlPt[0] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[1].x - mousePos.x) < maxDist && abs(ctrlPt[1].y - mousePos.y) < maxDist)
		ctrlPt[1] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[2].x - mousePos.x) < maxDist && abs(ctrlPt[2].y - mousePos.y) < maxDist)
		ctrlPt[2] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[3].x - mousePos.x) < maxDist && abs(ctrlPt[3].y - mousePos.y) < maxDist)
		ctrlPt[3] = glm::vec3(mousePos, 0.0f);
	else
		std::cout << "Mouse drag wasn't near a control point: (" << mousePos.x << ", " << mousePos.y << ")\n";
	
	sendDatatoOpenGL();
}

void Window::mouseReleaseEvent(QMouseEvent* event) {
	setMouseTracking(false);
	isMousePressed = false;
}

void Window::sendDatatoOpenGL(){	
	static const int t = NUM_OF_SEGMENTS;
	glm::vec3 curvePts[t];

	// control points
	Point start(0, ctrlPt[0].x, ctrlPt[0].y, ctrlPt[0].z);
	Point tan1(1, ctrlPt[1].x, ctrlPt[1].y, ctrlPt[1].z);
	Point tan2(2, ctrlPt[2].x, ctrlPt[2].y, ctrlPt[2].z);
	Point end(3, ctrlPt[3].x, ctrlPt[3].y, ctrlPt[3].z);

	//calculate curve points
	for (int i = 0; i < t; i++) {
		float position = (float)i / (float)t;
		x = bezierCurve(position, start._x, tan1._x, tan2._x, end._x);
		y = bezierCurve(position, start._y, tan1._y, tan2._y, end._y);
		// In our case, the z should be empty
		z = bezierCurve(position, start._z, tan1._z, tan2._z, end._z);
		Point result(4, x, y, z);

		curvePts[i].x = x;
		curvePts[i].y = y;
		curvePts[i].z = z;
	}

	//checks if mouse click hit the curve
	if (clickCount == 4 && isMousePressed == true) {
		float maxDist = 0.01f;
		
		//computes curve intersection
		for (int i = 0; i < NUM_OF_SEGMENTS; i++) {
			if (abs(curvePts[i].x - mousePos.x) < maxDist && abs(curvePts[i].y - mousePos.y) < maxDist)
				std::cout << "Hit curve segment " << i << " at: (" << curvePts[i].x << ", " << curvePts[i].y << ")\n";
		}
	}

	//Vertex array object for curve
	glGenVertexArrays(1, &vaoCurve);
	glBindVertexArray(vaoCurve);

	//Create vertex buffer for curve points
	glGenBuffers(1, &vertexBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(curvePts), curvePts, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//color array of curve points
	glm::vec3 ptColor[t];
	for (int i = 0; i < t; i++) {
		ptColor[i].x = 0.0f;
		ptColor[i].y = 1.0f;
		ptColor[i].z = 0.5f;
	}

	//Create color buffer for curve points
	glGenBuffers(1, &colorBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(ptColor), ptColor, GL_STATIC_DRAW);
	//enable color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//Vertex array object for control points
	glGenVertexArrays(1, &vaoPoints);
	glBindVertexArray(vaoPoints);

	//Create vertex buffer for control points
	glGenBuffers(1, &vertexCtrlPBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexCtrlPBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(ctrlPt), ctrlPt, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	//control point color array
	
	glm::vec3 ctrlPtColor[4];
	for (int i = 0; i < 4; i++){
		ctrlPtColor[i].x = 0.0f;
		ctrlPtColor[i].y = 0.0f;
		ctrlPtColor[i].z = 1.0f;
	}

	//Create color buffer for control points
	glGenBuffers(1, &colorCtrlPtBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorCtrlPtBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(ctrlPtColor), ctrlPtColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	update();
}

//checks if shader file loaded
std::string readShaderCode(const char*fileName){
	//read shader file
	std::ifstream meInput(fileName);

	//throws error if failed to load
	if (!meInput.good()){
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
	glBindVertexArray(vaoPoints);
	glPointSize(4.0f);
	glDrawArrays(GL_POINTS, 0, 4);

	if (clickCount == 4) {
		glBindVertexArray(vaoCurve);
		glDrawArrays(GL_LINE_STRIP, 0, NUM_OF_SEGMENTS);
	}
	update();
}

//scales curve to window size
void Window::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	viewportX = w;
	viewportY = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho scales the matrix by multiplying the matrix
	if (w <= h) {
		glOrtho(-5.0, 5.0, -5.0*(GLfloat)h / (GLfloat)w,
			5.0*(GLfloat)h / (GLfloat)w, -5.0, 5.0);
	}
	else {
		glOrtho(-5.0*(GLfloat)w / (GLfloat)h,
			5.0*(GLfloat)w / (GLfloat)h, -5.0, 5.0, -5.0, 5.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//converts pixels to world coordinate
glm::vec2 Window::mapTo(int x, int y){
	const int invertY = viewportY - y;
	float xMap = (float)x/ (float)viewportX;
	float yMap = (float)invertY/ (float)viewportY;

	return glm::vec2(xMap * 2.0f - 1.0f, yMap * 2.0f - 1.0f);
}

//destructor
Window::~Window()
{
	//delete buffers
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &colorBufferID);
	glDeleteBuffers(1, &vertexCtrlPBufferID);
	glDeleteBuffers(1, &colorCtrlPtBufferID);
	//stop using the program we created
	glUseProgram(0);
	//delete the program we created
	glDeleteProgram(programID);
}