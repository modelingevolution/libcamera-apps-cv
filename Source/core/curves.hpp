//
// Created by Daniel Kowalski on 23/01/2023.
//

#ifndef LIBCAMERA_APPS_CURVE_HPP
#define LIBCAMERA_APPS_CURVE_HPP

#include <vector>

class Point
{
public:
	int x, y;

	Point(int x, int y);
};


class Curves
{
public:
    Curves();
	void addCurve(Point start, Point control1, Point control2, Point end);
	int& operator[](unsigned int index);
	static const int size = 256;
	int points[size];
	int length();

private:

	int getPoint(int n1, int n2, float perc);
};


#endif //LIBCAMERA_APPS_CURVE_HPP
