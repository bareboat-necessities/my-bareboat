/*
 Copyright (C) 2010, 2011 Philipp Merkel <linux@philmerk.de>

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/Xrandr.h>
#include "twofingemu.h"
#include "gestures.h"
#include "easing.h"
#include "devices.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <time.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int debugMode = 0;

int inDebugMode() {
	return debugMode;
}

/* Daemonize. Source: http://www-theorie.physik.unizh.ch/~dpotter/howto/daemonize (public domain) */
static void daemonize(void) {
	pid_t pid, sid;

	/* already a daemon */
	if (getppid() == 1)
		return;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory.  This prevents the current
	 directory from being locked; hence not being able to remove it. */
	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	/* Redirect standard files to /dev/null */
	void* r;
	r = freopen("/dev/null", "r", stdin);
	r = freopen("/dev/null", "w", stdout);
	r = freopen("/dev/null", "w", stderr);
	r = r;
}

/* Finger information */
FingerInfo fingerInfos[2] = { { .rawX=0, .rawY=0, .rawZ=0, .id = -1, .slotUsed = 0, .setThisTime = 0 }, { .rawX=0, .rawY=0, .rawZ=0,
		.id = -1, .slotUsed = 0, .setThisTime = 0 } };

/* X stuff */
Display* display;
Window root;
int screenNum;
int deviceID;
int calibrateDeviceID;
Atom WM_CLASS;
pthread_t xLoopThread;
int randrEvBase;
int randrErrBase;
int xinputEvBase;
int xinputErrBase;

/* Calibration data */
int calibMinX, calibMaxX, calibMinY, calibMaxY;
unsigned char calibSwapX, calibSwapY, calibSwapAxes;
float calibMatrix[6];
int calibMatrixUse = 0;

/* The width and height of the screen in pixels */
unsigned int screenWidth, screenHeight;

/* Finger data */
int fingersDown = 0;
int fingersWereDown = 0;
/* Has button press of first button been called in XTest? */
int buttonDown = 0;

/* Does the device use the legacy MT protocol? */
int useLegacyProtocol = 0;

/* Blocking Device */
int blockingDeviceID = -1;
int blockingIntervalMilliseconds = BLOCKING_INTERVAL_MS_DEFAULT;
TimeVal lastBlockingInputTime = { 0, 0};
int currentTouchBlocked = 0;

/* Handle errors by, well, throwing them away. */
int invalidWindowHandler(Display *dsp, XErrorEvent *err) {
	return 0;
}

/* Grab the device so input is captured */
void grab(Display* display, int grabDeviceID) {
	XIEventMask device_mask;
	unsigned char mask_data[8] = { 0,0,0,0,0,0,0,0 };
	device_mask.mask_len = sizeof(mask_data);
	device_mask.mask = mask_data;
	XISetMask(device_mask.mask, XI_Motion);
	XISetMask(device_mask.mask, XI_ButtonPress);

/* Experiments with X MT support, not working yet */
//	XISetMask(device_mask.mask, XI_TouchBegin);
//	XISetMask(device_mask.mask, XI_TouchUpdate);
//	XISetMask(device_mask.mask, XI_TouchEnd);
//	XIGrabModifiers modifiers;
//	modifiers.modifiers = XIAnyModifier;
//	int r = XIGrabButton(display, grabDeviceID, XIAnyButton, root, None, GrabModeSync,
//			GrabModeAsync, True, &device_mask, 1, &modifiers);
//	int r = XIGrabTouchBegin(display, grabDeviceID, root, None, &device_mask, 0, modifiers);

	int r = XIGrabDevice(display, grabDeviceID, root, CurrentTime, None, GrabModeAsync, GrabModeAsync, False, &device_mask);

	if(debugMode) printf("Grab Result: %i\n", r);

}

/* Ungrab the device so input can be handled by application directly */
void ungrab(Display* display, int grabDeviceID) {
/* Experiments with X MT support, not working yet */
//	XIGrabModifiers modifiers[1] = { { 0, 0 } };
//	XIUngrabButton(display, grabDeviceID, 1, root, 1, modifiers);
	XIUngrabDevice(display, grabDeviceID, CurrentTime);
}

TimeVal getCurrentTime() {

	TimeVal time;
	gettimeofday(&time, NULL);

	return time;
}

int timeDiff(TimeVal start, TimeVal end)
{
	long seconds  = end.tv_sec  - start.tv_sec;
	long microSeconds = end.tv_usec - start.tv_usec;

	return (seconds * 1000 + microSeconds/1000);
}



