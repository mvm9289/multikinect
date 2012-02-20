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


#include "Render/RenderThread.h"

#include "GUI/RenderFrame.h"

using namespace MultiKinect;
using namespace GUI;
using namespace Render;


RenderThread::RenderThread() : wxThread(wxTHREAD_JOINABLE)
{
	mainFrame_ = 0;
	running_ = false;
	stopped_ = false;
	wxThread::Create();
}

RenderThread::~RenderThread()
{
}

wxThread::ExitCode RenderThread::Entry()
{
	while (!stopped_) mainFrame_->render();

	return 0;
}

void RenderThread::setMainFrame(RenderFrame* frame)
{
	mainFrame_ = frame;
}

void RenderThread::start()
{
	stopped_ = false;
	running_ = true;
	Run();
}

void RenderThread::stop()
{
	stopped_ = true;
	if (running_) Wait();
	running_ = false;
}
