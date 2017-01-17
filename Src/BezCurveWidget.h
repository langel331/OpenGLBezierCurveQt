#pragma once
#include <QtOpenGL\QGLWidget>
#include <QtGui\qkeyevent>

class BezCurveWidget : public QGLWidget
{
public:
	//BezCurveWidget(QWidget *parent = 0, const char *name = 0) :	QGLWidget(*parent, name) {};
protected:
	void initializeGL(void);
	void resizeGL(int w, int h);
	void paintGL();
	void keyPressEvent(QKeyEvent *e);
};
