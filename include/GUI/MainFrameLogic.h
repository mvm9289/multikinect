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


#ifndef __MAINFRAMELOGIC_H__
#define __MAINFRAMELOGIC_H__

#include "Globals/Include.h"
#include "GUI/MainFrame.h"


namespace MultiKinect
{
	namespace GUI
	{
		class MainFrameLogic : public MainFrame
		{
		private:
			uint32 appFrames_;
			float64 appFPS_;

			void setFPS();

		protected:
			void onClose(wxCloseEvent& event);
			void onExit(wxCommandEvent& event);
			void onRestart(wxCommandEvent& event);
			void onIdle(wxIdleEvent& event);
			void onOptionsPanel(wxCommandEvent& event);
			void onViewLog(wxCommandEvent& event);
			void onViewREADME(wxCommandEvent& event);
			void onKinectDeviceChoice(wxCommandEvent& event);
			void onUseColorCheck(wxCommandEvent& event);
			void onUseDepthCheck(wxCommandEvent& event);
			void onUseSkeletonCheck(wxCommandEvent& event);
			void onNearModeCheck(wxCommandEvent& event);
			void onSeatedModeCheck(wxCommandEvent& event);
			void onJointOrientationChoice(wxCommandEvent& event);
			void onResolutionChoice(wxCommandEvent& event);
			void onKinectMatrix(wxCommandEvent& event);
			void onLoad(wxCommandEvent& event);
			void onSave(wxCommandEvent& event);
			void onSaveAsDefault(wxCommandEvent& event);
			void onSetElevationAngle(wxCommandEvent& event);
			void onRecomputeAngleLimits(wxCommandEvent& event);
			void onEditConfigFile(wxCommandEvent& event);

		public:
			MainFrameLogic(wxWindow* parent);
			virtual ~MainFrameLogic();

			virtual void render();
		};
	}
}

#endif
