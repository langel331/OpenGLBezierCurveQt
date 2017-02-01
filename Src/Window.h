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
	int lineClickCount;
	glm::vec2 mousePos;
	bool isMousePressed;
	
	GLfloat bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3);
	glm::vec2 mapTo(int x, int y);
	GLuint vaoCurve;
	GLuint vertexBufferID;
	GLuint colorBufferID;

	glm::vec3 ctrlPt[4];
	GLuint vaoPoints;
	GLuint vertexCtrlPtBufferID;
	GLuint colorCtrlPtBufferID;

	glm::vec3 linePt[2];
	GLuint vaoLine;
	GLuint vertexLineBufferID;
	GLuint colorLineBufferID;

	glm::vec3 interPt[9];
	std::vector<glm::vec2> hit;
	std::vector<glm::vec2> intersect;
	GLuint vaoIntersectPt;
	GLuint vertexInterPtBufferID;
	GLuint colorInterPtBufferID;

	GLuint programID;
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void installShaders();
	void sendDatatoOpenGL();
	void  lineIntersection();

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