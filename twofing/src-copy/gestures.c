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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <X11/Xlib.h>
#include "profiles.h"
#include "twofingemu.h"
#include "gestures.h"
#include "easing.h"
#include <unistd.h>


/* clickMode: 0 - first finger; 1 - second finger; 2 - center */
int clickMode;


/* Number of milliseconds before a single click is registered, to give the user time to put down
   second finger for two-finger gestures. */
#define CLICK_DELAY 100

/* Continuation mode -- when 1, two finger gesture is continued when one finger is released. */
#define CONTINUATION 1

#if CONTINUATION
	#define TWO_FINGERS_DOWN fingersDown == 2 && hadTwoFingersOn == 0
	#define TWO_FINGERS_ON fingersDown > 0 && hadTwoFingersOn == 1
	#define TWO_FINGERS_UP fingersDown == 0 && hadTwoFingersOn == 1
#else
	#define TWO_FINGERS_DOWN fingersDown == 2 && fingersWereDown < 2
	#define TWO_FINGERS_ON fingersDown == 2
	#define TWO_FINGERS_UP fingersDown < 2 && fingersWereDown == 2
#endif


/* Maximum distance that two fingers have been moved while they were on */
double maxDist;
/* The time when the first finger touched */
TimeVal fingerDownTime;
/* Were there once two fingers on during the current touch phase? */
int hadTwoFingersOn = 0;

/* Profile of current window, if activated */
Profile* currentProfile = NULL;

/* Gesture information */
int amPerformingGesture = 0;
/* Current profile has scrollBraceAction, thus mouse pointer has to be moved during scrolling */
int dragScrolling = 0;

/* position of center between fingers at start of gesture */
int gestureStartCenterX, gestureStartCenterY;
/* distance between two fingers at start of gesture */
double gestureStartDist;
/* angle of two fingers at start of gesture */
double gestureStartAngle;
/* current position of center between fingers */
int currentCenterX, currentCenterY;

#define PI 3.141592654

/* 1 if the last X scroll command was to the right, -1 if it was to the left. */
int lastScrollDirectionX = 0;
/* 1 if the last Y scroll command was down, -1 if it was up. */
int lastScrollDirectionY = 0;
/* Time last scroll command in X/Y action was sent. */
TimeVal lastScrollXTime;
TimeVal lastScrollYTime;
/* Interval between the last two X/Y scroll commands. */
int lastScrollXIntv;
int lastScrollYIntv;
/*  Last values of lastScrollXIntv/lastScrollYIntv. */
int lastLastScrollXIntv;
int lastLastScrollYIntv;



void initGestures(int theClickMode) {
	clickMode = theClickMode;
}

/* All the gesture-related code.
 * Returns 1 if the method should be called again, 0 otherwise.
 */
