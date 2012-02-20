/*
	MultiKinect: Skeleton tracking based on multiple Microsoft Kinect cameras
	Copyright (C) 2012-2013  Miguel Angel Vico Moya

	This file is part of MultiKinect.

	MultiKinect is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Geom/Point.h"

#include <cassert>

using namespace MultiKinect;
using namespace Geom;


Point::Point()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Point::Point(float32 px, float32 py, float32 pz)
{
	x = px;
	y = py;
	z = pz;
}

Point::Point(const std::vector<float32>& pcoord)
{
	x = pcoord[0];
	y = pcoord[1];
	z = pcoord[2];
}

Point::Point(const Point& p)
{
	x = p.x;
	y = p.y;
	z = p.z;
}

Point::~Point(void)
{
}

float32 Point::operator[](uint32 ind) const
{
	assert(ind >= 0 && ind <= 2);

	if (ind == 0) return x;
	if (ind == 1) return y;
	if (ind == 2) return z;

	return 0.0f;
}

float32& Point::operator[](uint32 ind)
{
	assert(ind >= 0 && ind <= 2);

	if (ind == 0) return x;
	if (ind == 1) return y;
	if (ind == 2) return z;

	return x;
}

std::vector<float32> Point::getCoord()
{
	std::vector<float32> coord(3);
	coord[0] = x;
	coord[1] = y;
	coord[2] = z;

	return coord;
}

float32 Point::s()
{
	return x;
}

float32 Point::t()
{
	return y;
}

float32 Point::r()
{
	return z;
}

void Point::setCoord(const std::vector<float32>& pcoord)
{
	x = pcoord[0];
	y = pcoord[1];
	z = pcoord[2];
}

Point& Point::operator=(const Point& p)
{
	x = p.x;
	y = p.y;
	z = p.z;

	return *this;
}

const Point Point::operator-() const
{
	return Point(-x, -y, -z);
}

Point& Point::operator+=(const Point& p)
{
	x += p.x;
	y += p.y;
	z += p.z;

	return *this;
}

Point& Point::operator-=(const Point& p)
{
	return (*this += -p);
}

Point& Point::operator*=(const float32& c)
{
	x *= c;
	y *= c;
	z *= c;

	return *this;
}

Point& Point::operator/=(const float32& c)
{
	return (*this *= 1.0f/c);
}

const Point Point::operator+(const Point& p) const
{
	Point result(*this);
	result += p;

	return result;
}

const Point Point::operator-(const Point& p) const
{
	Point result(*this);
	result -= p;

	return result;
}

const Point Point::operator*(const float32& c) const
{
	Point result(*this);
	result *= c;

	return result;
}

const Point Point::operator/(const float32& c) const
{
	Point result(*this);
	result /= c;

	return result;
}

bool Point::operator==(const Point& p) const
{
	return (x == p.x) && (y == p.y) && (z == p.z);
}

bool Point::operator!=(const Point& p) const
{
	return !(*this == p);
}

float32 Point::distance(const Point& p) const
{
	float32 distX = x - p.x;
	float32 distY = y - p.y;
	float32 distZ = z - p.z;

	return std::sqrt(distX*distX + distY*distY + distZ*distZ);
}

const Point Geom::operator*(const float32& c, const Point& p)
{
	return p*c;
}
