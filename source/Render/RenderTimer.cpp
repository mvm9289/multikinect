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


#include "Render/RenderTimer.h"

#include "GUI/RenderFrame.h"

using namespace MultiKinect;
using namespace GUI;
using namespace Render;


RenderTimer::RenderTimer()
{
	mainFrame_ = 0;
}


RenderTimer::~RenderTimer()
{
}

void RenderTimer::Notify()
{
	mainFrame_->render();
}

void RenderTimer::setMainFrame(RenderFrame* frame)
{
	mainFrame_ = frame;
}

void RenderTimer::start()
{
	wxTimer::Start(10);
}

void RenderTimer::stop()
{
	wxTimer::Stop();
}
