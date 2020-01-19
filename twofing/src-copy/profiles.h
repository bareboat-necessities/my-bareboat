/*
 Copyright (C) 2010, Philipp Merkel <linux@philmerk.de>

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

#include "twofingemu.h"
#include <X11/keysym.h>

#ifndef PROFILES_H_
#define PROFILES_H_

/* Blacklist */
char* blacklist[] = { "chrome", "xournal", "gimp-2.6", "gimp", "mypaint", "inkscape", NULL };

/* WM-Blacklist */
char* wmBlacklist[] = { "kwin", "mutter", "metacity", "compiz", "unity", NULL };

/* Profiles */
int profileCount = 8;
Profile profiles[] = {
				{ 	.windowClass = "evince",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 1000,
					.vscrollStep = 1000,
					.scrollBraceAction = { ACTIONTYPE_BUTTONPRESS, 2, 0	},
					.scrollUpAction = { ACTIONTYPE_NONE,0,0 },
					.scrollDownAction = { ACTIONTYPE_NONE,0,0 },
					.scrollLeftAction = { ACTIONTYPE_NONE,0,0 },
					.scrollRightAction = { ACTIONTYPE_NONE,0,0 },
					.scrollEasing = 0,
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, MODIFIER_CONTROL },
					.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, MODIFIER_CONTROL },
					.zoomStep = 1.2,
					.zoomMinFactor = 1.3,
					.rotateInherit = 0,
					.rotateMinDistance = 100,
					.rotateMinAngle = 30,
					.rotateStep = 70,
					.rotateLeftAction = { ACTIONTYPE_KEYPRESS, XK_Left, MODIFIER_CONTROL },
					.rotateRightAction = { ACTIONTYPE_KEYPRESS, XK_Right, MODIFIER_CONTROL },
					.tapInherit = 1
				},
				{ 	.windowClass = "eog",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 1000,
					.vscrollStep = 1000,
					.scrollBraceAction = { ACTIONTYPE_BUTTONPRESS, 2, 0	},
					.scrollUpAction = { ACTIONTYPE_NONE,0,0 },
					.scrollDownAction = { ACTIONTYPE_NONE,0,0 },
					.scrollLeftAction = { ACTIONTYPE_NONE,0,0 },
					.scrollRightAction = { ACTIONTYPE_NONE,0,0 },
					.scrollEasing = 0,
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
					.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
					.zoomStep = 1.05,
					.zoomMinFactor = 1.1,
					.rotateInherit = 0,
					.rotateMinDistance = 100,
					.rotateMinAngle = 30,
					.rotateStep = 70,
					.rotateLeftAction = { ACTIONTYPE_KEYPRESS, XK_R, MODIFIER_CONTROL | MODIFIER_SHIFT },
					.rotateRightAction = { ACTIONTYPE_KEYPRESS, XK_R, MODIFIER_CONTROL },
					.tapInherit = 1
					//,
					//.tapAction = {ACTIONTYPE_NONE,0,0 }
				},
				{ 	.windowClass = "f-spot",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 50,
					.vscrollStep = 100,
					.scrollBraceAction = { ACTIONTYPE_NONE, 0, 0 },
					.scrollUpAction = { ACTIONTYPE_BUTTONPRESS, 5, MODIFIER_SHIFT },
					.scrollDownAction = { ACTIONTYPE_BUTTONPRESS, 4, MODIFIER_SHIFT },
					.scrollLeftAction = { ACTIONTYPE_BUTTONPRESS, 7, MODIFIER_SHIFT },
					.scrollRightAction = { ACTIONTYPE_BUTTONPRESS, 6, MODIFIER_SHIFT },
					.scrollEasing = 1,
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
					.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
					.zoomStep = 1.1,
					.zoomMinFactor = 1.3,
					.rotateInherit = 0,
					.rotateMinDistance = 100,
					.rotateMinAngle = 30,
					.rotateStep = 70,
					.rotateLeftAction = { ACTIONTYPE_KEYPRESS, XK_bracketleft, 0 },
					.rotateRightAction = { ACTIONTYPE_KEYPRESS, XK_bracketright, 0 },
					.tapInherit = 1
				},
				{ 	.windowClass = "netbook-launcher",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 300,
					.vscrollStep = 125,
					.scrollBraceAction = { ACTIONTYPE_NONE, 0, 0 },
					.scrollUpAction = { ACTIONTYPE_BUTTONPRESS,5,0 },
					.scrollDownAction = { ACTIONTYPE_BUTTONPRESS,4,0 },
					.scrollLeftAction = { ACTIONTYPE_KEYPRESS,XK_Right, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollRightAction = { ACTIONTYPE_KEYPRESS,XK_Left, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollEasing = 0,
					.zoomInherit = 1,
					.rotateInherit = 1,
					.tapInherit = 1
				},
				{ 	.windowClass = "desktop_window",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 300,
					.vscrollStep = 125,
					.scrollBraceAction = { ACTIONTYPE_NONE, 0, 0 },
					.scrollUpAction = { ACTIONTYPE_KEYPRESS,XK_Down, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollDownAction = { ACTIONTYPE_KEYPRESS,XK_Up, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollLeftAction = { ACTIONTYPE_KEYPRESS,XK_Right, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollRightAction = { ACTIONTYPE_KEYPRESS,XK_Left, MODIFIER_CONTROL | MODIFIER_ALT },
					.scrollEasing = 0,
					.zoomInherit = 1,
					.rotateInherit = 1,
					.tapInherit = 1
				},
				{ 	.windowClass = "acroread",
					.scrollInherit = 0,
					.scrollMinDistance = 50,
					.hscrollStep = 20,
					.vscrollStep = 20,
					.scrollBraceAction = { ACTIONTYPE_NONE,0,0 },
					.scrollUpAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
					.scrollDownAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
					.scrollLeftAction = { ACTIONTYPE_BUTTONPRESS, 5, MODIFIER_SHIFT },
					.scrollRightAction = { ACTIONTYPE_BUTTONPRESS, 4, MODIFIER_SHIFT },
					.scrollEasing = 1,
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_KEYPRESS, XK_equal, MODIFIER_CONTROL },
					.zoomOutAction = { ACTIONTYPE_KEYPRESS, XK_minus, MODIFIER_CONTROL },
					.zoomStep = 1.5,
					.zoomMinFactor = 1.5,
					.rotateInherit = 0,
					.rotateMinDistance = 100,
					.rotateMinAngle = 30,
					.rotateStep = 70,
					.rotateLeftAction = { ACTIONTYPE_KEYPRESS, XK_Left, MODIFIER_CONTROL },
					.rotateRightAction = { ACTIONTYPE_KEYPRESS, XK_Right, MODIFIER_CONTROL },
					.tapInherit = 1
				},
			 	{	.windowClass = "SimCity 4.exe",
					.scrollInherit = 0,
					.scrollMinDistance = 20,
					.hscrollStep = 1000,
					.vscrollStep = 1000,
					.scrollBraceAction = { ACTIONTYPE_BUTTONPRESS, 3, 0	},
					.scrollUpAction = { ACTIONTYPE_NONE,0,0 },
					.scrollDownAction = { ACTIONTYPE_NONE,0,0 },
					.scrollLeftAction = { ACTIONTYPE_NONE,0,0 },
					.scrollRightAction = { ACTIONTYPE_NONE,0,0 },
					.scrollEasing = 0,
//					.scrollMinDistance = 20,
//					.hscrollStep = 10,
//					.vscrollStep = 10,
//					.scrollBraceAction = { ACTIONTYPE_NONE,0,0 },
//					.scrollUpAction = { ACTIONTYPE_KEYPRESS, XK_Down, 0 },
//					.scrollDownAction = { ACTIONTYPE_KEYPRESS, XK_Up, 0 },
//					.scrollLeftAction = { ACTIONTYPE_KEYPRESS, XK_Right, 0 },
//					.scrollRightAction = { ACTIONTYPE_KEYPRESS, XK_Left, 0 },
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
					.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
					.zoomStep = 1.5,
					.zoomMinFactor = 1.5,
					.rotateInherit = 1,
					.tapInherit = 1
				},
				{ 	.windowClass = "googleearth-bin",
					.scrollInherit = 0,
					.scrollMinDistance = 100,
					.hscrollStep = 30,
					.vscrollStep = 30,
					.scrollBraceAction = { ACTIONTYPE_KEYPRESS, XK_Shift_L, 0 },
					.scrollUpAction = { ACTIONTYPE_BUTTONPRESS,4,0 },
					.scrollDownAction = { ACTIONTYPE_BUTTONPRESS,5,0 },
					.scrollLeftAction = { ACTIONTYPE_NONE,0,0 },
					.scrollRightAction = { ACTIONTYPE_NONE,0,0 },
					.scrollEasing = 0,
					.zoomInherit = 0,
					.zoomMinDistance = 80,
					.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
					.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
					.zoomStep = 1.2,
					.zoomMinFactor = 1.3,
					.rotateInherit = 0,
					.rotateMinDistance = 100,
					.rotateMinAngle = 10,
					.rotateStep = 15,
					.rotateLeftAction = { ACTIONTYPE_BUTTONPRESS, 5, MODIFIER_CONTROL },
					.rotateRightAction = { ACTIONTYPE_BUTTONPRESS, 4, MODIFIER_CONTROL },
					.tapInherit = 1
					//,
					//.tapAction = {ACTIONTYPE_NONE,0,0 }
				}
			};
/* DON'T FORGET TO CHANGE profileCount WHEN ADDING PROFILES! */


/* Default (fallback) profile */
Profile defaultProfile = {	.windowClass = NULL,
				.scrollInherit = 0,
				.hscrollStep = 40,
				.vscrollStep = 40,
				.scrollMinDistance = 50,
				.scrollBraceAction = { ACTIONTYPE_NONE,0,0 },
				.scrollUpAction = { ACTIONTYPE_BUTTONPRESS, 5, 0 },
				.scrollDownAction = { ACTIONTYPE_BUTTONPRESS, 4, 0 },
				.scrollLeftAction = { ACTIONTYPE_BUTTONPRESS, 7, 0 },
				.scrollRightAction = { ACTIONTYPE_BUTTONPRESS, 6, 0 },
				.scrollEasing = 1,
				.zoomInherit = 0,
				.zoomMinDistance = 80,
				.zoomInAction = { ACTIONTYPE_BUTTONPRESS, 4, MODIFIER_CONTROL },
				.zoomOutAction = { ACTIONTYPE_BUTTONPRESS, 5, MODIFIER_CONTROL },
				.zoomStep = 1.5,
				.zoomMinFactor = 1.5,
				.rotateInherit = 0,
				.rotateMinDistance = 10000,
				.rotateMinAngle = 370,
				.rotateLeftAction = { ACTIONTYPE_NONE,0,0 },
				.rotateRightAction = { ACTIONTYPE_NONE,0,0 },
				.rotateStep = 90,
				.tapInherit = 0,
				.tapAction = { ACTIONTYPE_BUTTONPRESS, 3, 0	}
			  };


#endif /* PROFILES_H_ */
