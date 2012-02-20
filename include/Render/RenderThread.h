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


#ifndef __RENDERTHREAD_H__
#define __RENDERTHREAD_H__

#include "Globals/Include.h"
#include <wx/thread.h>
#include <wx/msw/winundef.h>


namespace MultiKinect
{
	namespace Render
	{
		class RenderThread : public wxThread
		{
		private:
			RenderFrame* mainFrame_;
			bool running_;
			bool stopped_;

			ExitCode Entry();

		public:
			RenderThread();
			virtual ~RenderThread();

			void setMainFrame(RenderFrame* frame);
			void start();
			void stop();
		};
	}
}

#endif
