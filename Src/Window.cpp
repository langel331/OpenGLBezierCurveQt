#include <GL\glew.h>
#include <fstream>
#include "Window.h"
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <QtGui\qkeyevent>
#include <vector>

static const int NUM_OF_SEGMENTS = 100;
glm::vec2 curvePts[NUM_OF_SEGMENTS];
static const int NUM_OF_INTERSECTS = 3;
static const int NUM_OF_CTRL_PTS = 4;
static const float PI = glm::pi<float>();
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

float* Window::findCubeRoots(float A, float B, float C, float D) {
	////https://www.easycalculation.com/algebra/cubic-equation.php	
	//float q = (3.0f*C - B*B) / 9.0f;
	//float r = (-27.0f*D + B*(9.0f*C - 2.0f*B*B))/ 54.0f;
	//float discriminant = pow(q, 3.0f) + r*r;
	//float r13 = 2.0f*sqrt(abs(q));
	//float T = r - sqrt(abs(discriminant));
	//float S = r + sqrt(abs(discriminant));
	//float term1 = sqrt(3.0f)*((-T + S) / 2.0f);
	//float t[NUM_OF_INTERSECTS];
	//
	////1 real root, 2 complex(disregarded)
	//if (discriminant < 0.0f) {
	//	term1 = B / 3.0f;
	//	t[0] = -term1 + r13 *cos(pow(q, 3.0f)/3.0f);
	//	t[1], t[2] = 10000.0f; //assign off screen value;
	//}
	//
	////3 real roots (may have double or triple roots)
	//else {
	//	t[0] = -term1 + r13 *cos(pow(q, 3.0f) / 3.0f);
	//	t[1] = -term1 + r13 *cos(pow(q, 3.0f) + 2.0f * PI/ 3.0f);
	//	t[2] = -term1 + r13 *cos(pow(q, 3.0f) + 4.0f * PI/ 3.0f);
	//}

	//https://www.particleincell.com/2013/cubic-line-intersection/
	float a = B / A;
	float b = C / A;
	float c = D / A;

	float t[3];

	float Q = (3.0f * b - pow(a, 2.0f)) / 9.0f;
	float R = (9.0f * a*b - 27.0f * c - 2.0f * pow(a, 3.0f)) / 54.0f;

	//discriminant
	float discriminant = pow(Q, 3.0f) + pow(R, 2.0f);

	float S;
	float T;
	float Im;
	float th;

	if (discriminant >= 0.0f)										// complex or duplicate roots
	{
		S = glm::sign(R + sqrt(discriminant))*pow(abs(R + sqrt(discriminant)), (1.0f / 3.0f));
		T = glm::sign(R - sqrt(discriminant))*pow(abs(R - sqrt(discriminant)), (1.0f / 3.0f));
		
		t[0] = -a / 3.0f + (S + T);                    
		t[1] = -a / 3.0f - (S + T) / 2.0f;					// real part of complex root
		t[2] = -a / 3.0f - (S + T) / 2.0f;					// real part of complex root
		Im = abs(sqrt(3.0f)*(S - T) / 2.0f);				// complex part of root pair   

													
		if (Im != 0.0f)
		{
			//discard complex roots
			t[1], t[2] = 1000.0f;
		}
	}
	else {                                         //3  distinct real roots
		th = acos(R / sqrt(-pow(Q, 3.0f)));

		t[0] = 2.0f * sqrt(-Q) * cos(th / 3.0f) - a / 3.0f;
		t[1] = 2.0f * sqrt(-Q) * cos((th + 2.0f * PI) / 3.0f) - a / 3.0f;
		t[2] = 2.0f * sqrt(-Q) * cos((th + 4.0f * PI) / 3.0f) - a / 3.0f;
		Im = 0.0f;							  
	}

	return t;
}

