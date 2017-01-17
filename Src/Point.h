#pragma once
#include <Gl/gl.h>
#include <stdio.h>

class Point{
public:
	int _id;
	GLfloat _x;
	GLfloat _y;
	GLfloat _z;

	Point(int id, GLfloat x, GLfloat y, GLfloat z) :
		_id(id),
		_x(x),
		_y(y),
		_z(z) {
	}

	void PrintPoint() {
		printf("Point %d = <%f, %f, %f>\n", _id, _x, _y, _z);
	}
};