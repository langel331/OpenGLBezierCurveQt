#pragma once
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>

class Window : public QGLWidget {
public:
	GLuint vao;
	GLuint programID;
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void installShaders();
	void sendDatatoOpenGL();
	~Window();
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
};