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


#ifndef __COLOR_H__
#define __COLOR_H__


#include "Globals/Include.h"
#include <vector>


namespace MultiKinect
{
	namespace Geom
	{
		class Color
		{
		public:
			float32 r, g, b, a;

			/*
			** Constructors
			*/
			Color();
			Color(float32 cr, float32 cg, float32 cb, float32 ca = 1.0f);
			Color(const std::vector<float32>& color);

			/*
			** Constructor by copy
			*/
			Color(const Color& color);

			/*
			** Destructors
			*/
			~Color();

			/*
			** Getters
			*/
			float32					operator[](uint32 ind) const;
			std::vector<float32>	getColor();

			/*
			** Setters
			*/
			float32&	operator[](uint32 ind);
			void		setColor(const std::vector<float32>& color);

			/*
			** Operators
			*/
			Color&		operator=(const Color& color);
			Color&		operator+=(const Color& color);
			Color&		operator-=(const Color& color);
			Color&		operator*=(float32 c);
			Color&		operator/=(float32 c);
			const Color	operator+(const Color& color)	const;
			const Color	operator-(const Color& color)	const;
			const Color	operator*(float32 c)			const;
			const Color	operator/(float32 c)			const;
			bool		operator==(const Color& color)	const;
			bool		operator!=(const Color& color)	const;
			void		clamp();
		};

		const Color operator*(float32 c, const Color& color);
	}
}

#endif