/* Send an XTest event to release the first button if it is currently pressed */
void releaseButton() {
	if (buttonDown) {
		buttonDown = 0;
		XTestFakeButtonEvent(display, 1, False, CurrentTime);
		XFlush(display);

/* Experiments with Pressure Sensitivity, not working yet */
/*		XDevice * dev = XOpenDevice(display, deviceID);
		int axes[3] = {fingerInfos[0].rawX, fingerInfos[0].rawY, fingerInfos[0].rawZ};
		XTestFakeDeviceButtonEvent(display, dev, 1, False, axes, 3, 0);
		XCloseDevice(display, dev);

		XFlush(display);
		grab(display, deviceID);*/
	}
}
/* Send an XTest event to press the first button if it is not pressed yet */
void pressButton() {
	if(!buttonDown) {

/* Experiments with Pressure Sensitivity, not working yet */
/*		XDevice * dev = XOpenDevice(display, deviceID);
		int axes[3] = {fingerInfos[0].rawX, fingerInfos[0].rawY, fingerInfos[0].rawZ};
		XTestFakeDeviceButtonEvent(display, dev, 1, False, axes, 3, 0);
		XCloseDevice(display, dev);
		XFlush(display);

		ungrab(display, deviceID);
		XFlush(display);

		printf("PRESS!\n");
		dev = XOpenDevice(display, deviceID);
		XTestFakeDeviceButtonEvent(display, dev, 1, True, axes, 3, 0);
		XCloseDevice(display, dev);
*/
		buttonDown = 1;
		XTestFakeButtonEvent(display, 1, True, CurrentTime);
		XFlush(display);
	}
}
/* Is the first button currently pressed? */
int isButtonDown() {
	return buttonDown;
}


/* Moves the pointer to the given position */
void movePointer(int x, int y, int z) {
	/* Move pointer to center between touch points */

	/* Experiments with XI events, not working yet */
	//	int axes[3] = {x, y, z};
	//	XDevice * dev = XOpenDevice(display, 17);
	//	XTestFakeDeviceMotionEvent(display, dev, False, 0, axes, 2, 0);
	//	XCloseDevice(display, dev);

	XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
	XFlush(display);
}


/* Executes the given action -- synthesizes key/button press, release or both, depending
 * on value of whatToDo (EXECUTEACTION_PRESS/_RELEASE/_BOTH). */
