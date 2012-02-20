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


#include "Geom/Quaternion.h"

#include "Geom/Matrix3x3.h"
#include "Geom/Matrix4x4.h"
#include <cmath>
#include <cassert>

using namespace MultiKinect;
using namespace Geom;


Quaternion::Quaternion()
{
	real_ = 0.0f;
	imaginary_ = Vector();
}

Quaternion::Quaternion(float32 real, float32 x, float32 y, float32 z)
{
	real_ = real;
	imaginary_ = Vector(x, y, z);
}

Quaternion::Quaternion(float32 real, const Vector& imaginary)
{
	real_ = real;
	imaginary_ = imaginary;
}

Quaternion::Quaternion(float32 theta_z, float32 theta_y, float32 theta_x)
{
	float32 cos_z_2 = cosf(0.5f*theta_z);
	float32 cos_y_2 = cosf(0.5f*theta_y);
	float32 cos_x_2 = cosf(0.5f*theta_x);

	float32 sin_z_2 = sinf(0.5f*theta_z);
	float32 sin_y_2 = sinf(0.5f*theta_y);
	float32 sin_x_2 = sinf(0.5f*theta_x);

	real_ = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
	imaginary_.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
	imaginary_.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
	imaginary_.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
}

Quaternion::Quaternion(const Quaternion& q)
{
	real_ = q.real_;
	imaginary_ = q.imaginary_;
}

Quaternion::~Quaternion()
{
}

float32 Quaternion::real() const
{
	return real_;
}

Vector Quaternion::imaginary() const
{
	return imaginary_;
}

void Quaternion::conjugate()
{
	imaginary_ = -imaginary_;
}

const Quaternion Quaternion::conjugated() const
{
	Quaternion aux(*this);
	aux.conjugate();
	return aux;
}

void Quaternion::invert()
{
	conjugate();
	*this /= length_squared();
}

const Quaternion Quaternion::inverse() const
{
	Quaternion aux(*this);
	aux.invert();
	return aux;
}

const Quaternion Quaternion::log() const
{
	float32 a = acos(real_);
	float32 sina = sin(a);

	Quaternion result;
	if (sina > 0.0f)
	{
		result.imaginary_.x = a*imaginary_.x/sina;
		result.imaginary_.y = a*imaginary_.y/sina;
		result.imaginary_.z = a*imaginary_.z/sina;
	}

	return result;
}

const Quaternion Quaternion::exp() const
{
	float32 a = imaginary_.length();
	float32 sina = sin(a);
	float32 cosa = cos(a);

	Quaternion result;
	result.real_ = cosa;
	if (a > 0.0f)
	{
		result.imaginary_.x = sina*imaginary_.x/a;
		result.imaginary_.y = sina*imaginary_.y/a;
		result.imaginary_.z = sina*imaginary_.z/a;
	}

	return result;
}

float32 Quaternion::dot(const Quaternion& q) const
{
	return imaginary_*q.imaginary_ + real_*q.real_;
}

float32 Quaternion::length() const
{
	return std::sqrt(length_squared());
}

float32 Quaternion::length_squared() const
{
	return real_*real_ + imaginary_*imaginary_; 
}

void Quaternion::normalize()
{
	*this /= length();
}

const Quaternion Quaternion::normalized() const
{
	Quaternion aux(*this);
	aux.normalize();
	return aux;
}

const Matrix3x3 Quaternion::rotationMat3() const
{
	assert(length() > 0.9999f && length() < 1.0001f);
	return Matrix3x3(
			1.0f - 2.0f*(imaginary_.y*imaginary_.y + imaginary_.z*imaginary_.z),
			2.0f*(imaginary_.x*imaginary_.y - real_*imaginary_.z),
			2.0f*(imaginary_.x*imaginary_.z + real_*imaginary_.y),
			2.0f*(imaginary_.x*imaginary_.y + real_*imaginary_.z),
			1.0f - 2.0f*(imaginary_.x*imaginary_.x + imaginary_.z*imaginary_.z),
			2.0f*(imaginary_.y*imaginary_.z - real_*imaginary_.x),
			2.0f*(imaginary_.x*imaginary_.z - real_*imaginary_.y),
			2.0f*(imaginary_.y*imaginary_.z + real_*imaginary_.x),
			1.0f - 2.0f*(imaginary_.x*imaginary_.x + imaginary_.y*imaginary_.y)).transposed();
}

