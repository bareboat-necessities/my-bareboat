/*
		Secondary Button Mouse stimulator for touch screen : long press to emulate right button, very long press for middle button

		Any move while pressing cancels the button action (move detected when x/y value change is > mouse_sensitivity parameter)

 Tested with :
	- Raspberry FT5406 7inch display
	- WaveShare WS170120 7inch USB display

 Compile with:

 g++ -Wall -Ofast ts-mouse.cpp -o ts-mouse -lX11 -lXtst -lXext

 Usage:

 ts-mouse [DeviceName]
 ts-mouse [DeviceName] [Time to click]
 ts-mouse [DeviceName] [Time to click] [Time to hold]
 ts-mouse [DeviceName] [Time to click] [Time to hold] [Mouse sensitivity]

 Default values:

 DeviceName = WS170120 (WaveShare 7inch USB)
 Time to click = .5 (500mS)
 Time to hold = 4 (4 S)
 Mouse sensitivity = 8 (any movement > 8 pixels cancels current click emulation)

 To list input devices:
 xinput --list

 Just :
 - compile the .cpp file
 - add a line in XDG autostart file : "/etc/xdg/lxsession/LXDE-pi/autostart":
 @/home/pi/mouse/ts-mouse

 (ts-mouse file is supposed to be in /home/pi/mouse ...)

 See also: https://www.raspberrypi.org/forums/viewtopic.php?t=121602&start=50

*/

#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <X11/extensions/XTest.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>
#include<fstream>
#include<iostream>

int main(int argc, char** argv)
	{
	const char* eventFile;

	Display* dpy = NULL;
	XEvent event;
	dpy = XOpenDisplay(NULL);

	int fd;
	struct input_event ie;
	Window root, child;
	int rootX, rootY, winX, winY;
	unsigned int mask;
	XQueryPointer(dpy, DefaultRootWindow(dpy), &root, &child, &rootX, &rootY, &winX, &winY, &mask);

	std::array<char, 128> buffer;
	std::string result;

	char *Handler;
	const char* deviceName = "WS170120"; // Default to WaveShare 7inch(H)
	float time_to_click = .5; // How long to hold the button down?
	float time_to_hold = 4; // Time pressed for holding click
	int move_sensitivity = 8 ; // nb of pixels to detect move (to cancel click emulation)

	if (argc >= 2)
		deviceName = argv[1];
	if (argc >= 3)
		time_to_click = atof(argv[2]);
	if( argc >= 4)
		time_to_hold = atof(argv[3]);
	if( argc >= 5)
		move_sensitivity = atoi(argv[4]);

	if ( asprintf( &Handler, "cat /proc/bus/input/devices | awk -v x='%s' '$0~x,EOF' | awk '/Handler/ {print $3; exit}'", deviceName ) <= 0 )
	{
		std::cerr << "Couldn't start command." << std::endl;
		return 1;
	}

//	std::cout << "Device: " << deviceName << " TimeRightClick: " << time_to_click << " TimeMiddleClick: " << time_to_hold << std::endl;

	FILE* pipe = popen(Handler, "r");

	if (!pipe)
	{
		std::cerr << "Couldn't start command." << std::endl;
		return 1;
	}
	while (fgets(buffer.data(), 128, pipe) != NULL)
		result += buffer.data();
	pclose(pipe);

	result = "/dev/input/" + result.substr(0, result.length() - 1);

//	std::cout << "Using file:" << result << std::endl;

	eventFile = result.c_str();

	if ((fd = open(eventFile, O_RDONLY)) == -1) {
		perror("opening device");
		return 1;
	}


	auto time_start_sec = ie.time.tv_sec;
	auto time_stop_sec = ie.time.tv_sec;
	auto time_start_usec = ie.time.tv_usec;
	auto time_stop_usec = ie.time.tv_usec;
	auto time_stop_usec_adj = ie.time.tv_usec;
	auto time_real = ie.time.tv_usec;
	float time_elapsed_adj;
	int pressed = 0;
	int cursor_x, cursor_y ;

//	std::cout << "Ready for mouse events..." << std::endl;

	while (true) {

		// Grab mouse events:
		read(fd, &ie, sizeof(struct input_event));


//		std::cout << "Type: " << ie.type << " Code: " << ie.code << " Value: " << ie.value << " Time: " << ie.time.tv_sec << "." << ie.time.tv_usec << std::endl;

		if (pressed && ie.type == EV_KEY && ie.code == BTN_TOUCH && ie.value == 0) {

			pressed = 0;

			// Timer from mouse events is used
			time_real = ie.time.tv_sec;
			time_stop_sec = time_real - time_start_sec;
			time_stop_usec = ie.time.tv_usec;

			if (time_stop_usec < time_start_usec) {

				time_stop_usec_adj = time_stop_usec + (1000000 - time_start_usec);
				if (time_real - time_start_sec > 0)
					time_stop_sec = time_stop_sec - 1;

			}
			else
				time_stop_usec_adj = time_stop_usec - time_start_usec;

			time_elapsed_adj = float(time_stop_sec) + (float(time_stop_usec_adj) / 1000000);

//			std::cout << "Released! ...  Time held: " << time_elapsed_adj << " seconds." << " Time: " << time_real << "." << time_stop_usec << std::endl;

			if (time_elapsed_adj >= time_to_click) {

				if (time_elapsed_adj >= time_to_hold)
				{

//					std::cout << "Middle button clicked!" << std::endl;
					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

					usleep(1000);

					XTestFakeButtonEvent(dpy, 2, True, CurrentTime);
					usleep(1000);

					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

					usleep(1000);

					XTestFakeButtonEvent(dpy, 2, False, CurrentTime);
					usleep(1000);
					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

				}
				else {

//					std::cout << "Right button clicked!" << std::endl;
					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

					usleep(1000);

					XTestFakeButtonEvent(dpy, 3, True, CurrentTime);
					usleep(1000);

					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

					usleep(1000);

					XTestFakeButtonEvent(dpy, 3, False, CurrentTime);
					usleep(1000);
					XQueryPointer(dpy, RootWindow(dpy, 0), &event.xbutton.root,
						&event.xbutton.window, &event.xbutton.x_root,
						&event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y,
						&event.xbutton.state);

				}
			}

		}


		// Mouse button is 'clicked' (initial press):
		if (ie.type == EV_KEY && ie.code == BTN_TOUCH && ie.value == 1) {
			pressed = 1;
			cursor_x = 0;
			cursor_y = 0;
			time_start_sec = ie.time.tv_sec;
			time_start_usec = ie.time.tv_usec;
//			std::cout << "\nPressed! ...  Time: " << time_start_sec << "." << time_start_usec << std::endl;

		}

		// Mouse has moved ... (cancel)
		if (pressed && ie.type == EV_ABS ){
			if(ie.code == ABS_MT_POSITION_X) {
				if( cursor_x == 0 )
					cursor_x = ie.value ;
				else if( abs( ie.value - cursor_x ) > move_sensitivity )
					pressed = 0;
			}
			else if(ie.code == ABS_MT_POSITION_Y) {
				if( cursor_y == 0 )
					cursor_y = ie.value ;
				else if( abs( ie.value - cursor_y ) > move_sensitivity )
					pressed = 0;
			}
			if( !pressed ) {
				time_start_sec = ie.time.tv_sec;
				time_start_usec = ie.time.tv_usec;
//				std::cout << "\nMove! ...  Time: " << time_start_sec << "." << time_start_usec << std::endl;
			}
		}
	}

	XCloseDisplay(dpy);

	return 0;

}