void executeAction(Action* action, int whatToDo) {
	if (whatToDo & EXECUTEACTION_PRESS) {
		if (action->actionType != ACTIONTYPE_NONE && action->modifier != 0) {
			if (action->modifier & MODIFIER_SHIFT) {
				XTestFakeKeyEvent(display,
						XKeysymToKeycode(display, XK_Shift_L), True,
						CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_CONTROL) {
				XTestFakeKeyEvent(display, XKeysymToKeycode(display,
						XK_Control_L), True, CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_ALT) {
				XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Alt_L),
						True, CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_SUPER) {
				XTestFakeKeyEvent(display,
						XKeysymToKeycode(display, XK_Super_L), True,
						CurrentTime);
				XFlush(display);
			}
		}

		switch (action->actionType) {
		case ACTIONTYPE_BUTTONPRESS:
			XTestFakeButtonEvent(display, action->keyButton, True, CurrentTime);
			XFlush(display);
			break;
		case ACTIONTYPE_KEYPRESS:
			XTestFakeKeyEvent(display, XKeysymToKeycode(display,
					action->keyButton), True, CurrentTime);
			XFlush(display);
			break;
		}

	}

	if (whatToDo & EXECUTEACTION_RELEASE) {

		switch (action->actionType) {
		case ACTIONTYPE_BUTTONPRESS:
			XTestFakeButtonEvent(display, action->keyButton, False, CurrentTime);
			XFlush(display);
			break;
		case ACTIONTYPE_KEYPRESS:
			XTestFakeKeyEvent(display, XKeysymToKeycode(display,
					action->keyButton), False, CurrentTime);
			XFlush(display);
			break;
		}

		if (action->actionType != ACTIONTYPE_NONE && action->modifier != 0) {
			if (action->modifier & MODIFIER_SHIFT) {
				XTestFakeKeyEvent(display,
						XKeysymToKeycode(display, XK_Shift_L), False,
						CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_CONTROL) {
				XTestFakeKeyEvent(display, XKeysymToKeycode(display,
						XK_Control_L), False, CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_ALT) {
				XTestFakeKeyEvent(display, XKeysymToKeycode(display, XK_Alt_L),
						False, CurrentTime);
				XFlush(display);
			}
			if (action->modifier & MODIFIER_SUPER) {
				XTestFakeKeyEvent(display,
						XKeysymToKeycode(display, XK_Super_L), False,
						CurrentTime);
				XFlush(display);
			}
		}
	}

}

Window getParentWindow(Window w) {
	Window root, parent;
	Window* childWindows = NULL;
	unsigned int childCount;
	if(XQueryTree(display, w, &root, &parent, &childWindows, &childCount)) {

		if (childWindows != NULL)
			XFree(childWindows);
		return parent;
	} else {
		return None;
	}

}

Window getLastChildWindow(Window w) {
	Window root, parent;
	Window* childWindows = NULL;
	unsigned int childCount;
	if(XQueryTree(display, w, &root, &parent, &childWindows, &childCount)) {

		if (childWindows != NULL) {
			if(childCount > 0) {
				Window child = childWindows[childCount - 1];
				XFree(childWindows);
				return child;
			}
			XFree(childWindows);
		}
		return None;
	} else {
		return None;
	}

}

Window getActiveWindow() {

/*	Window *root_return = None, *child_return = None;
	int *root_x_return = 0, *root_y_return = 0;
	int *win_x_return = 0, *win_y_return = 0;
	unsigned int *mask_return = 0;

	XQueryPointer(display, root, root_return, child_return, root_x_return, root_y_return, 
                     win_x_return, win_y_return, mask_return);

	return *child_return;*/
	return getCurrentWindow();

}

/* Returns the active top-level window. A top-level window is one that has WM_CLASS set.
 * May also return None. */
Window getCurrentWindow() {

	/* First get the window that has the input focus */
	Window currentWindow;
	int revert;
	XGetInputFocus(display, &currentWindow, &revert);

	if (currentWindow == None) {
		//if(debugMode) printf("Leave getCurrentWindow\n");
		return currentWindow;
	}

	/* Now go through parent windows until we find one with WM_CLASS set. */


	XClassHint* classHint = XAllocClassHint();
	if(classHint == NULL) {
		if(debugMode) printf("Couldn't allocate class hint!!\n");
		return None;
	}

	int i = 0;
	while (1) {
		//if(debugMode) printf("in Loop\n");
		i++;
		if(i >= 5) {
			if(debugMode) printf("Too many iterations in getCurrentWindow\n");
			XFree(classHint);
			//if(debugMode) printf("Leave getCurrentWindow\n");
			return None;
		}
		if (currentWindow == root || currentWindow == None) {
			//if(debugMode) printf("Reached root!\n");
			/* No top-level window available. Should never happen. */
			XFree(classHint);
			//if(debugMode) printf("Leave getCurrentWindow\n");
			return currentWindow;
		}

		//if(debugMode) printf("Call XGetClassHint!\n");
		if (XGetClassHint(display, currentWindow, classHint) == 0) {
			//if(debugMode) printf("Has no Class!\n");
			/* Has no WM_CLASS, thus no top-level window */
			Window parent = getParentWindow(currentWindow);

			if(parent == None || currentWindow == parent) {
				/* something wrong */
				XFree(classHint);
				return currentWindow;
			}
			/* Continue with parent until we find WM_CLASS */
			currentWindow = parent;
		} else {
			//if(debugMode) printf("Clean up class name!\n");
			if(classHint->res_class != NULL) XFree(classHint->res_class);
			if(classHint->res_name != NULL) XFree(classHint->res_name);
			XFree(classHint);
			//if(debugMode) printf("Leave getCurrentWindow\n");
			return currentWindow;
		}
	}
}


/* Sets the calibrated x, y coordinates from the raw coordinates in the given FingerInfo */
void calibrate(FingerInfo* fingerInfo) {
	float xf; float yf;
	if (calibSwapAxes) {

		xf = ((float)(fingerInfo->rawY - calibMinX))/((float) (calibMaxX-calibMinX));
		yf = ((float)(fingerInfo->rawX - calibMinY))/((float) (calibMaxY-calibMinY));

	} else {

		xf = ((float)(fingerInfo->rawX - calibMinX))/((float) (calibMaxX-calibMinX));
		yf = ((float)(fingerInfo->rawY - calibMinY))/((float) (calibMaxY-calibMinY));

	}
	if (calibSwapX) xf = 1 - xf;
	if (calibSwapY) yf = 1 - yf;

	if(calibMatrixUse) {
		float xfold = xf;
		/* Apply matrix transformation */
		xf = xf * calibMatrix[0] + yf * calibMatrix[1] + calibMatrix[2];
		yf = xfold * calibMatrix[3] + yf * calibMatrix[4] + calibMatrix[5];
	}

	fingerInfo->x = xf * screenWidth;
	fingerInfo->y = yf * screenHeight;

	if (fingerInfo->x < 0)
		fingerInfo->x = 0;
	if (fingerInfo->y < 0)
		fingerInfo->y = 0;
	if (fingerInfo->x > screenWidth)
		fingerInfo->x = screenWidth;
	if (fingerInfo->y > screenHeight)
		fingerInfo->y = screenHeight;


}

/* Process the finger data gathered from the last set of events */
void processFingers() {
	int i;
	fingersDown = 0;
	for(i = 0; i < 2; i++) {
		if(fingerInfos[i].slotUsed) {
			calibrate(&(fingerInfos[i]));
			fingersDown++;
		}
	}

	if(fingersWereDown == 0 && 
	   fingersDown > 0 && 
	   blockingDeviceID != -1 && 
	   timeDiff(lastBlockingInputTime, getCurrentTime()) < blockingIntervalMilliseconds) {
		currentTouchBlocked = 1;
		if(debugMode) printf("Touch blocked.\n");
	}

	processFingerGesture(fingerInfos, fingersDown, fingersWereDown, currentTouchBlocked);

	if(fingersDown == 0) {
		currentTouchBlocked = 0;
	}

	/* Save number of fingers to compare next time */
	fingersWereDown = fingersDown;
}

/* Returns a pointer to the profile of the currently selected
 * window, or defaultProfile if there is no specific profile for it or the window is invalid. */
char* getWindowClass(Window w) {
	char * result = NULL;
	if (w != None) {

		XClassHint* classHint = XAllocClassHint();

		/* Read WM_CLASS member */
		if (XGetClassHint(display, w, classHint)) {

			if(classHint->res_class != NULL) {

				result = malloc((strlen(classHint->res_name) + 1) * sizeof(char));
				if(result != NULL) {
					strcpy(result, classHint->res_name);
				}
				XFree(classHint->res_class);

			}

			if(classHint->res_name != NULL) XFree(classHint->res_name);
			XFree(classHint);
		}
	}

	return result;

}


/* Returns whether the given window is blacklisted */
int isWindowBlacklisted(Window w) {
	if(w == None) return 0;

	return isWindowBlacklistedForGestures(w);
}

void setScreenSize(XRRScreenChangeNotifyEvent * evt) {
	screenWidth = evt->width;
	screenHeight = evt->height;
	if(debugMode) {
		printf("New screen size: %i x %i\n", screenWidth, screenHeight);
	}
}



/* X thread that processes X events in parallel to kernel device loop */
void handleXEvent() {
	XEvent ev;
	XNextEvent(display, &ev);
	//if(debugMode) printf("Handle event\n");
	if (XGetEventData(display, &(ev.xcookie))) {
		XGenericEventCookie *cookie = &(ev.xcookie);

		if (cookie->evtype == XI_Motion || cookie->evtype == XI_ButtonPress) {
			XIDeviceEvent * devEvt = (XIDeviceEvent*) cookie->data;
			if(blockingDeviceID != -1 && devEvt->deviceid == blockingDeviceID) {
				// Blocking event received
				if(debugMode) printf("Blocking for next %i milliseconds.\n", blockingIntervalMilliseconds);
				lastBlockingInputTime = getCurrentTime();
			}
		}

		if (cookie->evtype == XI_PropertyEvent) {
			/* Device properties changed -> recalibrate. */
			if(debugMode) printf("Device properties changed.\n");
			readCalibrationData(0);
		}

		/*if (cookie->evtype == XI_Motion) {
			//if(debugMode) printf("Motion event\n");
			//int r = XIAllowEvents(display, deviceID, XIReplayDevice, CurrentTime);
			//printf("XIAllowEvents result: %i\n", r);
		} else if(cookie->evtype == XI_ButtonPress) {
			//if(debugMode) printf("Button Press\n");
			//int r = XIAllowEvents(display, deviceID, XIReplayDevice, CurrentTime);
			//printf("XIAllowEvents result: %i\n", r);
		}*/

		// In an ideal world, the following would work. But unfortunately the touch events
		// delivered by evdev are often crap on the eGalax screen, with missing events when there
		// should be some. So we still have to read directly from the input device, as bad as that is.
		//if (cookie->evtype == XI_TouchBegin || cookie->evtype == XI_TouchUpdate || cookie->evtype == XI_TouchEnd) {
		//	XIDeviceEvent * devEvt = (XIDeviceEvent*) cookie->data;
		//	printf("Detail: %i\n", devEvt->detail);
		//	printf("Mask[0]: %i\n", (int) devEvt->valuators.mask[0]);
		//
		//	/* Look for slot to put the data into by looking at the tracking ids */
		//	int index = -1;
		//	int i;
		//	for(i = 0; i < 2; i++) {
		//		if(fingerInfos[i].id == devEvt->detail) {
		//			index = i;
		//			break;
		//		}
		//	}
		//
		//	/* No slot for this id found, look for free one */
		//	if(index == -1) {
		//		for(i = 0; i < 2; i++) {
		//			if(!fingerInfos[i].slotUsed) {
		//				/* "Empty" slot, so we can add it. */
		//				index = i;
		//				fingerInfos[i].id = devEvt->detail;
		//				break;
		//			}
		//		}
		//	}
		//
		//	/* We have found a slot */
		//	if(index != -1) {
		//		fingerInfos[index].slotUsed = (cookie->evtype != XI_TouchEnd ? 1 : 0);
		//
		//		i = 0;
		//		if((devEvt->valuators.mask[0] & 1) == 1)
		//		{
		//			fingerInfos[index].rawX = (int) devEvt->valuators.values[i++];
		//		}
		//		if((devEvt->valuators.mask[0] & 2) == 2)
		//		{
		//			fingerInfos[index].rawY = (int) devEvt->valuators.values[i++];
		//		}
		//		if((devEvt->valuators.mask[0] & 4) == 4)
		//		{
		//			fingerInfos[index].rawZ = (int) devEvt->valuators.values[i++];
		//		}
		//	}
		//
		//	processFingers();
		//}


		XFreeEventData(display, &(ev.xcookie));

	} else {
		if(ev.type == randrEvBase + RRScreenChangeNotify) {
			setScreenSize((XRRScreenChangeNotifyEvent *) &ev);
		}
	}
}

/* Reads the calibration data from evdev, should be self-explanatory. */
void readCalibrationData(int exitOnFail) {
	if(debugMode) {
		printf("Start calibration\n");
	}
	Atom retType;
	int retFormat;
	unsigned long retItems, retBytesAfter;
	unsigned int* data;
	if(XIGetProperty(display, calibrateDeviceID, XInternAtom(display,
			"Evdev Axis Calibration", 0), 0, 4 * 32, False, XA_INTEGER,
			&retType, &retFormat, &retItems, &retBytesAfter,
			(unsigned char**) &data) != Success) {
		data = NULL;
	}

	if (data == NULL || retItems != 4 || data[0] == data[1] || data[2] == data[3]) {
		/* evdev might not be ready yet after resume. Let's wait a second and try again. */
		sleep(1);

		if(XIGetProperty(display, calibrateDeviceID, XInternAtom(display,
				"Evdev Axis Calibration", 0), 0, 4 * 32, False, XA_INTEGER,
				&retType, &retFormat, &retItems, &retBytesAfter,
				(unsigned char**) &data) != Success) {
			return;
		}

		if (retItems != 4 || data[0] == data[1] || data[2] == data[3]) {

			if(debugMode) {
				printf("No calibration data found, use default values.\n");
			}

			/* Get minimum/maximum of axes */

			int nDev;
			XIDeviceInfo * deviceInfo = XIQueryDevice(display, calibrateDeviceID, &nDev);

			int c;
			for(c = 0; c < deviceInfo->num_classes; c++) {
				if(deviceInfo->classes[c]->type == XIValuatorClass) {
					XIValuatorClassInfo* valuatorInfo = (XIValuatorClassInfo *) deviceInfo->classes[c];
					if(valuatorInfo->mode == XIModeAbsolute) {
						if(valuatorInfo->label == XInternAtom(display, "Abs X", 0)
						|| valuatorInfo->label == XInternAtom(display, "Abs MT Position X", 0)) 
						{
							calibMinX = valuatorInfo->min;
							calibMaxX = valuatorInfo->max;
						}
						else if(valuatorInfo->label == XInternAtom(display, "Abs Y", 0) 							|| valuatorInfo->label == XInternAtom(display, "Abs MT Position Y", 0))
						{
							calibMinY = valuatorInfo->min;
							calibMaxY = valuatorInfo->max;
						}
					}
				}
			}

			XIFreeDeviceInfo(deviceInfo);

		} else {
			calibMinX = data[0];
			calibMaxX = data[1];
			calibMinY = data[2];
			calibMaxY = data[3];
		}
	} else {
		calibMinX = data[0];
		calibMaxX = data[1];
		calibMinY = data[2];
		calibMaxY = data[3];
	}

	if(data != NULL) {
		XFree(data);
	}

	float * data4 = NULL;
	if(XIGetProperty(display, calibrateDeviceID, XInternAtom(display,
			"Coordinate Transformation Matrix", 0), 0, 9 * 32, False, XInternAtom(display,
			"FLOAT", 0),
			&retType, &retFormat, &retItems, &retBytesAfter,
			(unsigned char **) &data4) != Success) {
		data4 = NULL;
	}
	calibMatrixUse = 0;
	if(data4 != NULL && retItems == 9) {
		int i;
		for(i = 0; i < 6; i++) {
			/* We only take the first two rows of the matrix, the rest is unimportant anyway */
			calibMatrix[i] = data4[i];
		}
		calibMatrixUse = 1;
	}
	if(data4 != NULL) {
		XFree(data4);
	}



	unsigned char* data2;

	if(XIGetProperty(display, calibrateDeviceID, XInternAtom(display,
			"Evdev Axis Inversion", 0), 0, 2 * 8, False, XA_INTEGER, &retType,
			&retFormat, &retItems, &retBytesAfter, (unsigned char**) &data2) != Success) {
		return;
	}

	if (retItems != 2) {
		if (exitOnFail) {
			printf("No valid axis inversion data found.\n");
			exit(1);
		} else {
			return;
		}
	}

	calibSwapX = data2[0];
	calibSwapY = data2[1];

	XFree(data2);

	if(XIGetProperty(display, calibrateDeviceID,
			XInternAtom(display, "Evdev Axes Swap", 0), 0, 8, False,
			XA_INTEGER, &retType, &retFormat, &retItems, &retBytesAfter,
			(unsigned char**) &data2) != Success) {
		return;
	}

	if (retItems != 1) {
		if (exitOnFail) {
			printf("No valid axes swap data found.\n");
			exit(1);
		} else {
			return;
		}
	}
	calibSwapAxes = data2[0];

	XFree(data2);

	if(debugMode)
	{
		printf("Calibration: MinX: %i; MaxX: %i; MinY: %i; MaxY: %i\n", calibMinX, calibMaxX, calibMinY, calibMaxY);
		printf("Invert X Axis: %s\n", calibSwapX ? "Yes" : "No");
		printf("Invert Y Axis: %s\n", calibSwapY ? "Yes" : "No");
		printf("Swap Axes: %s\n", calibSwapAxes ? "Yes" : "No");
		if(calibMatrixUse)
		{
			printf("Calibration Matrix: \t%f\t%f\t%f\n                    \t%f\t%f\t%f\n", calibMatrix[0], calibMatrix[1], calibMatrix[2], calibMatrix[3], calibMatrix[4], calibMatrix[5]);
		}
	}

}


/* Main function, contains kernel driver event loop */
int main(int argc, char **argv) {

	char* devname = 0;
	int doDaemonize = 1;
	int doWait = 0;
	int clickMode = 2;
	int justVersion = 0;

	char* blockingDevName = 0;

	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--debug") == 0) {
			doDaemonize = 0;
			debugMode = 1;
		} else if (strcmp(argv[i], "--version") == 0) {
			justVersion = 1;
		} else if (strcmp(argv[i], "--wait") == 0) {
			doWait = 1;
		} else if (strcmp(argv[i], "--click=first") == 0) {
			clickMode = 0;
		} else if (strcmp(argv[i], "--click=second") == 0) {
			clickMode = 1;
		} else if (strcmp(argv[i], "--click=center") == 0) {
			clickMode = 2;
		} else if (strcmp(argv[i], "--blockingdevice") == 0) {
			if(i + 1 < argc) {
				blockingDevName = argv[++i];
			}
		} else if (strcmp(argv[i], "--blockinginterval") == 0) {
			if(i + 1 < argc) {
				blockingIntervalMilliseconds = atoi(argv[++i]);
			}
		} else {
			devname = argv[i];
		}

	}

	if(debugMode || justVersion)
	{
		printf("twofing, the two-fingered daemon\nVersion %s\n\n", VERSION);
	}

	if(justVersion)
	{ 
		return 0;
	}

	initGestures(clickMode);



	if (doDaemonize) {
		daemonize();
	}

	if (doWait) {
		/* Wait until all necessary things are loaded */
		sleep(10);
	}


	/* Connect to X server */
	if ((display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "ERROR: Couldn't connect to X server\n");
		exit(1);
	}

	/* Read X data */
	screenNum = DefaultScreen(display);

	root = RootWindow(display, screenNum);

//	realDisplayWidth = DisplayWidth(display, screenNum);
//	realDisplayHeight = DisplayHeight(display, screenNum);

	WM_CLASS = XInternAtom(display, "WM_CLASS", 0);

	/* Get notified about new windows */
	XSelectInput(display, root, StructureNotifyMask | SubstructureNotifyMask);

	//TODO load blacklist and profiles from file(s)

	/* Device file name */
	if (devname == 0) {
		devname = "/dev/twofingtouch";
	}

	/* Try to read from device file */
	int fileDesc;
	if ((fileDesc = open(devname, O_RDONLY)) < 0) {
		perror("/dev/twofingtouch");
		return 1;
	}

	fd_set fileDescSet;
	FD_ZERO(&fileDescSet);

	int eventQueueDesc = XConnectionNumber(display);	

	while (1) {
		/* Perform initialization at beginning and after module has been re-loaded */
		int rd, i;
		struct input_event ev[64];

		char name[256] = "Unknown";

		/* Read device name */
		ioctl(fileDesc, EVIOCGNAME(sizeof(name)), name);
		printf("Input device name: \"%s\"\n", name);

		/* Look if a mapping is available and, if yes, map calibration device name */
		char calibrateName[256];
		strcpy(calibrateName, name);
		for(i = 0; mapDeviceNameForCalibration[i].origDeviceName != NULL; i++)
		{
			if(strcmp(name, mapDeviceNameForCalibration[i].origDeviceName) == 0)
			{
				strcpy(calibrateName, mapDeviceNameForCalibration[i].mappedDeviceName);
				printf("For calibration: \"%s\"\n", calibrateName);
				break;
			}
		}

		//TODO activate again?
		//XSetErrorHandler(invalidWindowHandler);


		int opcode;
		if (!XQueryExtension(display, "RANDR", &opcode, &randrEvBase,
				&randrErrBase)) {
			fprintf(stderr, "ERROR: X RANDR extension not available.\n");
			XCloseDisplay(display);
			exit(1);
		}

		/* Which version of XRandR? We support 1.3 */
		int major = 1, minor = 3;
		if (!XRRQueryVersion(display, &major, &minor)) {
			fprintf(stderr, "ERROR: XRandR version not available.\n");
			XCloseDisplay(display);
			exit(1);
		} else if(!(major>1 || (major == 1 && minor >= 3))) {
			fprintf(stderr, "ERROR: XRandR 1.3 not available. Server supports %d.%d\n", major, minor);
			XCloseDisplay(display);
			exit(1);
		}

		/* XInput Extension available? */
		if (!XQueryExtension(display, "XInputExtension", &opcode, &xinputEvBase,
				&xinputErrBase)) {
			fprintf(stderr, "ERROR: X Input extension not available.\n");
			XCloseDisplay(display);
			exit(1);
		}

		/* Which version of XI2? We support 2.1 */
		major = 2; minor = 1;
		if (XIQueryVersion(display, &major, &minor) == BadRequest) {
			fprintf(stderr, "ERROR: XI 2.1 not available. Server supports %d.%d\n", major, minor);
			XCloseDisplay(display);
			exit(1);
		}

		screenWidth = XDisplayWidth(display, screenNum);
		screenHeight = XDisplayHeight(display, screenNum);

		int n;
		XIDeviceInfo *info = XIQueryDevice(display, XIAllDevices, &n);
		if (!info) {
			fprintf(stderr, "ERROR: No XInput devices available\n");
			exit(1);
		}

		/* Go through input devices and look for that with the same name as the given device */
		deviceID = -1;
		calibrateDeviceID = -1;
		blockingDeviceID = -1;
		int devindex;
		for(devindex = 0; devindex < n; devindex++) {
			if(info[devindex].use == XIMasterPointer || info[devindex].use
					== XIMasterKeyboard)
				continue;

			if(strcmp(info[devindex].name, name) == 0) {
				deviceID = info[devindex].deviceid;
			}
			if(strcmp(info[devindex].name, calibrateName) == 0) {
                                calibrateDeviceID = info[devindex].deviceid;
                        }
			if(blockingDevName != 0 && strcmp(info[devindex].name, blockingDevName) == 0) {
				blockingDeviceID = info[devindex].deviceid;
			}
		}
		if(deviceID == -1) {
			fprintf(stderr, "ERROR: Input device not found in XInput device list!\n");
			exit(1);
		}
		if(calibrateDeviceID == -1) {
			printf("Using default device for calibration\n");
			calibrateDeviceID = deviceID;
		}
		if(blockingDevName != 0) {
			if(blockingDeviceID == -1) {
				printf("WARNING: Blocking device \"%s\" not found in XInput device list!\n", blockingDevName);
			} else {
				printf("Blocking on device %i.\n", blockingDeviceID);
			}
		}

		XIFreeDeviceInfo(info);

		if(debugMode) printf("XInput device id is %i.\n", deviceID);
		if(debugMode) printf("XInput device id for calibration is %i.\n", calibrateDeviceID);

		/* Prepare by reading calibration */
		readCalibrationData(1);

		/* Receive device property change events */
		XIEventMask device_mask2;
		unsigned char mask_data2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		device_mask2.deviceid = deviceID;
		device_mask2.mask_len = sizeof(mask_data2);
		device_mask2.mask = mask_data2;
		XISetMask(device_mask2.mask, XI_PropertyEvent);
		XISetMask(device_mask2.mask, XI_ButtonPress);
		//XISetMask(device_mask2.mask, XI_TouchBegin);
		//XISetMask(device_mask2.mask, XI_TouchUpdate);
		//XISetMask(device_mask2.mask, XI_TouchEnd);
		XISelectEvents(display, root, &device_mask2, 1);

		/* Recieve events when screen size changes */
		XRRSelectInput(display, root, RRScreenChangeNotifyMask);

		/* Receive events from blocking device */
		if(blockingDeviceID != -1)
		{
			XIEventMask device_mask3;
			unsigned char mask_data3[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			device_mask3.deviceid = blockingDeviceID;
			device_mask3.mask_len = sizeof(mask_data3);
			device_mask3.mask = mask_data3;
			XISetMask(device_mask3.mask, XI_ButtonPress);
			XISetMask(device_mask3.mask, XI_Motion);
			XISelectEvents(display, root, &device_mask3, 1);
		}


		/* Receive touch events */



		/* Needed for XTest to work correctly */
		XTestGrabControl(display, True);


		/* Needed for some reason to receive events */
/*		XGrabPointer(display, root, False, 0, GrabModeAsync, GrabModeAsync,
				None, None, CurrentTime);
		XUngrabPointer(display, CurrentTime);*/

		grab(display, deviceID);		

		printf("Reading input from device ... (interrupt to exit)\n");

		/* We perform raw event reading here as X touch events don't seem too reliable */
		int currentSlot = 0;

		/* If we use the legacy protocol, we collect all data of one finger into tempFingerInfo and set
		   it to the correct slot once MT_SYNC occurs. */
		FingerInfo tempFingerInfo = { .rawX=0, .rawY=0, .rawZ=0, .id = -1, .slotUsed = 0, .setThisTime = 0 };

		while (1) {


			FD_SET(fileDesc, &fileDescSet);
			FD_SET(eventQueueDesc, &fileDescSet);

			select(MAX(fileDesc, eventQueueDesc) + 1, &fileDescSet, NULL, NULL, getEasingStepTimeVal());
			
			checkEasingStep();

			if(FD_ISSET(fileDesc, &fileDescSet))
			{


				rd = read(fileDesc, ev, sizeof(struct input_event) * 64);
				if (rd < (int) sizeof(struct input_event)) {
					printf("Data stream stopped\n");
					break;
				}
				for (i = 0; i < rd / sizeof(struct input_event); i++) {

					if (ev[i].type == EV_SYN) {
						if (0 == ev[i].code) { // Ev_Sync event end
							/* All finger data received, so process now. */

							if(useLegacyProtocol) {
								/* Clear slots not set this time */
								int i;
								for(i = 0; i < 2; i++) {
									if(fingerInfos[i].setThisTime) {
										fingerInfos[i].setThisTime = 0;
									} else {
										/* Clear slot */
										fingerInfos[i].slotUsed = 0;
									}
								}
								tempFingerInfo.slotUsed = 0;
							}

							processFingers();

						} else if (2 == ev[i].code) { // MT_Sync : Multitouch event end

							if(!useLegacyProtocol) {

								/* This messsage indicates we use legacy protocol, so switch */
								useLegacyProtocol = 1;
								currentSlot = -1;
								tempFingerInfo.slotUsed = 0;
								if(debugMode) printf("Switch to legacy protocol.\n");
							} else {
								if(tempFingerInfo.slotUsed) {
									/* Finger info for one finger collected in tempFingerInfo, so save it to fingerInfos. */

									/* Look for slot to put the data into by looking at the tracking ids */
									int index = -1;
									int i;
									for(i = 0; i < 2; i++) {
										if(fingerInfos[i].slotUsed && fingerInfos[i].id == tempFingerInfo.id) {
											index = i;
											break;
										}
									}
							
									if(index == -1) {
										for(i = 0; i < 2; i++) {
											if(!fingerInfos[i].slotUsed) {
												/* "Empty" slot, so we can add it. */
												index = i;
												fingerInfos[i].id = tempFingerInfo.id;
												fingerInfos[i].slotUsed = 1;
												break;
											}
										}
									}

									if(index != -1) {
										/* Copy temporary data to slot */
										fingerInfos[index].setThisTime = 1;
										fingerInfos[index].rawX = tempFingerInfo.rawX;
										fingerInfos[index].rawY = tempFingerInfo.rawY;
										fingerInfos[index].rawZ = tempFingerInfo.rawZ;
									}
								}
							}
						}

					} else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW
							|| ev[i].code == MSC_SCAN)) {
					} else if (ev[i].code == 47) {
						currentSlot = ev[i].value;
						if(currentSlot < 0 || currentSlot > 1) currentSlot = -1;
					} else {
						/* Set finger info values for current finger */
						if (ev[i].code == 57) {
							/* ABS_MT_TRACKING_ID */
							if(currentSlot != -1) {
								if(ev[i].value == -1)
								{
									fingerInfos[currentSlot].slotUsed = 0;
								}
								else
								{
									fingerInfos[currentSlot].id = ev[i].value;
									fingerInfos[currentSlot].slotUsed = 1;
								}
							} else if(useLegacyProtocol) {
								tempFingerInfo.id = ev[i].value;
								tempFingerInfo.slotUsed = 1;
							}
						};
						if (ev[i].code == 53) {
							if(currentSlot != -1) {
								fingerInfos[currentSlot].rawX = ev[i].value;
							} else if(useLegacyProtocol) {
								tempFingerInfo.rawX = ev[i].value;
							}
						};
						if (ev[i].code == 54) {
							if(currentSlot != -1) {
								fingerInfos[currentSlot].rawY = ev[i].value;
							} else if(useLegacyProtocol) {
								tempFingerInfo.rawY = ev[i].value;
							}
						};
						if (ev[i].code == 58) {
							if(currentSlot != -1) {
								fingerInfos[currentSlot].rawZ = ev[i].value;
							} else if(useLegacyProtocol) {
								tempFingerInfo.rawZ = ev[i].value;
							}
						};
					}
				}

			}

			if(FD_ISSET(eventQueueDesc, &fileDescSet)) {
				handleXEvent();
			}
		}

		/* Stream stopped, probably because module has been unloaded */
		close(fileDesc);

		/* Clean up */
		releaseButton();
		ungrab(display, deviceID);

		/* Wait until device file is there again */
		while ((fileDesc = open(devname, O_RDONLY)) < 0) {
			sleep(1);
		}

	}

}