const Matrix4x4 Quaternion::rotationMat4() const
{
	assert(length() > 0.9999f && length() < 1.0001f);
	return Matrix4x4(
		1.0f - 2.0f*(imaginary_.y*imaginary_.y + imaginary_.z*imaginary_.z),
		2.0f*(imaginary_.x*imaginary_.y - real_*imaginary_.z),
		2.0f*(imaginary_.x*imaginary_.z + real_*imaginary_.y),
		0.0f,
		2.0f*(imaginary_.x*imaginary_.y + real_*imaginary_.z),
		1.0f - 2.0f*(imaginary_.x*imaginary_.x + imaginary_.z*imaginary_.z),
		2.0f*(imaginary_.y*imaginary_.z - real_*imaginary_.x),
		0.0f,
		2.0f*(imaginary_.x*imaginary_.z - real_*imaginary_.y),
		2.0f*(imaginary_.y*imaginary_.z + real_*imaginary_.x),
		1.0f - 2.0f*(imaginary_.x*imaginary_.x + imaginary_.y*imaginary_.y),
		0.0f,
		0.0f, 0.0f, 0.0f, 1.0f).transposed();
}

const Matrix4x4 Quaternion::isomorphicMat4() const
{
	return Matrix4x4(
		real_,  -imaginary_.x, -imaginary_.y, -imaginary_.z,
		imaginary_.x, real_, -imaginary_.z, imaginary_.y,
		imaginary_.y, imaginary_.z, real_, -imaginary_.x,
		imaginary_.z, -imaginary_.y, imaginary_.x, real_);
}

Quaternion Quaternion::linear_interp(const Quaternion& q1, const Quaternion& q2, float32 t)
{
	return (q1*(1.0f - t) + q2*t).normalized();
}

Quaternion Quaternion::spherical_linear_interp(const Quaternion& q1, const Quaternion& q2, float32 t, bool invert)
{
	Quaternion q3;
	float32 dot = q1.dot(q2);
	if (dot < 0.0f && invert)
	{
		dot = -dot;
		q3 = -q2;
	}
	else q3 = q2;

	if (dot > -0.95f && dot < 0.95f)
	{
		float32 angle = acosf(dot);
		return (q1*sinf(angle*(1.0f - t)) + q3*sinf(angle*t))/sinf(angle);
	}
	else return linear_interp(q1, q3, t);
}

Quaternion Quaternion::spherical_cubic_interp(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float32 t)
{
	Quaternion c = spherical_linear_interp(q1, q2, t, false);
	Quaternion d = spherical_linear_interp(a, b, t, false);

	return spherical_linear_interp(c, d, 2.0f*t*(1.0f - t), false);
}

Quaternion Quaternion::bezier_interp(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float32 t)
{
	Quaternion q11 = spherical_linear_interp(q1, a, t, false);
	Quaternion q12 = spherical_linear_interp(a, b, t, false);
	Quaternion q13 = spherical_linear_interp(b, q2, t, false);

	return spherical_linear_interp(spherical_linear_interp(q11, q12, t, false), spherical_linear_interp(q12, q13, t, false), t, false);
}

Quaternion Quaternion::from_angle_axis(float32 angle, const Vector& axis)
{
	return Quaternion(cosf(angle*0.5f), axis*sinf(angle*0.5f));
}

void Quaternion::to_angle_axis(float32& angle, Vector& axis) const
{
	angle = acosf(real_);

	float32 sinf_theta_inv = 1.0f/sinf(angle);

	axis.x = imaginary_.x*sinf_theta_inv;
	axis.y = imaginary_.y*sinf_theta_inv;
	axis.z = imaginary_.z*sinf_theta_inv;

	angle *= 2.0f;
}

