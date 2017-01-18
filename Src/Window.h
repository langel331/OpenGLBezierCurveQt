#pragma once
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>
#include "Point.h"

class Window : public QGLWidget {
public:
	GLfloat x, y, z;
	int viewportX;
	int viewportY;
	int pointCount;
	GLuint vertexBufferID;
	GLuint colorBufferID;
	GLuint vao;
	GLuint programID;
	GLfloat ctrlPt[12];
	void mapTo(int x, int y, float output[2]);

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