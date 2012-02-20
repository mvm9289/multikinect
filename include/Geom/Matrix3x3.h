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


#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__


#include "Globals/Include.h"
#include <vector>


namespace MultiKinect
{
	namespace Geom
	{
		class Matrix3x3
		{
		public:
			enum Axis
			{
				X_AXIS = 0,
				Y_AXIS,
				Z_AXIS
			};

		private:
			std::vector< std::vector<float32> >	data_;

		public:
			Matrix3x3();
			Matrix3x3(
				float32 m00, float32 m01, float32 m02,
				float32 m10, float32 m11, float32 m12,
				float32 m20, float32 m21, float32 m22);
			Matrix3x3(const Matrix3x3& m);
			~Matrix3x3();

			Matrix3x3&		setIdentity();
			Matrix3x3&		setZero();
			Matrix3x3&		setRotation(float32 angle, uint32 axis);
			Matrix3x3&		setRotation(float32 angle, const Vector& axis);
			void			swap(Matrix3x3& m);
			void			invert();
			void			transpose();
			const Matrix3x3	inverse()		const;
			const Matrix3x3	transposed()	const;
			void			getEulerAngles(float32& psi, float32& theta, float32& phi) const; // In radian units

			std::vector<float32>&	operator[](int32 i);
			Matrix3x3&				operator=(const Matrix3x3& m);
			Matrix3x3&				operator*=(const Matrix3x3& m);
			Matrix3x3&				operator*=(float32 d);
			Matrix3x3&				operator/=(float32 d);
			std::vector<float32>	operator[](int32 i)				const;
			const Matrix3x3			operator*(const Matrix3x3& m)	const;
			const Matrix3x3			operator*(float32 d)			const;
			const Matrix3x3			operator/(float32 d)			const;
			const Point				operator*(const Point& p)		const;
			const Vector			operator*(const Vector& v)		const;
			bool					operator==(const Matrix3x3& m)	const;
			bool					operator!=(const Matrix3x3& m)	const;
		};

		const Matrix3x3 operator*(float32 d, const Matrix3x3& m);
	}
}

#endif