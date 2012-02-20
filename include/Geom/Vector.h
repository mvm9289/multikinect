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


#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "Globals/Include.h"
#include "Geom/Point.h"


namespace MultiKinect
{
	namespace Geom
	{
		class Vector : public Point
		{
		public:
			/*
			** Constructors
			*/
			Vector();
			Vector(float32 px, float32 py, float32 pz = 0.0f);
			Vector(const std::vector<float32>& pcoord);
			Vector(const Point& start, const Point& end);

			/*
			** Constructors by copy
			*/
			Vector(const Point& p);
			Vector(const Vector& p);

			/*
			** Operators
			**
			**		* -> Dot product
			**		^ -> Cross product
			*/
			Vector& operator=(const Vector& v);
			const Vector operator-() const;
			Vector& operator+=(const Vector& v);
			Vector& operator-=(const Vector& v);
			Vector& operator*=(const float32& c);
			Vector& operator/=(const float32& c);
			const Vector operator+(const Vector& v) const;
			const Vector operator-(const Vector& v) const;
			const Vector operator*(const float32& c) const;
			const Vector operator/(const float32& c) const;
			Vector& operator^=(const Vector& v);
			const float32 operator*(const Vector& v) const;
			const Vector operator^(const Vector& v) const;
			bool operator==(const Vector& v) const;
			bool operator!=(const Vector& v) const;
			float32 length() const;
			Vector& normalize();
		};

		const Vector operator*(const float32& c, const Vector& v);
	}
}

#endif

