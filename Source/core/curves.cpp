//
// Created by Daniel Kowalski on 23/01/2023.
//

#include "curves.hpp"
#include <sstream>
#include <stdexcept>
#include <iostream>

Point::Point(int x ,  int y)
{
	this->x = x;
	this->y = y;
}

Curves::Curves() {
	for (int i = 0; i < 256; i++){
		points[i] = 255 - i;
	}
}

int &Curves::operator[](unsigned int index)
{
	return points[index];
}

int Curves::getPoint(int n1, int n2, float perc)
{
	int diff = n2 - n1;

	return n1 + (diff * perc);
}

void Curves::addCurve(Point start, Point control1, Point control2, Point end)
{
    for (float i = 0; i < 1; i += 0.001)
    {
        // green lines
        int xa = getPoint(start.x, control1.x, i);
        int ya = getPoint(start.y, control1.y, i);
        int xb = getPoint(control1.x, control2.x, i);
        int yb = getPoint(control1.y, control2.y, i);
        int xc = getPoint(control2.x, end.x, i);
        int yc = getPoint(control2.y, end.y, i);

        // blue line
        int xm = getPoint(xa, xb, i);
        int ym = getPoint(ya, yb, i);
        int xn = getPoint(xb, xc, i);
        int yn = getPoint(yb, yc, i);

        // black dot
        int x = getPoint(xm, xn, i);
        int y = getPoint(ym, yn, i);

        points[x] = y;
    }
}

int Curves::length()
{
	return sizeof(points)/sizeof(points[0]);
}
