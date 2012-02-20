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


#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "Globals/Include.h"
#include "Geom/Vector.h"
#include <vector>


namespace MultiKinect
{
	namespace Geom
	{
		class Quaternion
		{
		private:
			float32	real_;
			Vector	imaginary_;

		public:
			Quaternion();
			Quaternion(float32 real, float32 x, float32 y, float32 z);
			Quaternion(float32 real, const Vector& imaginary);
			Quaternion(float32 theta_z, float32 theta_y, float32 theta_x); // In radians units
			Quaternion(const Quaternion& q);
			~Quaternion();

			float32	real() const;
			Vector	imaginary() const;

			void				conjugate();
			const Quaternion	conjugated() const;
			void				invert();
			const Quaternion	inverse() const;
			const Quaternion	log() const;
			const Quaternion	exp() const;

			float32				dot(const Quaternion& q) const;
			float32				length() const;
			float32				length_squared() const;
			void				normalize();
			const Quaternion	normalized() const;

			const Matrix3x3 rotationMat3() const;
			const Matrix4x4 rotationMat4() const;
			const Matrix4x4 isomorphicMat4() const;

			static Quaternion linear_interp(const Quaternion& q1, const Quaternion& q2, float32 t);
			static Quaternion spherical_linear_interp(const Quaternion& q1, const Quaternion& q2, float32 t, bool invert = true);
			static Quaternion spherical_cubic_interp(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float32 t);
			static Quaternion bezier_interp(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float32 t);

			static Quaternion	from_angle_axis(float32 angle, const Vector& axis);
			void				to_angle_axis(float32& angle, Vector& axis) const;

			void euler_angles(float32& theta_z, float32& theta_y, float32& theta_x, bool homogenous = true) const;

			float32&			operator[](int32 i);
			float32				operator[](int32 i) const;
			Quaternion&			operator=(const Quaternion& q);
			Quaternion&			operator+=(const Quaternion& q);
			Quaternion&			operator-=(const Quaternion& q);
			Quaternion&			operator*=(const Quaternion& q);
			Quaternion&			operator/=(const Quaternion& q);
			Quaternion&			operator*=(float32 d);
			Quaternion&			operator/=(float32 d);
			const Quaternion	operator+(const Quaternion& q) const;
			const Quaternion	operator-(const Quaternion& q) const;
			const Quaternion	operator*(const Quaternion& q) const;
			const Quaternion	operator/(const Quaternion& q) const;
			const Quaternion	operator*(float32 d) const;
			const Quaternion	operator/(float32 d) const;
			const Quaternion	operator-() const;
			const Point			operator*(const Point& p) const;
			const Vector		operator*(const Vector& v) const;
			bool				operator==(const Quaternion& q) const;
			bool				operator!=(const Quaternion& q) const;
		};

		const Quaternion operator*(float32 d, const Quaternion& q);
	}
}

#endif
