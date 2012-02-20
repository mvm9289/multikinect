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


#ifndef __MATRIX4X4_H__
#define __MATRIX4X4_H__

#include "Globals/Include.h"
#include <vector>


namespace MultiKinect
{
	namespace Geom
	{
		class Matrix4x4
		{
		private:
			static const uint32 N_;
			std::vector< std::vector<float32> > data_;

		public:
			static const uint32 X_AXIS;
			static const uint32 Y_AXIS;
			static const uint32 Z_AXIS;

			Matrix4x4();
			Matrix4x4(
				float32 m00, float32 m01, float32 m02, float32 m03,
				float32 m10, float32 m11, float32 m12, float32 m13,
				float32 m20, float32 m21, float32 m22, float32 m23,
				float32 m30, float32 m31, float32 m32, float32 m33);
			Matrix4x4(const Matrix4x4& m);
			~Matrix4x4();

			Matrix4x4& setIdentity();
			Matrix4x4& setZero();
			Matrix4x4& setRotation(float32 angle, uint32 axis);
			Matrix4x4& setRotation(float32 angle, const Vector& axis);
			Matrix4x4& setTranslation(const Vector& v);
			Matrix4x4& setScale(float32 scale, int32 axis = -1);
			void swap(Matrix4x4& m);
			void invert();
			const Matrix4x4 inverse() const;
			void transpose();
			const Matrix4x4 transposed() const;
			void			getEulerAngles(float32& psi, float32& theta, float32& phi) const; // In radian units

			std::vector<float32>& operator[](int32 i);
			std::vector<float32> operator[](int32 i) const;
			Matrix4x4& operator=(const Matrix4x4& m);
			Matrix4x4& operator*=(const Matrix4x4& m);
			Matrix4x4& operator*=(float32 d);
			Matrix4x4& operator/=(float32 d);
			const Matrix4x4 operator*(const Matrix4x4& m) const;
			const Matrix4x4 operator*(float32 d) const;
			const Matrix4x4 operator/(float32 d) const;
			const Point operator*(const Point& p) const;
			const Vector operator*(const Vector& v) const;
			bool operator==(const Matrix4x4& m) const;
			bool operator!=(const Matrix4x4& m) const;
		};

		const Matrix4x4 operator*(float32 d, const Matrix4x4& m);
	}
}

#endif
