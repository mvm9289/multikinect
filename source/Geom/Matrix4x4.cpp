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


#include "Geom/Matrix4x4.h"

#include "Geom/Vector.h"
#include <cmath>


const uint32 Matrix4x4::N_ = 4;
const uint32 Matrix4x4::X_AXIS = 0;
const uint32 Matrix4x4::Y_AXIS = 1;
const uint32 Matrix4x4::Z_AXIS = 2;

Matrix4x4::Matrix4x4()
{
	data_ = std::vector< std::vector<float32> >(N_, std::vector<float32>(N_));
	setIdentity();
}

Matrix4x4::Matrix4x4(
	float32 m00, float32 m01, float32 m02, float32 m03,
	float32 m10, float32 m11, float32 m12, float32 m13,
	float32 m20, float32 m21, float32 m22, float32 m23,
	float32 m30, float32 m31, float32 m32, float32 m33)
{
	data_ = std::vector< std::vector<float32> >(N_, std::vector<float32>(N_));
	data_[0][0] = m00; data_[0][1] = m01; data_[0][2] = m02; data_[0][3] = m03;
	data_[1][0] = m10; data_[1][1] = m11; data_[1][2] = m12; data_[1][3] = m13;
	data_[2][0] = m20; data_[2][1] = m21; data_[2][2] = m22; data_[2][3] = m23;
	data_[3][0] = m30; data_[3][1] = m31; data_[3][2] = m32; data_[3][3] = m33;
}

Matrix4x4::Matrix4x4(const Matrix4x4& m)
{
	data_ = std::vector< std::vector<float32> >(N_, std::vector<float32>(N_));
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			data_[i][j] = m.data_[i][j];
}

Matrix4x4::~Matrix4x4() {}

Matrix4x4& Matrix4x4::setIdentity()
{
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			data_[i][j] = basic_cast<float32>(i == j);
	return *this;
}

Matrix4x4& Matrix4x4::setZero()
{
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			data_[i][j] = 0.0;
	return *this;
}

Matrix4x4& Matrix4x4::setRotation(float32 angle, uint32 axis)
{
	setIdentity();
	float32 cosA = cos(angle);
	float32 sinA = sin(angle);
	data_[0][0] = data_[1][1] = data_[2][2] = cosA;
	data_[axis][0] = data_[axis][1] = data_[axis][2] = 0.0;
	data_[0][axis] = data_[1][axis] = data_[2][axis] = 0.0;
	data_[axis][axis] = 1.0;
	data_[(axis + 1)%3][(axis + 2)%3] = -sinA;
	data_[(axis + 2)%3][(axis + 1)%3] = sinA;
	return *this;
}

Matrix4x4& Matrix4x4::setRotation(float32 angle, const Vector& axis)
{
	setIdentity();
	Vector u(axis);
	u.normalize();
	float32 c = std::cos(angle);
	float32 s = std::sin(angle);
	float32 ux2 = u.x*u.x;
	float32 uy2 = u.y*u.y;
	float32 uz2 = u.z*u.z;
	float32 t = 1.0f - c;
	data_[0][0] = ux2 + (1.0f - ux2)*c;
	data_[0][1] = u.x*u.y*t - u.z*s;
	data_[0][2] = u.x*u.z*t + u.y*s;
	data_[1][0] = u.x*u.y*t + u.z*s;
	data_[1][1] = uy2 + (1.0f - uy2)*c;
	data_[1][2] = u.y*u.z*t - u.x*s;
	data_[2][0] = u.x*u.z*t - u.y*s;
	data_[2][1] = u.y*u.z*t + u.x*s;
	data_[2][2] = uz2 + (1.0f - uz2)*c;
	return *this;
}

Matrix4x4& Matrix4x4::setTranslation(const Vector& v)
{
	setIdentity();
	data_[0][3] = v.x;
	data_[1][3] = v.y;
	data_[2][3] = v.z;
	return *this;
}

Matrix4x4& Matrix4x4::setScale(float32 scale, int32 axis)
{
	setIdentity();
	if (axis == -1) data_[0][0] = data_[1][1] = data_[2][2] = scale;
	else data_[axis][axis] = scale;
	return *this;
}

void Matrix4x4::swap(Matrix4x4& m)
{
	data_.swap(m.data_);
}

