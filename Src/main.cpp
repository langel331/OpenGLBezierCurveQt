#include <QtWidgets\qapplication.h>
#include <QtOpenGL\qgl.h>
#include <QtWidgets\qmainwindow.h>
#include <Gl/gl.h>
#include <Gl/glu.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "Window.h"
#include <iostream>

int main(int argc, char** argv) {
	std::cout << "Click in the window 4 times to select your curve control points\n";

	// Specifies OpenGL version used
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	//Qt program
	QApplication app(argc, argv);
	app.setApplicationName("Bezier Curve");
	
	//draw window
	Window window;
	window.showMaximized();

	return app.exec();
}
