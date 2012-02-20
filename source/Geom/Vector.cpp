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


#include "Geom/Vector.h"

#include "Geom/Matrix3x3.h"
#include <cmath>

using namespace MultiKinect;
using namespace Geom;


Vector::Vector() : Point()
{
}

Vector::Vector(float32 px, float32 py, float32 pz) : Point(px, py, pz)
{
}

Vector::Vector(const std::vector<float32>& pcoord) : Point(pcoord)
{
}

Vector::Vector(const Point& start, const Point& end) : Point(end - start)
{
}

Vector::Vector(const Point& p) : Point(p)
{
}

Vector::Vector(const Vector& p) : Point(p)
{
}

Vector& Vector::operator=(const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	
	return *this;
}

const Vector Vector::operator-() const
{
	return Vector(-x, -y, -z);
}

Vector& Vector::operator+=(const Vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vector& Vector::operator-=(const Vector& v)
{
	return (*this += -v);
}

Vector& Vector::operator*=(const float32& c)
{
	x *= c;
	y *= c;
	z *= c;

	return *this;
}

Vector& Vector::operator/=(const float32& c)
{
	return (*this *= 1.0f/c);
}

const Vector Vector::operator+(const Vector& v) const
{
	Vector result(*this);
	result += v;

	return result;
}

const Vector Vector::operator-(const Vector& v) const
{
	Vector result(*this);
	result -= v;

	return result;
}

const Vector Vector::operator*(const float32& c) const
{
	Vector result(*this);
	result *= c;

	return result;
}

const Vector Vector::operator/(const float32& c) const
{
	Vector result(*this);
	result /= c;

	return result;
}

Vector& Vector::operator^=(const Vector& v)
{
	Vector aux(*this);
	aux.x = y*v.z - z*v.y;
	aux.y = z*v.x - x*v.z;
	aux.z = x*v.y - y*v.x;

	*this = aux;

	return *this;
}

const float32 Vector::operator*(const Vector& v) const
{
	return x*v.x + y*v.y + z*v.z;
}

const Vector Vector::operator^(const Vector& v) const
{
	Vector result(*this);
	result ^= v;
	
	return result;
}

bool Vector::operator==(const Vector& v) const
{
	Vector v1(*this);
	Vector v2(v);
	v1.normalize();
	v2.normalize();

	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}

bool Vector::operator!=(const Vector& v) const
{
	return !(*this == v);
}

float32 Vector::length() const
{
	return sqrt(x*x + y*y + z*z);
}

Vector& Vector::normalize()
{
	*this /= length();
	return *this;
}

const Vector Geom::operator*(const float32& c, const Vector& v)
{
	return v*c;
}
