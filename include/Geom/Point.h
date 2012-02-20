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


#ifndef __POINT_H__
#define __POINT_H__

#include "Globals/Include.h"
#include <vector>


namespace MultiKinect
{
	namespace Geom
	{
		class Point
		{
		public:
			float32 x, y, z;

			/*
			** Constructors
			*/
			Point();
			Point(float32 px, float32 py, float32 pz = 0);
			Point(const std::vector<float32>& pcoord);

			/*
			** Constructor by copy
			*/
			Point(const Point& p);

			/*
			** Destructors
			*/
			~Point(void);

			/*
			** Getters
			*/
			float32					operator[](uint32 ind) const;
			std::vector<float32>	getCoord();
			float32					s();
			float32					t();
			float32					r();

			/*
			** Setters
			*/
			float32&	operator[](uint32 ind);
			void		setCoord(const std::vector<float32>& pcoord);

			/*
			** Operators
			*/
			Point&		operator=(const Point& p);
			const Point	operator-() const;
			Point&		operator+=(const Point& p);
			Point&		operator-=(const Point& p);
			Point&		operator*=(const float32& c);
			Point&		operator/=(const float32& c);
			const Point	operator+(const Point& p) const;
			const Point	operator-(const Point& p) const;
			const Point	operator*(const float32& c) const;
			const Point	operator/(const float32& c) const;
			bool		operator==(const Point& p) const;
			bool		operator!=(const Point& p) const;

			/*
			** Functions
			*/
			float32 distance(const Point& p) const;
		};

		const Point operator*(const float32& c, const Point& p);
	}
}

#endif
