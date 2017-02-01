#include <GL\glew.h>
#include <fstream>
#include "Window.h"
#include "Point.h"
#include <iostream>
#include <glm\glm.hpp>
#include <QtGui\qkeyevent>
#include <vector>

static const int NUM_OF_SEGMENTS = 100;
glm::vec3 curvePts[NUM_OF_SEGMENTS];

//constructor
Window::Window(){
	//clickCount = 4 verifies a curve has been created and ensures a line can only be created after a curve
	clickCount = 0;

	//lineClickCount = 2 verifies a line has been created
	lineClickCount = 0;
}

//returns result of Cubic bezier curve equation
GLfloat Window::bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3) {
	GLfloat point = (pow((1 - t), 3.0) * P0) + (3 * pow((1 - t), 2) * t * P1) + (3 * (1 - t) * t * t * P2) + (pow(t, 3) * P3);
	return point;
}

//returns intersection between line and curve
void  Window::lineIntersection() {
	//starting point of line
	float x0 = linePt[0].x;
	float y0 = linePt[0].y;

	//ending point of line
	float x1 = linePt[1].x;
	float y1 = linePt[1].y;

	//line vector
	glm::vec2 line = glm::vec2(x1-x0, y1-y0);

	float A = y1 - y0;
	float B = x0 - x1;
	float C = x0*(y0 - y1) + y0*(x1 - x0);

	int increments = NUM_OF_SEGMENTS;

	for (int i = 0; i < increments; i++) {
		float t = i / (float)increments;

		//components of bezier equation
		float xBez = bezierCurve(t, ctrlPt[0].x, ctrlPt[1].x, ctrlPt[2].x, ctrlPt[3].x);
		float yBez = bezierCurve(t, ctrlPt[0].y, ctrlPt[1].y, ctrlPt[2].y, ctrlPt[3].y);

		//components of parametric line equation
		float xLine = x0 + t*line.x;
		float yLine = y0 + t*line.y;

		//float distance = sqrt(pow(xBez - xLine, 2.0f) + pow(yBez - yLine, 2.0f));
		float distance = abs(A*xBez + B*yBez + C)/pow(A*A + B*B, 0.5f);

		//closest point on curve to line
		float xClosestPt = (B*(B*xBez - A*yBez) - A*C)/(A*A + B*B);
		float yClosestPt = (A*(-B*xBez + A*yBez) - B*C)/(A*A + B*B);

		if (distance < 0.005f) {
		//if (abs(xLine - xBez) < 0.1f && abs(yLine - yBez) < 0.1f) {
			intersect.push_back(glm::vec2(xClosestPt, yClosestPt));
		}
	}


	if (intersect.size() > 0 && lineClickCount == 2) {
		std::cout << "Intersects at: \n";
		for (int i = 0; i < intersect.size(); i++) {
			std::cout <<"(" << intersect[i].x << ", " << intersect[i].y << ")\n";
		}
	}
	else if (intersect.size() == 0)
		std::cout << "There are no intersections\n";

	sendDatatoOpenGL();
}

void Window::mouseMoveEvent(QMouseEvent* event) {
	//mouse position
	mousePos = mapTo(event->pos().x(), event->pos().y());
	isMousePressed = true;

	// update position of existing control point if clicked and dragged
	if ((event->button() == Qt::LeftButton || event->button() == Qt::RightButton) && clickCount > 3) {
		setMouseTracking(true);
	}
	float mouseMaxDist = 0.1f;

	if (abs(ctrlPt[0].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[0].y - mousePos.y) < mouseMaxDist)
		ctrlPt[0] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[1].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[1].y - mousePos.y) < mouseMaxDist)
		ctrlPt[1] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[2].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[2].y - mousePos.y) < mouseMaxDist)
		ctrlPt[2] = glm::vec3(mousePos, 0.0f);
	else if (abs(ctrlPt[3].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[3].y - mousePos.y) < mouseMaxDist)
		ctrlPt[3] = glm::vec3(mousePos, 0.0f);

	//update position of line if clicked and dragged
	if (abs(linePt[0].x - mousePos.x) < mouseMaxDist && abs(linePt[0].y - mousePos.y) < mouseMaxDist)
		linePt[0] = glm::vec3(mousePos, 0.0f);
	else if (abs(linePt[1].x - mousePos.x) < mouseMaxDist && abs(linePt[1].y - mousePos.y) < mouseMaxDist)
		linePt[1] = glm::vec3(mousePos, 0.0f);
	
	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton){
		intersect.clear();
		hit.clear();
	}
	sendDatatoOpenGL();
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

	//create line after curve is made
	if (event->button() == Qt::RightButton && clickCount > 3 && lineClickCount < 2) {
		linePt[lineClickCount].x = mousePos.x;
		linePt[lineClickCount].y = mousePos.y;
		linePt[lineClickCount].z = 0.0f;

		sendDatatoOpenGL();
		lineClickCount++;
	}

	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton){
		intersect.clear();
		hit.clear();
	}

	sendDatatoOpenGL();
}

