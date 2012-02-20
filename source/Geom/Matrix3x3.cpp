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


#include "Geom/Matrix3x3.h"

#include "Geom/Vector.h"
#include <cmath>


Matrix3x3::Matrix3x3()
{
	data_ = std::vector< std::vector<float32> >(3, std::vector<float32>(3));
	setIdentity();
}

Matrix3x3::Matrix3x3(
	float32 m00, float32 m01, float32 m02,
	float32 m10, float32 m11, float32 m12,
	float32 m20, float32 m21, float32 m22)
{
	data_		= std::vector< std::vector<float32> >(3, std::vector<float32>(3));
	data_[0][0]	= m00;		data_[0][1] = m01;		data_[0][2] = m02;
	data_[1][0]	= m10;		data_[1][1] = m11;		data_[1][2] = m12;
	data_[2][0]	= m20;		data_[2][1] = m21;		data_[2][2] = m22;
}

Matrix3x3::Matrix3x3(const Matrix3x3& m)
{
	data_ = std::vector< std::vector<float32> >(3, std::vector<float32>(3));
	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			data_[i][j] = m.data_[i][j];
}

Matrix3x3::~Matrix3x3() {}

Matrix3x3& Matrix3x3::setIdentity()
{
	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			data_[i][j] = basic_cast<float32>(i == j);

	return *this;
}

Matrix3x3& Matrix3x3::setZero()
{
	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			data_[i][j] = 0.0f;

	return *this;
}

Matrix3x3& Matrix3x3::setRotation(float32 angle, uint32 axis)
{
	float32 cosA						=	cos(angle);
	float32 sinA						=	sin(angle);
	data_[0][0]							=	data_[1][1]		=	data_[2][2]		=	cosA;
	data_[axis][0]						=	data_[axis][1]	=	data_[axis][2]	=	0.0f;
	data_[0][axis]						=	data_[1][axis]	=	data_[2][axis]	=	0.0f;
	data_[axis][axis]					=	1.0;
	data_[(axis + 1)%3][(axis + 2)%3]	=	-sinA;
	data_[(axis + 2)%3][(axis + 1)%3]	=	sinA;

	return *this;
}

Matrix3x3& Matrix3x3::setRotation(float32 angle, const Vector& axis)
{
	Vector	u	= Vector(axis).normalize();
	float32	c	= std::cos(angle);
	float32	s	= std::sin(angle);
	float32	ux2	= u.x*u.x;
	float32	uy2	= u.y*u.y;
	float32	uz2	= u.z*u.z;
	float32	t	= 1.0f - c;
	data_[0][0]	= ux2 + (1.0f - ux2)*c;
	data_[0][1]	= u.x*u.y*t - u.z*s;
	data_[0][2]	= u.x*u.z*t + u.y*s;
	data_[1][0]	= u.x*u.y*t + u.z*s;
	data_[1][1]	= uy2 + (1.0f - uy2)*c;
	data_[1][2]	= u.y*u.z*t - u.x*s;
	data_[2][0]	= u.x*u.z*t - u.y*s;
	data_[2][1]	= u.y*u.z*t + u.x*s;
	data_[2][2]	= uz2 + (1.0f - uz2)*c;

	return *this;
}

void Matrix3x3::swap(Matrix3x3& m)
{
	data_.swap(m.data_);
}

void Matrix3x3::invert()
{
	std::vector< std::vector<float32> > data(3, std::vector<float32>(6));

	for (uint32 i = 0; i < 3; i++)
	{
		for (uint32 j = 0; j < 3; j++)
		{
			data[j][i]		=	data_[j][i];
			data[j][i + 3]	=	0.0f;
		}
		data[i][i + 3] = 1.0f;
	}

	for (uint32 i = 0; i < 3; i++)
	{
		for (uint32 j = i + 1; j < 3; j++)
		{
			float32 factor = -data[j][i]/data[i][i];
			for (uint32 k = 0; k < 6; k++)
				data[j][k] = data[j][k] + factor*data[i][k];
		}
	}

	for (int32 i = 2; i >= 0; i--)
	{
		for (int32 j = i - 1; j >= 0; j--)
		{
			float32 factor = -data[j][i]/data[i][i];
			for (uint32 k = 0; k < 6; k++)
				data[j][k] = data[j][k] + factor*data[i][k];
		}
	}

	for (uint32 i = 0; i < 3; i++)
	{
		float32 factor = 1.0f/data[i][i];
		for (uint32 k = 0; k < 6; k++)
			data[i][k] *= factor;
	}

	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			data_[i][j] = data[i][j + 3];
}

