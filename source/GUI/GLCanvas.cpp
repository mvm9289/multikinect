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


#include "GUI/GLCanvas.h"

#include "Globals/Definitions.h"
#include "Geom/Color.h"
#include "Geom/Matrix4x4.h"
#include "Geom/Point.h"
#include "Geom/Vector.h"
#include "Kinect/KinectManager.h"
#include "Kinect/KinectSkeleton.h"
#include "Render/RenderSystem.h"
#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Geom;
using namespace GUI;
using namespace Kinect;
using namespace Render;
using namespace Tools;


BEGIN_EVENT_TABLE(GLCanvas, wxWindow)
EVT_SIZE(GLCanvas::onResize)
EVT_MOTION(GLCanvas::onMouseMove)
EVT_LEFT_DOWN(GLCanvas::onLeftClick)
EVT_LEFT_UP(GLCanvas::onLeftRelease)
EVT_MOUSEWHEEL(GLCanvas::onMouseWheel)
END_EVENT_TABLE()


GLCanvas::GLCanvas(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name,
	int* attribList) : wxGLCanvas(parent, id, pos, size, style, name, attribList)
{
	initialized_ = false;
	longitudeAngle_ = 1.75f*PI32;
	latitudeAngle_ = 0.25f*PI32;
	zoomFactor_ = 1.0f;
	leftClicked_ = false;
	previousX_ = 0;
	previousY_ = 0;
}

GLCanvas::~GLCanvas()
{
}

void GLCanvas::initializeGL()
{
	GLenum errCode = glewInit();
	if (errCode != GLEW_OK)
		Log::write("[GLCanvas] GLCanvas()", "ERROR: Unable to initialize GLEW.");

	setViewport();
	setProjection();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_MULTISAMPLE_ARB);

	initialized_ = true;
}

void GLCanvas::setViewport() const
{
	glViewport(0, 0, basic_cast<uint32>(GetSize().x), basic_cast<uint32>(GetSize().y));
}

void GLCanvas::setProjection() const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, basic_cast<float32>(GetSize().x)/basic_cast<float32>(GetSize().y), 0.01f, 100.0f);
}

void GLCanvas::setModelview() const
{
	Vector obsDir(1.0f, 0.0f, 0.0f);
	Vector longitudeVec(0.0f, 1.0f, 0.0f);
	Vector latitudeVec(0.0f, 0.0f, 1.0f);
	Matrix4x4 rotLongitude = Matrix4x4().setRotation(longitudeAngle_, longitudeVec);
	latitudeVec = rotLongitude*latitudeVec;
	Matrix4x4 rotLatitude = Matrix4x4().setRotation(latitudeAngle_, latitudeVec);
	obsDir = rotLatitude*rotLongitude*obsDir;

	float32 obsDist = Vector(Config::room.width, 2.0f*Config::room.height, Config::room.depth).length();
	Point vrp(Config::room.origin.x + Config::room.width*0.5f, Config::room.origin.y, Config::room.origin.z + Config::room.depth*0.5f);
	Point obs = vrp + (0.1f + (obsDist - 0.1f)*zoomFactor_)*obsDir;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(obs.x, obs.y, obs.z, vrp.x, vrp.y, vrp.z, 0.0f, 1.0f, 0.0f);
}