void Window::mouseReleaseEvent(QMouseEvent* event) {
	setMouseTracking(false);
	isMousePressed = false;
	if (lineClickCount == 2)
		lineIntersection();
	sendDatatoOpenGL();
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

		curvePts[i].x = x;
		curvePts[i].y = y;
		curvePts[i].z = z;
	}

	//checks if mouse click hit the curve
	if (clickCount > 3 && isMousePressed == true) {
		float maxDist = 0.01f;
		
		//computes curve intersection with mouse click
		for (int i = 0; i < NUM_OF_SEGMENTS; i++) {
			if (abs(curvePts[i].x - mousePos.x) < maxDist && abs(curvePts[i].y - mousePos.y) < maxDist)
				std::cout << "Hit curve at: (" << curvePts[i].x << ", " << curvePts[i].y << ")\n";
		}
	}

	//-----------Control Points-------
	//Vertex array object for control points
	glGenVertexArrays(1, &vaoPoints);
	glBindVertexArray(vaoPoints);

	//Create vertex buffer for control points
	glGenBuffers(1, &vertexCtrlPtBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexCtrlPtBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(ctrlPt), ctrlPt, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	//control point color array

	glm::vec3 ctrlPtColor[4];
	for (int i = 0; i < 4; i++) {
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


	//-----------Curve-------
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


	//-----------Line-------
	//Vertex array object for line
	glGenVertexArrays(1, &vaoLine);
	glBindVertexArray(vaoLine);

	//Create vertex buffer for control points
	glGenBuffers(1, &vertexLineBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexLineBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(linePt), linePt, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	//line color array

	glm::vec3 lineColor[2];
	for (int i = 0; i < 2; i++) {
		lineColor[i].x = 1.0f;
		lineColor[i].y = 1.0f;
		lineColor[i].z = 0.0f;
	}

	//Create color buffer for control points
	glGenBuffers(1, &colorLineBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorLineBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineColor), lineColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);


	//-----------Intersection Points-------
	//Vertex array object for control points
	glGenVertexArrays(1, &vaoIntersectPt);
	glBindVertexArray(vaoIntersectPt);

	//Create vertex buffer for control points
	glGenBuffers(1, &vertexInterPtBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexInterPtBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, intersect.size()*sizeof(intersect), &intersect[0], GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	//point color array
	glm::vec3 interPtColor[3];
	for (int i = 0; i < 3; i++) {
		interPtColor[i].x = 0.0f;
		interPtColor[i].y = 1.0f;
		interPtColor[i].z = 0.0f;
	}

	//Create color buffer for points
	glGenBuffers(1, &colorInterPtBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorInterPtBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(interPtColor), interPtColor, GL_STATIC_DRAW);
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
	sendDatatoOpenGL();
	installShaders();
}

//draws
void Window::paintGL() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	glUseProgram(programID);
	
	glBindVertexArray(vaoPoints);
	//draw control points
	glPointSize(4.0f);
	glDrawArrays(GL_POINTS, 0, 4);

	// draw curve after 4 points are defined
	if (clickCount > 3) {
		glBindVertexArray(vaoCurve);
		glDrawArrays(GL_LINE_STRIP, 0, NUM_OF_SEGMENTS);
	}

	glBindVertexArray(vaoLine);
	//draw line after curve is drawn
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	// draw intersects after line
	if (lineClickCount == 2 && intersect.size() != 0) {
		glBindVertexArray(vaoIntersectPt);
		glPointSize(8.0f);
		glDrawArrays(GL_POINTS, 0, intersect.size());
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
	glDeleteBuffers(1, &vertexCtrlPtBufferID);
	glDeleteBuffers(1, &colorCtrlPtBufferID);
	glDeleteBuffers(1, &vertexLineBufferID);
	glDeleteBuffers(1, &colorLineBufferID);
	glDeleteBuffers(1, &vertexInterPtBufferID);
	glDeleteBuffers(1, &colorInterPtBufferID);
	//stop using the program we created
	glUseProgram(0);
	//delete the program we created
	glDeleteProgram(programID);
}