void Matrix4x4::invert()
{
	std::vector< std::vector<float32> > data(N_, std::vector<float32>(2*N_));

	for (uint32 i = 0; i < N_; i++)
	{
		for (uint32 j = 0; j < N_; j++)
		{
			data[j][i] = data_[j][i];
			data[j][i + N_] = 0.0;
		}
		data[i][i + N_] = 1.0;
	}

	for (uint32 i = 0; i < N_; i++)
	{
		for (uint32 j = i + 1; j < N_; j++)
		{
			float32 factor = -data[j][i]/data[i][i];
			for (uint32 k = 0; k < 2*N_; k++)
				data[j][k] = data[j][k] + factor*data[i][k];
		}
	}

	for (int32 i = N_ - 1; i >= 0; i--)
	{
		for (int32 j = i - 1; j >= 0; j--)
		{
			float32 factor = -data[j][i]/data[i][i];
			for (uint32 k = 0; k < 2*N_; k++)
				data[j][k] = data[j][k] + factor*data[i][k];
		}
	}

	for (uint32 i = 0; i < N_; i++)
	{
		float32 factor = 1.0f/data[i][i];
		for (uint32 k = 0; k < 2*N_; k++)
			data[i][k] *= factor;
	}

	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			data_[i][j] = data[i][j + N_];
}

const Matrix4x4 Matrix4x4::inverse() const
{
	Matrix4x4 auxMat(*this);
	auxMat.invert();

	return auxMat;
}

void Matrix4x4::transpose()
{
	Matrix4x4 auxMat;
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			auxMat.data_[i][j] = data_[j][i];
	swap(auxMat);
}

const Matrix4x4 Matrix4x4::transposed() const
{
	Matrix4x4 auxMat(*this);
	auxMat.transpose();

	return auxMat;
}

void Matrix4x4::getEulerAngles(float32& psi, float32& theta, float32& phi) const
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

std::vector<float32>& Matrix4x4::operator[](int32 i)
{
	return data_[i];
}

std::vector<float32> Matrix4x4::operator[](int32 i) const
{
	return data_[i];
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m)
{
	Matrix4x4 auxMat(m);
	swap(auxMat);

	return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
{
	Matrix4x4 auxMat;
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
		{
			auxMat.data_[i][j] = 0.0;
			for (uint32 k = 0; k < N_; k++)
				auxMat.data_[i][j] += data_[i][k]*m.data_[k][j];
		}
	swap(auxMat);

		return *this;
}

Matrix4x4& Matrix4x4::operator*=(float32 d)
{
	for (uint32 i = 0; i < N_; i++)
		for (uint32 j = 0; j < N_; j++)
			data_[i][j] *= d;

	return *this;
}

Matrix4x4& Matrix4x4::operator/=(float32 d)
{
	return *this*=(1.0f/d);
}

const Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const
{
	Matrix4x4 res(*this);

	return res*=m;
}

const Matrix4x4 Matrix4x4::operator*(float32 d) const
{
	Matrix4x4 res(*this);

	return res*=d;
}

const Matrix4x4 Matrix4x4::operator/(float32 d) const
{
	return *this*(1.0f/d);
}

const Point Matrix4x4::operator*(const Point& p) const
{
	Point result;
	result.x = p.x*data_[0][0] + p.y*data_[0][1] + p.z*data_[0][2] + data_[0][3];
	result.y = p.x*data_[1][0] + p.y*data_[1][1] + p.z*data_[1][2] + data_[1][3];
	result.z = p.x*data_[2][0] + p.y*data_[2][1] + p.z*data_[2][2] + data_[2][3];

	return result;
}

const Vector Matrix4x4::operator*(const Vector& v) const
{
	Vector result;
	result.x = v.x*data_[0][0] + v.y*data_[0][1] + v.z*data_[0][2];
	result.y = v.x*data_[1][0] + v.y*data_[1][1] + v.z*data_[1][2];
	result.z = v.x*data_[2][0] + v.y*data_[2][1] + v.z*data_[2][2];

	return result;
}

bool Matrix4x4::operator==(const Matrix4x4& m) const
{
	bool equal = true;
	for (uint32 i = 0; i < N_ && equal; i++)
		for (uint32 j = 0; j < N_ && equal; j++)
			equal = data_[i][j] == m.data_[i][j];

	return equal;
}

bool Matrix4x4::operator!=(const Matrix4x4& m) const
{
	return !(*this==m);
}

const Matrix4x4 operator*(float32 d, const Matrix4x4& m)
{
	return m*d;
}