const Matrix3x3 Matrix3x3::inverse() const
{
	Matrix3x3 auxMat(*this);
	auxMat.invert();

	return auxMat;
}

void Matrix3x3::transpose()
{
	Matrix3x3 auxMat;
	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			auxMat.data_[i][j] = data_[j][i];
	swap(auxMat);
}

const Matrix3x3 Matrix3x3::transposed() const
{
	Matrix3x3 auxMat(*this);
	auxMat.transpose();

	return auxMat;
}

void Matrix3x3::getEulerAngles(float32& psi, float32& theta, float32& phi) const
{
	if (std::abs(data_[2][0]) != 1)
	{
		theta = -asin(data_[2][0]); // We could choose PI32 + asin(data_[2][0]) too
		float32 cosTheta = cos(theta);
		psi = atan2(data_[2][1]/cosTheta, data_[2][2]/cosTheta);
		phi = atan2(data_[1][0]/cosTheta, data_[0][0]/cosTheta);
	}
	else
	{
		phi = 0.0f; // We could choose any value
		if (data_[2][0] == 1)
		{
			theta = -0.5f*PI32;
			psi = -phi + atan2(-data_[0][1], -data_[0][2]);
		}
		else
		{
			theta = 0.5f*PI32;
			psi = phi + atan2(data_[0][1], data_[0][2]);
		}
	}
}

std::vector<float32>& Matrix3x3::operator[](int32 i)
{
	return data_[i];
}

std::vector<float32> Matrix3x3::operator[](int32 i) const
{
	return data_[i];
}

Matrix3x3& Matrix3x3::operator=(const Matrix3x3& m)
{
	Matrix3x3 auxMat(m);
	swap(auxMat);

	return *this;
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& m)
{
	Matrix3x3 auxMat;
	for (uint32 i = 0; i < 3; i++)
	{
		for (uint32 j = 0; j < 3; j++)
		{
			auxMat.data_[i][j] = 0.0;
			for (uint32 k = 0; k < 3; k++)
				auxMat.data_[i][j] += data_[i][k]*m.data_[k][j];
		}
	}
	swap(auxMat);

	return *this;
}

Matrix3x3& Matrix3x3::operator*=(float32 d)
{
	for (uint32 i = 0; i < 3; i++)
		for (uint32 j = 0; j < 3; j++)
			data_[i][j] *= d;

	return *this;
}

Matrix3x3& Matrix3x3::operator/=(float32 d)
{
	return *this*=(1.0f/d);
}

const Matrix3x3 Matrix3x3::operator*(const Matrix3x3& m) const
{
	Matrix3x3 res(*this);

	return res*=m;
}

const Matrix3x3 Matrix3x3::operator*(float32 d) const
{
	Matrix3x3 res(*this);

	return res*=d;
}

const Matrix3x3 Matrix3x3::operator/(float32 d) const
{
	return *this*(1.0f/d);
}

const Point Matrix3x3::operator*(const Point& p) const
{
	Point result;
	result.x = p.x*data_[0][0] + p.y*data_[0][1] + p.z*data_[0][2];
	result.y = p.x*data_[1][0] + p.y*data_[1][1] + p.z*data_[1][2];
	result.z = p.x*data_[2][0] + p.y*data_[2][1] + p.z*data_[2][2];

	return result;
}

const Vector Matrix3x3::operator*(const Vector& v) const
{
	Vector result;
	result.x = v.x*data_[0][0] + v.y*data_[0][1] + v.z*data_[0][2];
	result.y = v.x*data_[1][0] + v.y*data_[1][1] + v.z*data_[1][2];
	result.z = v.x*data_[2][0] + v.y*data_[2][1] + v.z*data_[2][2];

	return result;
}

bool Matrix3x3::operator==(const Matrix3x3& m) const
{
	bool equal = true;
	for (uint32 i = 0; i < 3 && equal; i++)
		for (uint32 j = 0; j < 3 && equal; j++)
			equal = data_[i][j] == m.data_[i][j];

	return equal;
}

bool Matrix3x3::operator!=(const Matrix3x3& m) const
{
	return !(*this==m);
}

const Matrix3x3 operator*(float32 d, const Matrix3x3& m)
{
	return m*d;
}
