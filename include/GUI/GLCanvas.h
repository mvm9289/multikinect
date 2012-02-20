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


#ifndef __GLCANVAS_H__
#define __GLCANVAS_H__

#include "Globals/Include.h"
#include "Globals/Config.h"
#include <GL/glew.h>
#include <wx/glcanvas.h>


namespace MultiKinect
{
	namespace GUI
	{
		class GLCanvas : public wxGLCanvas
		{
		private:
			bool initialized_;
			float32 longitudeAngle_;
			float32 latitudeAngle_;
			float32 zoomFactor_;
			bool leftClicked_;
			int32 previousX_;
			int32 previousY_;

			void initializeGL();
			void setViewport() const;
			void setProjection() const;
			void setModelview() const;
			void render3DRoom() const;
			void renderKinectCamera(const Matrix4x4& kinectMatrix) const;

			void renderSkeletonJoint(const Point& joint, const Quaternion& jointOrientation, float32 radius, uint32 subdivisions, GLUquadricObj* quadric) const;
			void renderSkeletonBone(const Point& startJoint, const Point& endJoint, float32 radius, uint32 subdivisions, GLUquadricObj* quadric) const;
			void renderKinectSkeleton(const KinectSkeleton& kinectSkeleton) const;

		protected:
			DECLARE_EVENT_TABLE()

		public:
			GLCanvas(
				wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height),
				long style = wxSIMPLE_BORDER,
				const wxString& name = wxT("GLCanvas"),
				int* attribList = 0);
			virtual ~GLCanvas();

			void render();

			void onResize(wxSizeEvent& event);
			void onMouseMove(wxMouseEvent& event);
			void onLeftClick(wxMouseEvent& event);
			void onLeftRelease(wxMouseEvent& event);
			void onMouseWheel(wxMouseEvent& event);
		};
	}
}

#endif
