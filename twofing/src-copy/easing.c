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
#include "twofingemu.h"
#include "easing.h"
#include "gestures.h"
#include <unistd.h>


/* Variables for easing: */
/* 1 if there is currently easing going on */
int easingActive = 0;

/* Start Interval between two easing steps */
int easingInterval = 0;
/* 1 if easing shall move to the right, -1 if it shall move to the left. */
int easingDirectionX = 0;
/* 1 if easing shall move down, -1 if it shall move up. */
int easingDirectionY = 0;
/* The profile for the easing */
Profile* easingProfile;

TimeVal easingStepTimeVal;

/* Starts the easing; profile, interval and directions have to be set before. */
void startEasing(Profile * profile, int directionX, int directionY, int interval) {
	easingDirectionX = directionX;
	easingDirectionY = directionY;
	easingProfile = profile;
	easingInterval = interval;
	easingStepTimeVal.tv_sec = interval / 1000;
	easingStepTimeVal.tv_usec = (interval % 1000) * 1000;
	easingActive = 1;

}

/* Stops the easing. */
void stopEasing() {
	if(easingActive) {
		easingActive = 0;
	}
}

void checkEasingStep()
{
	if(easingActive && easingStepTimeVal.tv_sec <= 0 && easingStepTimeVal.tv_usec <= 0)
	{
		
		if(inDebugMode()) printf("Easing step\n");
		if (easingProfile->scrollInherit) {
			if(easingDirectionY == -1) {
				executeAction(&(getDefaultProfile()->scrollUpAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionY == 1) {
				executeAction(&(getDefaultProfile()->scrollDownAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionX == -1) {
				executeAction(&(getDefaultProfile()->scrollLeftAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionX == 1) {
				executeAction(&(getDefaultProfile()->scrollRightAction),
					EXECUTEACTION_BOTH);
			}
		} else {
			if(easingDirectionY == -1) {
				executeAction(&(easingProfile->scrollUpAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionY == 1) {
				executeAction(&(easingProfile->scrollDownAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionX == -1) {
				executeAction(&(easingProfile->scrollLeftAction),
					EXECUTEACTION_BOTH);
			}
			if(easingDirectionX == 1) {
				executeAction(&(easingProfile->scrollRightAction),
					EXECUTEACTION_BOTH);
			}
		}


		easingInterval = (int) (((float) easingInterval) * 1.15);

		easingStepTimeVal.tv_sec = easingInterval / 1000;
		easingStepTimeVal.tv_usec = (easingInterval % 1000) * 1000;

		if(easingInterval > MAX_EASING_INTERVAL) {
			easingActive = 0;
		}

	 }
}

TimeVal* getEasingStepTimeVal()
{
	if(easingActive) {
		return &easingStepTimeVal;
	} else {
		return NULL;
	}

}

int isEasingActive()
{
	return easingActive;
}
