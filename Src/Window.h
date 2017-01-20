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
	glm::vec2 mousePos;
	bool isMousePressed;
	GLuint vertexBufferID;
	GLuint colorBufferID;
	GLuint vertexCtrlPBufferID;
	GLuint colorCtrlPtBufferID;
	GLuint vaoCurve;
	GLuint vaoPoints;
	GLuint programID;
	glm::vec3 ctrlPt[4];
	glm::vec3 curvePts[];
	GLfloat bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3);
	glm::vec2 mapTo(int x, int y);
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void installShaders();
	void sendDatatoOpenGL();
	Window();
	~Window();
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void paintGL();
};