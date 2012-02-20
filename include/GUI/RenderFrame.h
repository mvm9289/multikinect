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


#ifndef __RENDERFRAME_H__
#define __RENDERFRAME_H__

#include "Globals/Include.h"
#include "Globals/Config.h"
#include <wx/frame.h>


namespace MultiKinect
{
	namespace GUI
	{
		class RenderFrame : public wxFrame
		{
		public:
			RenderFrame(
				wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxString& title = wxString(Config::system.appTitle.c_str(), wxConvUTF8),
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);
			virtual ~RenderFrame();

			virtual void render() = 0;
		};
	}
}

#endif
