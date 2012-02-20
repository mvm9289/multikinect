#include <vrpn/vrpn_Tracker.h>
#include <vrpn/vrpn_Button.h>
#include <vrpn/vrpn_Analog.h>

#include <iostream>
using namespace std;

void VRPN_CALLBACK handler_kinect_tracker(void* userData, const vrpn_TRACKERCB t)
{
	cout << "SkeletonTracker:" << endl;
	cout << "\tID: " << t.sensor << endl;
	cout << "\tPosition: " << t.pos[0] << " " << t.pos[1] << " " << t.pos[2] << endl;
	cout << "\tOrientation: " << t.quat[0] << " " << t.quat[1] << " " << t.quat[2] << " " << t.quat[3] << endl << endl;
}

void VRPN_CALLBACK handler_wiimote_button(void* userData, const vrpn_BUTTONCB b)
{
	cout << "Wiimote:" << endl;
	if (b.state)	cout << "\tButton pressed: ";
	else			cout << "\tButton released: ";
	cout << b.button << endl << endl;
}

void VRPN_CALLBACK handler_wiimote_analog(void* userData, const vrpn_ANALOGCB a)
{
	cout << "Wiimote:" << endl;
	for (int i = 0; i < a.num_channel; i++)
	{
		cout << "\tChannel " << i << ": " << a.channel[i] << endl;
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	vrpn_Tracker_Remote *vrpnTracker	=	new vrpn_Tracker_Remote("SkeletonTracker0@localhost");
	vrpn_Button_Remote	*vrpnButton		=	new vrpn_Button_Remote("WiiMote0@localhost");
	vrpn_Analog_Remote	*vrpnAnalog		=	new vrpn_Analog_Remote("WiiMote0@localhost");

	vrpnTracker->register_change_handler(0, handler_kinect_tracker);
	vrpnButton->register_change_handler(0, handler_wiimote_button);
	vrpnAnalog->register_change_handler(0, handler_wiimote_analog);

	while (true)
	{
		vrpnTracker->mainloop();
		vrpnButton->mainloop();
		vrpnAnalog->mainloop();
	}

	return 0;
}

