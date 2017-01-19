#pragma once
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>
#include <glm\glm.hpp>


class Window : public QGLWidget {
public:
	GLfloat x, y, z;
	int viewportX;
	int viewportY;
	int clickCount;
	GLuint vertexBufferID;
	GLuint colorBufferID;
	GLuint vertexCtrlPBufferID;
	GLuint colorCtrlPtBufferID;
	GLuint vaoCurve;
	GLuint vaoPoints;
	GLuint programID;
	glm::vec3 ctrlPt[4];
	glm::vec2 mapTo(int x, int y);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void installShaders();
	void sendDatatoOpenGL();
	void resizeGL(int w, int h);
	Window();
	~Window();
protected:
	virtual void initializeGL();
	void mousePressEvent(QMouseEvent* event);
	virtual void paintGL();
};