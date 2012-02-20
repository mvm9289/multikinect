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


#include "Geom/Color.h"

#include <cassert>


Color::Color()
{
	r = 1.0f;
	g = 1.0f;
	b = 1.0f;
	a = 1.0f;
}

Color::Color(float32 cr, float32 cg, float32 cb, float32 ca)
{
	r = cr;
	g = cg;
	b = cb;
	a = ca;
}

Color::Color(const std::vector<float32>& color)
{
	r = color[0];
	g = color[1];
	b = color[2];
	if (color.size() > 3)	a = color[3];
	else					a = 1.0f;
}

Color::Color(const Color& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
}

Color::~Color(void)
{
}

float32 Color::operator[](uint32 ind) const
{
	assert(ind >= 0 && ind <= 3);

	if (ind == 0) return r;
	if (ind == 1) return g;
	if (ind == 2) return b;
	if (ind == 3) return a;

	return 0.0f;
}

std::vector<float32> Color::getColor()
{
	std::vector<float32> color(4);
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	return color;
}

float32& Color::operator[](uint32 ind)
{
	assert(ind >= 0 && ind <= 3);

	if (ind == 0) return r;
	if (ind == 1) return g;
	if (ind == 2) return b;
	if (ind == 3) return a;

	return r;
}

void Color::setColor(const std::vector<float32>& color)
{
	r = color[0];
	g = color[1];
	b = color[2];
	if (color.size() > 3)	a = color[3];
	else					a = 1.0f;
}

Color& Color::operator=(const Color& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	return *this;
}

Color& Color::operator+=(const Color& color)
{
	r += color.r;
	g += color.g;
	b += color.b;
	a += color.a;

	clamp();

	return *this;
}

Color& Color::operator-=(const Color& color)
{
	r -= color.r;
	g -= color.g;
	b -= color.b;
	a -= color.a;

	clamp();

	return *this;
}

Color& Color::operator*=(float32 c)
{
	r *= c;
	g *= c;
	b *= c;
	a *= c;

	clamp();

	return *this;
}

Color& Color::operator/=(float32 c)
{
	*this *= 1.0f/c;

	return *this;
}

const Color Color::operator+(const Color& color) const
{
	Color result(*this);
	result += color;

	return result;
}

const Color Color::operator-(const Color& color) const
{
	Color result(*this);
	result -= color;

	return result;
}

const Color Color::operator*(float32 c) const
{
	Color result(*this);
	result *= c;

	return result;
}

const Color Color::operator/(float32 c) const
{
	Color result(*this);
	result /= c;

	return result;
}

bool Color::operator==(const Color& color) const
{
	return (r == color.r) && (g == color.g) && (b == color.b) && (a == color.a);
}

bool Color::operator!=(const Color& color) const
{
	return !(*this == color);
}

void Color::clamp()
{
	if		(r < 0.0f) r = 0.0f;
	else if	(r > 1.0f) r = 1.0f;

	if		(g < 0.0f) g = 0.0f;
	else if	(g > 1.0f) g = 1.0f;

	if		(b < 0.0f) b = 0.0f;
	else if	(b > 1.0f) b = 1.0f;

	if		(a < 0.0f) a = 0.0f;
	else if	(a > 1.0f) a = 1.0f;
}

const Color Geom::operator*(float32 c, const Color& color)
{
	return color*c;
}
