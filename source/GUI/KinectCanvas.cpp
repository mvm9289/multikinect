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


#include "GUI/KinectCanvas.h"

#include "Geom/Color.h"
#include "Geom/Point.h"
#include "Geom/Vector.h"
#include "Globals/Config.h"
#include "GUI/MainFrameLogic.h"
#include "Render/RenderSystem.h"
#include "Kinect/KinectSkeleton.h"
#include "Tools/Timer.h"
#include <wx/image.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

using namespace MultiKinect;
using namespace Geom;
using namespace Globals;
using namespace GUI;
using namespace Kinect;
using namespace Render;
using namespace Tools;


BEGIN_EVENT_TABLE(KinectCanvas, wxWindow)
EVT_PAINT(KinectCanvas::OnPaint)
END_EVENT_TABLE()


KinectCanvas::KinectCanvas(
	CanvasType type,
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name) : wxWindow(parent, id, pos, size, style, name)
{
	type_ = type;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetDoubleBuffered(true);
}

KinectCanvas::~KinectCanvas()
{
}

wxPoint* KinectCanvas::transformToWXSegment(const std::vector<Point>& segment)
{
	wxPoint* wxSegment = new wxPoint[segment.size()];
	for (uint32 j = 0; j < segment.size(); j++)
	{
		Point depthPoint = KinectSkeleton::transformToDepthPoint(segment[j]);
		wxSegment[j].x = (int32)(depthPoint.x);
		wxSegment[j].y = (int32)(depthPoint.y);
	}

	return wxSegment;
}

void KinectCanvas::render()
{
	Refresh();
	Update();
}

void KinectCanvas::OnPaint(wxPaintEvent& event)
{
	if ((Config::canvas[Globals::INSTANCE_ID].rgbImage			&&	type_ == COLOR_CANVAS) ||
		(Config::canvas[Globals::INSTANCE_ID].depthMap			&&	type_ == DEPTH_CANVAS) ||
		(Config::canvas[Globals::INSTANCE_ID].skeletonTracking	&&	type_ == SKELETON_CANVAS))
	{
		uint8*			bitmap		= 0;
		uint32			nSkeletons	= 0;
		KinectSkeleton*	skeletons	= 0;
		bool			isValidData	= false;

		if (type_ == COLOR_CANVAS)
		{
			if (RenderSystem::isInitialized()) bitmap = RenderSystem::getKinectColorFrame();
			isValidData = bitmap != 0;
		}
		else if (type_ == DEPTH_CANVAS)
		{
			if (RenderSystem::isInitialized()) bitmap = RenderSystem::getKinectDepthFrame();
			isValidData = bitmap != 0;
		}
		else if (type_ == SKELETON_CANVAS)
		{
			if (RenderSystem::isInitialized()) RenderSystem::getKinectSkeletons(nSkeletons, skeletons);
			isValidData = skeletons != 0;
		}

		wxBitmap bmp;
		if (isValidData && (type_ == COLOR_CANVAS || type_ == DEPTH_CANVAS))
		{
			wxImage image(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height, bitmap, true);
			bmp			= wxBitmap(image);
			isValidData	= isValidData && bmp.Ok();
		}

		wxPaintDC dc(this);
		if(!RenderSystem::isInitialized() || !isValidData)
		{
			wxString message = wxT("Unknown view: No image");
			if		(type_ == COLOR_CANVAS)		message = wxT("Color view: No image");
			else if	(type_ == DEPTH_CANVAS)		message = wxT("Depth view: No image");
			else if	(type_ == SKELETON_CANVAS)	message = wxT("Skeleton view: No image");

			dc.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
			dc.Clear();
			dc.DrawLabel(message, wxRect(dc.GetSize()), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
		}
		else if (type_ == COLOR_CANVAS || type_ == DEPTH_CANVAS)
			dc.DrawBitmap(bmp, (dc.GetSize().GetWidth() - bmp.GetWidth())/2, (dc.GetSize().GetHeight() - bmp.GetHeight())/2);
		else
		{
			dc.SetBackground(wxBrush(wxColour(0, 0, 0), wxSOLID));
			dc.Clear();
			wxPen pen;
			pen.SetWidth(4);
			for (uint32 i = 0; i < nSkeletons; i++)
			{
				Color playerColor = skeletons[i].getPlayerColor();
				pen.SetColour(
					basic_cast<uint8>(playerColor.r*255.0f),
					basic_cast<uint8>(playerColor.g*255.0f),
					basic_cast<uint8>(playerColor.b*255.0f));

				// Draw bones
				dc.SetPen(pen);

				std::vector<Point>	segment		=	skeletons[i].getSegment(KinectSkeleton::K_BODY);
				wxPoint*			wxSegment	=	transformToWXSegment(segment);
				dc.DrawLines(basic_cast<uint32>(segment.size()), wxSegment);
				delete[] wxSegment;

				segment		=	skeletons[i].getSegment(KinectSkeleton::K_ARM_LEFT);
				wxSegment	=	transformToWXSegment(segment);
				dc.DrawLines(basic_cast<uint32>(segment.size()), wxSegment);
				delete[] wxSegment;

				segment		=	skeletons[i].getSegment(KinectSkeleton::K_ARM_RIGHT);
				wxSegment	=	transformToWXSegment(segment);
				dc.DrawLines(basic_cast<uint32>(segment.size()), wxSegment);
				delete[] wxSegment;

				segment		=	skeletons[i].getSegment(KinectSkeleton::K_LEG_LEFT);
				wxSegment	=	transformToWXSegment(segment);
				dc.DrawLines(basic_cast<uint32>(segment.size()), wxSegment);
				delete[] wxSegment;

				segment		=	skeletons[i].getSegment(KinectSkeleton::K_LEG_RIGHT);
				wxSegment	=	transformToWXSegment(segment);
				dc.DrawLines(basic_cast<uint32>(segment.size()), wxSegment);
				delete[] wxSegment;

				// Draw joints
				std::vector<Point> jointsPositions	=	skeletons[i].getJointsPositions();
				std::vector<bool> validJoints		=	skeletons[i].getJointsValidity();
				for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
				{
					if (validJoints[j])
					{
						Point dot = KinectSkeleton::transformToDepthPoint(jointsPositions[j]);
						dc.DrawCircle(dot.x, dot.y, 4);
					}
				}
			}
		}
	}
	event.Skip();
}