void GLCanvas::render3DRoom() const
{
	glDisable(GL_LIGHTING);

	// Axes
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	// X axis
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(Config::room.origin.x, 0.0f, 0.0f);
	glVertex3f(Config::room.origin.x + Config::room.width, 0.0f, 0.0f);
	// Y axis
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, Config::room.origin.y, 0.0f);
	glVertex3f(0.0f, Config::room.origin.y + Config::room.height, 0.0f);
	// Z axis
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, Config::room.origin.z);
	glVertex3f(0.0f, 0.0f, Config::room.origin.z + Config::room.depth);
	glEnd();

	// One meter lines
	glLineWidth(2.0f);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glBegin(GL_LINES);
	for (float32 step = 0.0f; step <= Config::room.width; step += 1.0f)
	{
		float32 xStep = Config::room.origin.x + step;
		if (std::abs(xStep) > 0.0001f)
		{
			glVertex3f(xStep, Config::room.origin.y, Config::room.origin.z);
			glVertex3f(xStep, Config::room.origin.y, Config::room.origin.z + Config::room.depth);
		}
	}
	for (float32 step = 0.0f; step <= Config::room.depth; step += 1.0f)
	{
		float32 zStep = Config::room.origin.z + step;
		if (std::abs(zStep) > 0.0001f)
		{
			glVertex3f(Config::room.origin.x, Config::room.origin.y, zStep);
			glVertex3f(Config::room.origin.x + Config::room.width, Config::room.origin.y, zStep);
		}
	}
	glEnd();

	// Ten centimeters lines
	glLineWidth(0.5f);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glBegin(GL_LINES);
	for (float32 step = 0.0f; step <= Config::room.width; step += 0.1f)
	{
		float32 xStep = Config::room.origin.x + step;
		if (std::abs(xStep) > 0.0001f)
		{
			glVertex3f(xStep, Config::room.origin.y, Config::room.origin.z);
			glVertex3f(xStep, Config::room.origin.y, Config::room.origin.z + Config::room.depth);
		}
	}
	for (float32 step = 0.0f; step <= Config::room.depth; step += 0.1f)
	{
		float32 zStep = Config::room.origin.z + step;
		if (std::abs(zStep) > 0.0001f)
		{
			glVertex3f(Config::room.origin.x, Config::room.origin.y, zStep);
			glVertex3f(Config::room.origin.x + Config::room.width, Config::room.origin.y, zStep);
		}
	}
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_LIGHTING);
}

void GLCanvas::renderKinectCamera(const Matrix4x4& kinectMatrix) const
{
	glDisable(GL_LIGHTING);

	// Axes
	Point orig = kinectMatrix*Point(0.0f, 0.0f, 0.0f);
	Point axisX = kinectMatrix*Point(0.1f, 0.0f, 0.0f);
	Point axisY = kinectMatrix*Point(0.0f, 0.1f, 0.0f);
	Point axisZ = kinectMatrix*Point(0.0f, 0.0f, 0.1f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// X axis
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(orig.x, orig.y, orig.z);
	glVertex3f(axisX.x, axisX.y, axisX.z);
	// Y axis
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(orig.x, orig.y, orig.z);
	glVertex3f(axisY.x, axisY.y, axisY.z);
	// Z axis
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(orig.x, orig.y, orig.z);
	glVertex3f(axisZ.x, axisZ.y, axisZ.z);
	glEnd();

	// Prism
	Point prismOrigin = kinectMatrix*Point(0.0f, 0.0f, 0.0f);
	Point prismSquare1 = kinectMatrix*Point(-0.1f, 0.05f, 0.2f);
	Point prismSquare2 = kinectMatrix*Point(0.1f, 0.05f, 0.2f);
	Point prismSquare3 = kinectMatrix*Point(0.1f, -0.05f, 0.2f);
	Point prismSquare4 = kinectMatrix*Point(-0.1f, -0.05f, 0.2f);
	glLineWidth(2.0f);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(prismOrigin.x, prismOrigin.y, prismOrigin.z);
	glVertex3f(prismSquare1.x, prismSquare1.y, prismSquare1.z);
	glVertex3f(prismOrigin.x, prismOrigin.y, prismOrigin.z);
	glVertex3f(prismSquare2.x, prismSquare2.y, prismSquare2.z);
	glVertex3f(prismOrigin.x, prismOrigin.y, prismOrigin.z);
	glVertex3f(prismSquare3.x, prismSquare3.y, prismSquare3.z);
	glVertex3f(prismOrigin.x, prismOrigin.y, prismOrigin.z);
	glVertex3f(prismSquare4.x, prismSquare4.y, prismSquare4.z);
	glVertex3f(prismSquare1.x, prismSquare1.y, prismSquare1.z);
	glVertex3f(prismSquare2.x, prismSquare2.y, prismSquare2.z);
	glVertex3f(prismSquare2.x, prismSquare2.y, prismSquare2.z);
	glVertex3f(prismSquare3.x, prismSquare3.y, prismSquare3.z);
	glVertex3f(prismSquare3.x, prismSquare3.y, prismSquare3.z);
	glVertex3f(prismSquare4.x, prismSquare4.y, prismSquare4.z);
	glVertex3f(prismSquare4.x, prismSquare4.y, prismSquare4.z);
	glVertex3f(prismSquare1.x, prismSquare1.y, prismSquare1.z);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_LIGHTING);
}