//returns intersection between line and curve
void  Window::lineIntersection() {
	//starting point of line
	float x0 = linePt[0].x;
	float y0 = linePt[0].y;

	//ending point of line
	float x1 = linePt[1].x;
	float y1 = linePt[1].y;

	////line vector
	//glm::vec2 line = glm::vec2(x1-x0, y1-y0);

	//coefficients of line equation
	float a = y1 - y0;
	float b = x0 - x1;
	float c = x0*(y0 - y1) + y0*(x1 - x0);

	//coefficients of curve
	glm::vec2 A; // 3rd degree coefficient
	A.x = -ctrlPt[0].x + 3.0f * ctrlPt[1].x - 3.0f * ctrlPt[2].x + ctrlPt[3].x;
	A.y = -ctrlPt[0].y + 3.0f * ctrlPt[1].y - 3.0f * ctrlPt[2].y + ctrlPt[3].y;
	std::cout << "A: " <<A.x << "," << A.y <<"\n";

	glm::vec2 B; // 2nd degree coefficient
	B.x = 3.0f * ctrlPt[0].x - 6.0f * ctrlPt[1].x + 3.0f*ctrlPt[2].x;
	B.y = 3.0f * ctrlPt[0].y - 6.0f * ctrlPt[1].y + 3.0f*ctrlPt[2].y;
	std::cout << "B: " << B.x << "," << B.y << "\n";

	glm::vec2 C; // 1st degree coefficient
	C.x = -3.0f * ctrlPt[0].x + 3.0f*ctrlPt[1].x;
	C.y = -3.0f * ctrlPt[0].y + 3.0f*ctrlPt[1].y;
	std::cout << "C: " << C.x << "," << C.y << "\n";

	glm::vec2 D; //constant
	D.x = ctrlPt[0].x;
	D.y = ctrlPt[0].y;
	std::cout << "D: " << D.x << "," << D.y << "\n";

	//system of equations
	float P[NUM_OF_CTRL_PTS];
	P[0] = a*A.x + b*A.y;
	P[1] = a*B.x + b*B.y;
	P[2] = a*C.x + b*C.y;
	P[3] = a*D.x + b*D.y + c;

	float t;

	for (int i = 0; i < NUM_OF_INTERSECTS; i++) {

		t = findCubeRoots(P[0], P[1], P[2], P[3])[i];

		if (t< 0 || t > 1.0) {
			intersect[i].x, intersect[i].y = 1000.0f;
		}
		else {
			intersect[i].x = A.x*t*t*t + B.x*t*t + C.x*t + D.x;
			intersect[i].y = A.y*t*t*t + B.y*t*t + C.y*t + D.y;
		}								 
		std::cout << "Intersects at:  (" << intersect[i].x << ", " << intersect[i].y << ")\n";

		sendDatatoOpenGL();
	}

	std::cout << "\n";
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
		ctrlPt[0] = glm::vec2(mousePos);
	else if (abs(ctrlPt[1].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[1].y - mousePos.y) < mouseMaxDist)
		ctrlPt[1] = glm::vec2(mousePos);
	else if (abs(ctrlPt[2].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[2].y - mousePos.y) < mouseMaxDist)
		ctrlPt[2] = glm::vec2(mousePos);
	else if (abs(ctrlPt[3].x - mousePos.x) < mouseMaxDist && abs(ctrlPt[3].y - mousePos.y) < mouseMaxDist)
		ctrlPt[3] = glm::vec2(mousePos);

	//update position of line if clicked and dragged
	if (abs(linePt[0].x - mousePos.x) < mouseMaxDist && abs(linePt[0].y - mousePos.y) < mouseMaxDist)
		linePt[0] = glm::vec2(mousePos);
	else if (abs(linePt[1].x - mousePos.x) < mouseMaxDist && abs(linePt[1].y - mousePos.y) < mouseMaxDist)
		linePt[1] = glm::vec2(mousePos);
	
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

		std::cout << "(" << ctrlPt[clickCount].x << ", " << ctrlPt[clickCount].y << ")\n";
		sendDatatoOpenGL();
		clickCount++;
	}

	//create line after curve is made
	if (event->button() == Qt::RightButton && clickCount > 3 && lineClickCount < 2) {
		linePt[lineClickCount].x = mousePos.x;
		linePt[lineClickCount].y = mousePos.y;

		sendDatatoOpenGL();
		lineClickCount++;
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
	glm::vec2 curvePts[t];

	// control points
	glm::vec2 P0 = glm::vec2(ctrlPt[0].x, ctrlPt[0].y);
	glm::vec2 P1 = glm::vec2(ctrlPt[1].x, ctrlPt[1].y);
	glm::vec2 P2 = glm::vec2(ctrlPt[2].x, ctrlPt[2].y);
	glm::vec2 P3 = glm::vec2(ctrlPt[3].x, ctrlPt[3].y);

	//calculate curve points
	for (int i = 0; i < t; i++) {
		float position = (float)i / (float)t;
		x = bezierCurve(position, P0.x, P1.x, P2.x, P3.x);
		y = bezierCurve(position, P0.y, P1.y, P2.y, P3.y);

		curvePts[i].x = x;
		curvePts[i].y = y;
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


	//mapping memory from RAM to GPU buffer
	//control points
	glBindBuffer(GL_ARRAY_BUFFER, vertexCtrlPtBufferID);
	void* ctrlPtsGPUPointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ctrlPtsGPUPointer, ctrlPt, sizeof(ctrlPt));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//curve points
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	void* curvePtGPUPointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(curvePtGPUPointer, curvePts, sizeof(curvePts));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//line points
	glBindBuffer(GL_ARRAY_BUFFER, vertexLineBufferID);
	void* linePtGPUPointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(linePtGPUPointer, linePt, sizeof(linePt));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//intersect points
	glBindBuffer(GL_ARRAY_BUFFER, vertexInterPtBufferID);
	void* intersectPtGPUPointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(intersectPtGPUPointer, &intersect[0].x, NUM_OF_INTERSECTS * sizeof(glm::vec2));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	installShaders();

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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	//control point color array

	glm::vec3 ctrlPtColor[NUM_OF_CTRL_PTS];
	for (int i = 0; i < NUM_OF_CTRL_PTS; i++) {
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
	glEnableVertexAttribArray(1);
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	//color array of curve points
	glm::vec3 ptColor[NUM_OF_SEGMENTS];
	for (int i = 0; i < NUM_OF_SEGMENTS; i++) {
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
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	//line color array

	glm::vec3 lineColor[2];
	for (int i = 0; i < 2; i++) {
		lineColor[i].x = 1.0f;
		lineColor[i].y = 0.0f;
		lineColor[i].z = 0.0f;
	}

	//Create color buffer for control points
	glGenBuffers(1, &colorLineBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorLineBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineColor), lineColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//-----------Intersection Points-------
	intersect.resize(NUM_OF_INTERSECTS);

	//Vertex array object for control points
	glGenVertexArrays(1, &vaoIntersectPt);
	glBindVertexArray(vaoIntersectPt);

	//Create vertex buffer for control points
	glGenBuffers(1, &vertexInterPtBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexInterPtBufferID);
	//Define which buffer to bind to vertex array
	int intersectBufferSize = intersect.size() * sizeof(glm::vec2);
	glBufferData(GL_ARRAY_BUFFER, intersectBufferSize, &intersect[0].x, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	//Describe type  of data to OpenGL
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	//point color array
	glm::vec3 interPtColor[3];
	for (int i = 0; i < 3; i++) {
		interPtColor[i].x = 1.0f;
		interPtColor[i].y = 1.0f;
		interPtColor[i].z = 0.0f;
	}

	//Create color buffer for points
	glGenBuffers(1, &colorInterPtBufferID);
	//Bind color buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, colorInterPtBufferID);
	//Define which buffer to bind to color array
	glBufferData(GL_ARRAY_BUFFER, sizeof(interPtColor), interPtColor, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//unbind all buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	if (lineClickCount == 2) {
		glBindVertexArray(vaoIntersectPt);
		glPointSize(8.0f);
		glDrawArrays(GL_POINTS, 0, intersect.size());
	}

	glBindVertexArray(0);
	
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