int checkGesture(FingerInfo* fingerInfos, int fingersDown) {

	/* Calculate difference between two touch points and angle */
	int xdiff = fingerInfos[1].x - fingerInfos[0].x;
	int ydiff = fingerInfos[1].y - fingerInfos[0].y;
	double currentDist = sqrt(xdiff * xdiff + ydiff * ydiff);
	double currentAngle = atan2(ydiff, xdiff) * 180 / PI;

	/* Check distance the fingers (more exactly: the point between them)
	 * has been moved since start of the gesture. */
	int xdist = currentCenterX - gestureStartCenterX;
	int ydist = currentCenterY - gestureStartCenterY;
	double moveDist = sqrt(xdist * xdist + ydist * ydist);
	if (moveDist > maxDist && fingersDown == 2) {
		/* Set maxDist (but only if we are not in continuation) */
		maxDist = moveDist;
	}

	/* Prevent division by zero */
	if(gestureStartDist < 1) gestureStartDist = 0;

	/* We don't know yet what to do, so look if we can decide now (only do this if there
	   are still two fingers down, otherwise we are in continuation and can't decide). */
	if (amPerformingGesture == GESTURE_UNDECIDED && fingersDown == 2) {
		int scrollMinDist = currentProfile->scrollMinDistance;
		if (currentProfile->scrollInherit)
			scrollMinDist = defaultProfile.scrollMinDistance;
		if ((int) moveDist > scrollMinDist) {
			amPerformingGesture = GESTURE_SCROLL;
			if(inDebugMode()) printf("Start scrolling gesture\n");

			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollBraceAction),
						EXECUTEACTION_PRESS);
				if (defaultProfile.scrollBraceAction.actionType
						== ACTIONTYPE_NONE) {
					dragScrolling = 0;
				} else {
					dragScrolling = 1;
				}
			} else {
				executeAction(&(currentProfile->scrollBraceAction),
						EXECUTEACTION_PRESS);
				if (currentProfile->scrollBraceAction.actionType
						== ACTIONTYPE_NONE) {
					dragScrolling = 0;
				} else {
					dragScrolling = 1;
				}
			}
			return 1;
		}

		int zoomMinDist = currentProfile->zoomMinDistance;
		double zoomMinFactor = currentProfile->zoomMinFactor;
		if (currentProfile->zoomInherit) {
			zoomMinDist = defaultProfile.zoomMinDistance;
			zoomMinFactor = defaultProfile.zoomMinFactor;
		}
		if (abs((int) currentDist - gestureStartDist) > zoomMinDist && (currentDist / gestureStartDist > zoomMinFactor || currentDist / gestureStartDist < 1/zoomMinFactor)) {
			amPerformingGesture = GESTURE_ZOOM;
			if(inDebugMode()) printf("Start zoom gesture\n");
			return 1;
		}

		int rotateMinDist = currentProfile->rotateMinDistance;
		double rotateMinAngle = currentProfile->rotateMinAngle;
		if (currentProfile->rotateInherit) {
			rotateMinDist = defaultProfile.rotateMinDistance;
			rotateMinAngle = defaultProfile.rotateMinAngle;
		}
		double rotatedBy = currentAngle - gestureStartAngle;
		if (rotatedBy < -180)
			rotatedBy += 360;
		if (rotatedBy > 180)
			rotatedBy -= 360;
		//printf("Rotated by: %f; min. angle: %f\n", rotatedBy, rotateMinAngle);
		if (abs(rotatedBy) > rotateMinAngle && (int) currentDist
				> rotateMinDist) {
			amPerformingGesture = GESTURE_ROTATE;
			if(inDebugMode()) printf("Start rotation gesture\n");
			return 1;
		}
	}

	/* If we know what gesture to perform, look if there is something to do */
	switch (amPerformingGesture) {
	case GESTURE_SCROLL:
		;
		int hscrolledBy = currentCenterX - gestureStartCenterX;
		int vscrolledBy = currentCenterY - gestureStartCenterY;
		int hscrollStep = currentProfile->hscrollStep;
		int vscrollStep = currentProfile->vscrollStep;
		if (currentProfile->scrollInherit) {
			hscrollStep = defaultProfile.hscrollStep;
			vscrollStep = defaultProfile.vscrollStep;
		}
		if (hscrollStep == 0 || vscrollStep == 0)
			return 0;

		TimeVal currentTime = getCurrentTime();
		if (hscrolledBy > hscrollStep) {
			lastScrollDirectionX = 1;
			lastLastScrollXIntv = lastScrollXIntv;
			lastScrollXIntv = timeDiff(lastScrollXTime, currentTime);
			lastScrollXTime = currentTime;
			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollRightAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->scrollRightAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartCenterX = gestureStartCenterX + hscrollStep;
			return 1;
		} else if (hscrolledBy < -hscrollStep) {
			lastLastScrollXIntv = lastScrollXIntv;
			lastScrollXIntv = timeDiff(lastScrollXTime, currentTime);
			lastScrollXTime = currentTime;
			lastScrollDirectionX = -1;
			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollLeftAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->scrollLeftAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartCenterX = gestureStartCenterX - hscrollStep;
			return 1;
		}
		if (vscrolledBy > vscrollStep) {
			lastLastScrollYIntv = lastScrollYIntv;
			lastScrollYIntv = timeDiff(lastScrollYTime, currentTime);
			lastScrollYTime = currentTime;
			lastScrollDirectionY = 1;
			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollDownAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->scrollDownAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartCenterY = gestureStartCenterY + vscrollStep;
			return 1;
		} else if (vscrolledBy < -vscrollStep) {
			lastLastScrollYIntv = lastScrollYIntv;
			lastScrollYIntv = timeDiff(lastScrollYTime, currentTime);
			lastScrollYTime = currentTime;
			lastScrollDirectionY = -1;
			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollUpAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->scrollUpAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartCenterY = gestureStartCenterY - vscrollStep;
			return 1;
		}

		return 0;
	case GESTURE_ZOOM:
		;
		double zoomedBy = currentDist / gestureStartDist;
		double zoomStep = currentProfile->zoomStep;
		if (currentProfile->zoomInherit)
			zoomStep = defaultProfile.zoomStep;
		if (zoomedBy > zoomStep) {
			if(inDebugMode()) printf("Zoom in step\n");
			if (currentProfile->zoomInherit) {
				executeAction(&(defaultProfile.zoomInAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->zoomInAction),
						EXECUTEACTION_BOTH);
			}
			/* Reset distance */
			gestureStartDist = gestureStartDist * zoomStep;
			return 1;
		} else if (zoomedBy < 1 / zoomStep) {
			if(inDebugMode()) printf("Zoom out step\n");
			if (currentProfile->zoomInherit) {
				executeAction(&(defaultProfile.zoomOutAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->zoomOutAction),
						EXECUTEACTION_BOTH);
			}
			/* Reset distance */
			gestureStartDist = gestureStartDist / zoomStep;
			return 1;
		}
		return 0;
	case GESTURE_ROTATE:
		;
		double rotatedBy = currentAngle - gestureStartAngle;
		if (rotatedBy < -180)
			rotatedBy += 360;
		if (rotatedBy > 180)
			rotatedBy -= 360;
		double rotateStep = currentProfile->rotateStep;
		if (currentProfile->rotateInherit)
			rotateStep = defaultProfile.rotateStep;
		if (rotatedBy > rotateStep) {
			if(inDebugMode()) printf("Rotate right\n");
			if (currentProfile->rotateInherit) {
				executeAction(&(defaultProfile.rotateRightAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->rotateRightAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartAngle = gestureStartAngle + rotateStep;
		} else if (rotatedBy < -rotateStep) {
			if(inDebugMode()) printf("Rotate left\n");
			if (currentProfile->rotateInherit) {
				executeAction(&(defaultProfile.rotateLeftAction),
						EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->rotateLeftAction),
						EXECUTEACTION_BOTH);
			}

			gestureStartAngle = gestureStartAngle - rotateStep;
		}

		return 0;
	}

	return 0;

}

void processFingerGesture(FingerInfo* fingerInfos, int fingersDown, int fingersWereDown, int blockSingleTouches) {

	if(fingersDown != 0 && fingersWereDown == 0) {
		stopEasing();
	}

	TimeVal currentTime = getCurrentTime();
	if (TWO_FINGERS_DOWN) {
		/* Second finger touched (and maybe first too) */

		lastScrollXTime = currentTime;
		lastScrollYTime = currentTime;
		lastScrollXIntv = 0; lastScrollYIntv = 0;		
		lastLastScrollXIntv = 0; lastLastScrollYIntv = 0;		

		maxDist = 0;

		/* Memorize that there were two fingers on during touch */
		hadTwoFingersOn = 1;

		/* Get current profile */
		currentProfile = getWindowProfile(getActiveWindow());
		if(inDebugMode()) {
			if(currentProfile->windowClass != NULL) {
				printf("Use profile '%s'\n", currentProfile->windowClass);
			} else {
				printf("Use default profile.\n");
			}
		}

		/* If there had already been a single-touch event raised because the
		 * user was too slow, stop it now. */
		releaseButton();

		/* Calculate center position and distance between touch points */
		gestureStartCenterX = (fingerInfos[0].x + fingerInfos[1].x) / 2;
		gestureStartCenterY = (fingerInfos[0].y + fingerInfos[1].y) / 2;

		int xdiff = fingerInfos[1].x - fingerInfos[0].x;
		int ydiff = fingerInfos[1].y - fingerInfos[0].y;
		gestureStartDist = sqrt(xdiff * xdiff + ydiff * ydiff);
		gestureStartAngle = atan2(ydiff, xdiff) * 180 / PI;

		/* We have not decided on a gesture yet. */
		amPerformingGesture = GESTURE_UNDECIDED;

		movePointer(gestureStartCenterX, gestureStartCenterY, 0);
	} else if (TWO_FINGERS_ON) {

		/* Moved with two fingers */

		if(fingersDown == 2) {
			/* Calculate new center between fingers */
			currentCenterX = (fingerInfos[0].x + fingerInfos[1].x) / 2;
			currentCenterY = (fingerInfos[0].y + fingerInfos[1].y) / 2;
		} else {
			int i;
			for(i = 0; i <= 1; i++) {
				if(fingerInfos[i].slotUsed) {
					currentCenterX = fingerInfos[i].x;
					currentCenterY = fingerInfos[i].y;
				}
			}
		}

		/* If we are dragScrolling (we are scrolling and there is a brace action,
		 * we need to move the pointer */
		if (amPerformingGesture == GESTURE_SCROLL && dragScrolling) {
			/* Move pointer to center between touch points */
			movePointer(currentCenterX, currentCenterY, 0);
		}

		/* Perform gestures as long as there are some. */
		while (checkGesture(fingerInfos, fingersDown));
	} else if (TWO_FINGERS_UP) {
		/* Second finger (and maybe also first) released */

		if (amPerformingGesture == GESTURE_SCROLL) {
			/* If there was a scroll gesture and we have a brace action, perform release. */
			if (currentProfile->scrollInherit) {
				executeAction(&(defaultProfile.scrollBraceAction),
						EXECUTEACTION_RELEASE);
			} else {
				executeAction(&(currentProfile->scrollBraceAction),
						EXECUTEACTION_RELEASE);
			}
			if(currentProfile->scrollEasing) {
				int intv;
				int dirX = lastScrollDirectionX;
				int dirY = lastScrollDirectionY;

				/* Start easing */
				if(inDebugMode()) printf("Start easing\n");

				/* Compensate for scrolling gestures getting a little bit slower at the end */
				if(lastLastScrollXIntv < lastScrollXIntv && lastLastScrollXIntv != 0) lastScrollXIntv = lastLastScrollXIntv;
				if(lastLastScrollYIntv < lastScrollYIntv && lastLastScrollYIntv != 0) lastScrollYIntv = lastLastScrollYIntv;

				/* Check if scrolling intervals are not too long. Also check if last scrolling on an axis is longer
				   ago than twice its interval, which means the scrolling has been stopped or extremely slowed down since. */
				if(lastScrollYIntv == 0 || timeDiff(lastScrollYTime, currentTime) > lastScrollYIntv * 2 || lastScrollYIntv > MAX_EASING_START_INTERVAL) dirY = 0;
				if(lastScrollXIntv == 0 || timeDiff(lastScrollXTime, currentTime) > lastScrollXIntv * 2 || lastScrollXIntv > MAX_EASING_START_INTERVAL) dirX = 0;
				if(dirX != 0 || dirY != 0) {
					if(dirX != 0 && dirY != 0) {
						/* As we only support one interval, only use larger axis. */
						if(lastScrollXIntv < lastScrollYIntv) {
							dirY = 0;
						} else {
							dirX = 0;
						}
					}

					if(dirY == 0) {
						intv = lastScrollXIntv;
					} else if(dirX == 0) {
						intv = lastScrollYIntv;
					} else {
						/* We will never reach this, but removes warning */
						intv = 100000;
					}
					if(inDebugMode()) printf("Really start easing\n");
					startEasing(currentProfile, dirX, dirY, intv);
				}
			}
		}

		/* If we haven't performed a gesture and haven't moved too far, perform tap action. */
		if ((amPerformingGesture == GESTURE_NONE || amPerformingGesture
				== GESTURE_UNDECIDED) && maxDist < 10) {
			/* Move pointer to correct position */
			if(clickMode == 2) {
				/* Assume first finger is at ID 0 and second finger at ID 1, might have to be changed later */
				movePointer(gestureStartCenterX, gestureStartCenterY, fingerInfos[0].rawZ);
			} else {
				/* Assume first finger is at ID 0 and second finger at ID 1, might have to be changed later */
				movePointer(fingerInfos[clickMode].x, fingerInfos[clickMode].y, fingerInfos[clickMode].rawZ);
			}

			if (currentProfile->tapInherit) {
				executeAction(&(defaultProfile.tapAction), EXECUTEACTION_BOTH);
			} else {
				executeAction(&(currentProfile->tapAction), EXECUTEACTION_BOTH);
			}
		}

		amPerformingGesture = GESTURE_NONE;

	} else if (fingersDown == 1 && fingersWereDown == 0) {
		/* First finger touched */
		fingerDownTime = currentTime;

		if(!blockSingleTouches) {
			/* Fake single-touch move event */
			int i;
			for(i = 0; i <= 1; i++) {
				if(fingerInfos[i].slotUsed) {
					movePointer(fingerInfos[i].x, fingerInfos[i].y, fingerInfos[i].rawZ);
				}
			}
		}
	} else if (fingersDown == 1) {
		/* Moved with one finger */
		if(!blockSingleTouches) {
			if (hadTwoFingersOn == 0 && !isButtonDown()) {
				if (timeDiff(fingerDownTime, currentTime) > CLICK_DELAY) {
					/* Delay has passed, no gesture been performed, so perform single-touch press now */
					pressButton();
				}
			}
			if(isButtonDown()) {
				/* Fake single-touch move event */
				int i;
				for(i = 0; i <= 1; i++) {
					if(fingerInfos[i].slotUsed) {
						movePointer(fingerInfos[i].x, fingerInfos[i].y, fingerInfos[i].rawZ);
					}
				}

			}
		}
	} else if (fingersDown == 0 && fingersWereDown > 0) {
		/* Last finger released */
		if(!blockSingleTouches) {
			if (hadTwoFingersOn == 0 && !isButtonDown()) {
				/* The button press time has not been reached yet, and we never had two
				 * fingers on (we could not have done this in this short time) so
				 * we simulate button down and up now. */
				pressButton();
				releaseButton();
			} else {
				/* We release the button if it is down. */
				releaseButton();
			}
		}
		else
		{
			if(isButtonDown()) {
				releaseButton();
			}
		}
	}
	if (fingersDown == 0) {
		/* Reset fields after release */
		hadTwoFingersOn = 0;
	}
}


Profile* getDefaultProfile() {
	return &defaultProfile;
}

/* Returns a pointer to the profile of the currently selected
 * window, or defaultProfile if there is no specific profile for it or the window is invalid. */
Profile* getWindowProfile(Window w) {
	if (w != None) {

		char* class = getWindowClass(w);

		if(class != NULL) {
			if(inDebugMode()) {
				printf("Current window: '%s'\n", class);
			}

			int i;
			/* Look for the profile with this class */
			for (i = 0; i < profileCount; i++) {
				if (!strncmp(class, profiles[i].windowClass, 30)) {
					free(class);
					/* Return this profile */
					return &profiles[i];
				}
			}

			/* No profile found, return default. */
			free(class);
			return &defaultProfile;
		} else {
			return &defaultProfile;
		}
	} else {
		return &defaultProfile;
	}
}

int isWindowBlacklistedForGestures(Window w) {
	int i;
	char* class = getWindowClass(w);

	if (class != NULL) {
		if(inDebugMode()) printf("Found window with id %i and class '%s' \n", (int) w,
					class);

		for (i = 0; blacklist[i] != NULL; i++) {
			if (strncmp(class, blacklist[i], 30) == 0) {
				free(class);
				return 1;
			}
		}
		
		/* Not blacklisted. Check if is on wmBlacklist */
		for(i = 0; wmBlacklist[i] != NULL; i++) {
			if (strncmp(class, wmBlacklist[i], 30) == 0) {
				free(class);
				if(inDebugMode()) printf("Look for child\n");
				return isWindowBlacklisted(getLastChildWindow(w));
			}
		}

		free(class);
		return 0;
	} else {
		if(inDebugMode()) printf("Found window with id %i and no class.\n", (int) w);
		//if(inDebugMode()) printf("Look for another child\n");
		return isWindowBlacklisted(getLastChildWindow(w));
		return 0;
	}
}