void GLCanvas::renderSkeletonJoint(const Point& joint, const Quaternion& jointOrientation, float32 radius, uint32 subdivisions, GLUquadricObj* quadric) const
{
	glPushMatrix();
	glTranslatef(joint.x, joint.y, joint.z);
	gluQuadricOrientation(quadric, GLU_OUTSIDE);
	gluSphere(quadric, radius, subdivisions, subdivisions);
	if (Config::canvas[Globals::INSTANCE_ID].showOrientations)
	{
		Vector oriY(0.0f, 1.0f, 0.0f);
		Vector oriZ(0.0f, 0.0f, -1.0f);
		oriY = jointOrientation*oriY;
		oriZ = jointOrientation*oriZ;
		glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.2f*oriY.x, 0.2f*oriY.y, 0.2f*oriY.z);
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.2f*oriZ.x, 0.2f*oriZ.y, 0.2f*oriZ.z);
		glEnd();
		glPopAttrib();
	}
	glPopMatrix();
}

void GLCanvas::renderSkeletonBone(const Point& startJoint, const Point& endJoint, float32 radius, uint32 subdivisions, GLUquadricObj* quadric) const
{
	Vector bone = endJoint - startJoint;
	if(bone.x == 0) bone.x = 0.0001f;
	if(bone.y == 0) bone.y = 0.0001f;
	if(bone.z == 0) bone.z = 0.0001f;

	float32 angle = RAD2DEG32(std::acos(bone.z/bone.length()));
	if (bone.z < 0.0f) angle = -angle;
	float32 axisX = -bone.y*bone.z;
	float32 axisY = bone.x*bone.z;

	glPushMatrix();
	//Draw cylinder body
	glTranslatef(startJoint.x, startJoint.y, startJoint.z);
	glRotatef(angle, axisX, axisY, 0.0);
	gluQuadricOrientation(quadric, GLU_OUTSIDE);
	gluCylinder(quadric, radius, radius, bone.length(), subdivisions, 1);
	//Draw first cap
	gluQuadricOrientation(quadric, GLU_INSIDE);
	gluDisk(quadric, 0.0, radius, subdivisions, 1);
	glTranslatef(0, 0, bone.length());
	//Draw second cap
	gluQuadricOrientation(quadric, GLU_OUTSIDE);
	gluDisk(quadric, 0.0, radius, subdivisions, 1);
	glPopMatrix();
}

void GLCanvas::renderKinectSkeleton(const KinectSkeleton& kinectSkeleton) const
{
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);

	Color playerColor = kinectSkeleton.getPlayerColor();
	glColor4f(playerColor.r, playerColor.g, playerColor.b, 1.0f);

	// Draw skeleton bones
	std::vector< std::vector<Point> > segments = kinectSkeleton.getBonesSegments();
	uint32 nSegments = basic_cast<uint32>(segments.size());
	for (uint32 i = 0; i < nSegments; i++)
	{
		uint32 nJoints = basic_cast<uint32>(segments[i].size());
		for (uint32 j = 1; j < nJoints; j++)
			renderSkeletonBone(segments[i][j - 1], segments[i][j], 0.025f, 10, quadric);
	}

	// Draw skeleton joints
	std::vector<bool> validJoints = kinectSkeleton.getJointsValidity();
	std::vector<Point> joints = kinectSkeleton.getJointsPositions();
	std::vector<Quaternion> jointsOrientations = kinectSkeleton.getJointsOrientationsQuaternions();
	uint32 nJoints = basic_cast<uint32>(joints.size());
	bool oldValue = Config::canvas[Globals::INSTANCE_ID].showOrientations;
	for (uint32 i = 0; i < nJoints; i++)
	{
		if (enum_cast<KinectSkeleton::KinectJoint>(i) == KinectSkeleton::K_HEAD)
			Config::canvas[Globals::INSTANCE_ID].showOrientations = oldValue;
		else Config::canvas[Globals::INSTANCE_ID].showOrientations = false;
		if (validJoints[i]) renderSkeletonJoint(joints[i], jointsOrientations[i], 0.05f, 10, quadric);
	}
	Config::canvas[Globals::INSTANCE_ID].showOrientations = oldValue;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	gluDeleteQuadric(quadric);
}