void Quaternion::euler_angles(float32& theta_z, float32& theta_y, float32& theta_x, bool homogenous) const
{
	float32 sqw = real_*real_;    
	float32 sqx = imaginary_.x*imaginary_.x;    
	float32 sqy = imaginary_.y*imaginary_.y;    
	float32 sqz = imaginary_.z*imaginary_.z;    

	if (homogenous)
	{
		theta_x = atan2f(2.0f*(imaginary_.x*imaginary_.y + imaginary_.z*real_), sqx - sqy - sqz + sqw);    		
		theta_y = asinf(-2.0f*(imaginary_.x*imaginary_.z - imaginary_.y*real_));
		theta_z = atan2f(2.0f*(imaginary_.y*imaginary_.z + imaginary_.x*real_), -sqx - sqy + sqz + sqw);    
	}
	else
	{
		theta_x = atan2f(2.0f*(imaginary_.z*imaginary_.y + imaginary_.x*real_), 1.0f - 2.0f*(sqx + sqy));
		theta_y = asinf(-2.0f*(imaginary_.x*imaginary_.z - imaginary_.y*real_));
		theta_z = atan2f(2.0f*(imaginary_.x*imaginary_.y + imaginary_.z*real_), 1.0f - 2.0f*(sqy + sqz));
	}
}

float32& Quaternion::operator[](int32 i)
{
	assert(i >= 0 && i < 4);
	if (i == 0) return real_;
	else return imaginary_[i];
}

float32 Quaternion::operator[](int32 i) const
{
	assert(i >= 0 && i < 4);
	if (i == 0) return real_;
	else return imaginary_[i];
}

Quaternion& Quaternion::operator=(const Quaternion& q)
{
	real_ = q.real_;
	imaginary_ = q.imaginary_;
	return *this;
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	real_ += q.real_;
	imaginary_ += q.imaginary_;
	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	return *this += -q;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	*this = Quaternion(
		real_*q.real_ - imaginary_*q.imaginary_,
		imaginary_.y*q.imaginary_.z - imaginary_.z*q.imaginary_.y + real_*q.imaginary_.x + imaginary_.x*q.real_,
		imaginary_.z*q.imaginary_.x - imaginary_.x*q.imaginary_.z + real_*q.imaginary_.y + imaginary_.y*q.real_,
		imaginary_.x*q.imaginary_.y - imaginary_.y*q.imaginary_.x + real_*q.imaginary_.z + imaginary_.z*q.real_);
	return *this;
}

Quaternion& Quaternion::operator*=(float32 d)
{
	real_ *= d;
	imaginary_ *= d;
	return *this;
}

Quaternion& Quaternion::operator/=(const Quaternion& q)
{
	Quaternion aux(q); 
	aux.invert(); 
	return *this *= aux;
}

Quaternion& Quaternion::operator/=(float32 d)
{
	return *this *= (1.0f/d);
}

const Quaternion Quaternion::operator+(const Quaternion& q) const
{
	Quaternion aux(*this);
	aux += q;
	return aux;
}

const Quaternion Quaternion::operator-(const Quaternion& q) const
{
	Quaternion aux(*this);
	aux -= q;
	return aux;
}

const Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion aux(*this);
	aux *= q;
	return aux;
}

const Quaternion Quaternion::operator*(float32 d) const
{
	Quaternion aux(*this);
	aux *= d;
	return aux;
}

const Point Quaternion::operator*(const Point& p) const
{
	Quaternion aux(0, Vector(p.x, p.y, p.z));
	Vector result = ((*this)*aux*(this->conjugated())).imaginary_;
	return Point(result.x, result.y, result.z);
}

const Vector Quaternion::operator*(const Vector& v) const
{
	Quaternion aux(0, v);
	return ((*this)*aux*(this->conjugated())).imaginary_;
}

const Quaternion Quaternion::operator/(const Quaternion& q) const
{
	Quaternion aux(*this);
	aux /= q;
	return aux;
}

const Quaternion Quaternion::operator/(float32 d) const
{
	Quaternion aux(*this);
	aux /= d;
	return aux;
}

const Quaternion Quaternion::operator-() const
{
	return Quaternion(-real_, -imaginary_);
}

bool Quaternion::operator==(const Quaternion& q) const
{
	return real_ == q.real_ && imaginary_ == q.imaginary_;
}

bool Quaternion::operator!=(const Quaternion& q) const
{
	return !(*this==q);
}

const Quaternion Geom::operator*(float32 d, const Quaternion& q)
{
	return q*d;
}
