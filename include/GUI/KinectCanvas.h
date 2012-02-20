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


#ifndef __KINECTCANVAS_H__
#define __KINECTCANVAS_H__

#include "Globals/Include.h"
#include "Globals/Config.h"
#include <wx/window.h>
#include <vector>


namespace MultiKinect
{
	namespace GUI
	{
		class KinectCanvas : public wxWindow
		{
		public:
			enum CanvasType
			{
				COLOR_CANVAS = 0,
				DEPTH_CANVAS,
				SKELETON_CANVAS
			};

		private:
			CanvasType type_;

		protected:
			DECLARE_EVENT_TABLE()

		public:
			KinectCanvas(
				CanvasType type,
				wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height),
				long style = wxSIMPLE_BORDER,
				const wxString& name = wxT("KinectCanvas"));
			virtual ~KinectCanvas();

			static wxPoint* transformToWXSegment(const std::vector<Point>& segment);

			void render();
			void OnPaint(wxPaintEvent& event);
		};
	}
}

#endif