void GLCanvas::render()
{
	SetCurrent();

	if (!initialized_) initializeGL();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	setModelview();
	
	// Draw room
	render3DRoom();

	// Draw Kinect cameras
	if (Config::system.currentMode != Config::KINECT_CLIENT)
	{
		for (uint32 i = 0; i < KinectManager::getNumberOfDevices(); i++)
		{
			Matrix4x4 kinectMatrix;
			if (RenderSystem::getKinectMatrix(kinectMatrix, i))
				renderKinectCamera(kinectMatrix);
		}
	}

	// Draw skeletons
	if (Config::canvas[Globals::INSTANCE_ID].showSkeletons)
	{
		uint32 nSkeletons = 0;
		KinectSkeleton skeletons[KINECT_SKELETON_COUNT];
		if (Config::canvas[Globals::INSTANCE_ID].combineSkeletons)
		{
			RenderSystem::getTransformedKinectSkeletons(nSkeletons, skeletons);
			for (uint32 j = 0; j < nSkeletons; j++)
				renderKinectSkeleton(skeletons[j]);
		}
		else
		{
			for (uint32 i = 0; i < KinectManager::getNumberOfDevices(); i++)
			{
				RenderSystem::getTransformedKinectSkeletons(nSkeletons, skeletons, i);
				for (uint32 j = 0; j < nSkeletons; j++)
					renderKinectSkeleton(skeletons[j]);
			}
		}
	}

	glFlush();
	SwapBuffers();
}

void GLCanvas::onResize(wxSizeEvent& event)
{
	setViewport();
	setProjection();
}

void GLCanvas::onMouseMove(wxMouseEvent& event)
{
	this->SetFocus();
	if (leftClicked_)
	{
		int32 currentX = event.GetX();
		int32 currentY = event.GetY();

		float32 deltaX = basic_cast<float32>(currentX - previousX_)/basic_cast<float32>(GetSize().x);
		float32 deltaY = basic_cast<float32>(currentY - previousY_)/basic_cast<float32>(GetSize().y);
		longitudeAngle_ -= deltaX*0.4f*PI32;
		latitudeAngle_ += deltaY*0.4f*PI32;
		if (longitudeAngle_ < 0.0f) longitudeAngle_ += 2.0f*PI32;
		latitudeAngle_ = std::min(std::max(latitudeAngle_, -0.3f*PI32), 0.3f*PI32);

		previousX_ = currentX;
		previousY_ = currentY;
	}
}

void GLCanvas::onLeftClick(wxMouseEvent& event)
{
	this->SetFocus();
	leftClicked_ = true;
	previousX_ = event.GetX();
	previousY_ = event.GetY();
}

void GLCanvas::onLeftRelease(wxMouseEvent& event)
{
	this->SetFocus();
	leftClicked_ = false;
}

void GLCanvas::onMouseWheel(wxMouseEvent& event)
{
	this->SetFocus();
	float32 delta = basic_cast<float32>(event.GetWheelDelta());
	if (event.ControlDown()) delta *= 0.0005f;
	else delta *= 0.004f;
	if (event.GetWheelRotation() < 0) zoomFactor_ += delta*std::max(0.1f*zoomFactor_, 0.01f);
	else zoomFactor_ -= delta*std::max(0.1f*zoomFactor_, 0.01f);
	zoomFactor_ = std::min(std::max(zoomFactor_, 0.0f), 1.0f);
}
