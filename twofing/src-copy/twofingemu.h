/*
 Copyright (C) 2010, 2011, 2012 Philipp Merkel <linux@philmerk.de>

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

#ifndef TWOFINGEMU_H_
#define TWOFINGEMU_H_

#define MAX(a, b) (a > b ? a : b) 

#define VERSION "0.1.2.20120708"

#define BLOCKING_INTERVAL_MS_DEFAULT 500

typedef struct FingerInfo FingerInfo;
typedef struct Action Action;
typedef struct Profile Profile;

struct FingerInfo {
	int x;
	int y;
	int rawX;
	int rawY;
	int rawZ;
	int slotUsed;
	int setThisTime; /* For legacy protocol */
	int id; /* Tracking ID */
};

struct Action {
	int actionType;
	int keyButton;
	int modifier;
};

void startEasingThread();
void stopEasingThread();

#define MODIFIER_SHIFT 1
#define MODIFIER_CONTROL 2
#define MODIFIER_ALT 4
#define MODIFIER_SUPER 8

struct Profile {
	char* windowClass;

	int scrollInherit;
	Action scrollDownAction;
	Action scrollUpAction;
	Action scrollLeftAction;
	Action scrollRightAction;
	Action scrollBraceAction;
	int hscrollStep;
	int vscrollStep;
	int scrollMinDistance;
	int scrollEasing;

	int zoomInherit;
	Action zoomInAction;
	Action zoomOutAction;
	double zoomStep;
	int zoomMinDistance;
	double zoomMinFactor;

	int rotateInherit;
	Action rotateLeftAction;
	Action rotateRightAction;
	double rotateStep;
	int rotateMinDistance;
	double rotateMinAngle;

	int tapInherit;
	Action tapAction;
};

#define ACTIONTYPE_NONE 0
#define ACTIONTYPE_KEYPRESS 1
#define ACTIONTYPE_BUTTONPRESS 2

#define EXECUTEACTION_PRESS 1
#define EXECUTEACTION_RELEASE 2
#define EXECUTEACTION_BOTH 3

#define GESTURE_NONE 0
#define GESTURE_UNDECIDED 1
#define GESTURE_SCROLL 2
#define GESTURE_ZOOM 3
#define GESTURE_ROTATE 4

int inDebugMode();

void *xLoopThreadFunction(void *arg);
int isWindowBlacklisted(Window w);

Window getCurrentWindow();
char* getWindowClass(Window);
Window getLastChildWindow(Window);

Window getActiveWindow();

void processFingers();

void pressButton();
void releaseButton();
int isButtonDown();

void movePointer(int, int, int);
void executeAction(Action* action, int what);

void ungrab(Display *display,int deviceid);
void grab(Display *display,int deviceid);

int invalidWindowHandler(Display *dsp,XErrorEvent *err);

void readCalibrationData();
void startContinuation();

typedef struct timeval TimeVal;

TimeVal getCurrentTime();

int timeDiff(TimeVal start, TimeVal end);

#endif /* TWOFINGEMU_H_ */
