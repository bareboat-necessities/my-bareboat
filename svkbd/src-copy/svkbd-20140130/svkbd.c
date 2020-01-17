/* See LICENSE file for copyright and license details.
 *
 * To understand svkbd, start reading main().
 */
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/XTest.h>

/* macros */
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define LENGTH(x)       (sizeof x / sizeof x[0])

/* enums */
enum { ColFG, ColBG, ColLast };
enum { NetWMWindowType, NetLast };

/* typedefs */
typedef unsigned int uint;
typedef unsigned long ulong;

typedef struct {
	ulong norm[ColLast];
	ulong press[ColLast];
	ulong high[ColLast];

	Drawable drawable;
	GC gc;
	struct {
		int ascent;
		int descent;
		int height;
		XFontSet set;
		XFontStruct *xfont;
	} font;
} DC; /* draw context */

typedef struct {
	char *label;
	KeySym keysym;
	uint width;
	int x, y, w, h;
	Bool pressed;
	Bool highlighted;
} Key;

typedef struct {
	KeySym mod;
	uint button;
} Buttonmod;

/* function declarations */
static void motionnotify(XEvent *e);
static void buttonpress(XEvent *e);
static void buttonrelease(XEvent *e);
static void cleanup(void);
static void configurenotify(XEvent *e);
static void countrows();
static void die(const char *errstr, ...);
static void drawkeyboard(void);
static void drawkey(Key *k);
static void expose(XEvent *e);
static Key *findkey(int x, int y);
static ulong getcolor(const char *colstr);
static void initfont(const char *fontstr);
static void leavenotify(XEvent *e);
static void press(Key *k, KeySym mod);
static void run(void);
static void setup(void);
static int textnw(const char *text, uint len);
static void unpress(Key *k, KeySym mod);
static void updatekeys();

/* variables */
static int screen;
static void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ButtonRelease] = buttonrelease,
	[ConfigureNotify] = configurenotify,
	[Expose] = expose,
	[LeaveNotify] = leavenotify,
	[MotionNotify] = motionnotify
};
static Atom netatom[NetLast];
static Display *dpy;
static DC dc;
static Window root, win;
static Bool running = True, isdock = False;
static KeySym pressedmod = 0;
static int rows = 0, ww = 0, wh = 0, wx = 0, wy = 0;
static char *name = "svkbd";

Bool ispressing = False;

/* configuration, allows nested code to access above variables */
#include "config.h"
#include "layout.h"

void
motionnotify(XEvent *e)
{
	XPointerMovedEvent *ev = &e->xmotion;
	int i;

	for(i = 0; i < LENGTH(keys); i++) {
		if(keys[i].keysym && ev->x > keys[i].x
				&& ev->x < keys[i].x + keys[i].w
				&& ev->y > keys[i].y
				&& ev->y < keys[i].y + keys[i].h) {
			if(keys[i].highlighted != True) {
				if(ispressing) {
					keys[i].pressed = True;
				} else {
					keys[i].highlighted = True;
				}
				drawkey(&keys[i]);
			}
			continue;
		}

		if(!IsModifierKey(keys[i].keysym) && keys[i].pressed == True) {
			unpress(&keys[i], 0);

			drawkey(&keys[i]);
		}
		if(keys[i].highlighted == True) {
			keys[i].highlighted = False;
			drawkey(&keys[i]);
		}
	}
}

void
buttonpress(XEvent *e) {
	int i;
	XButtonPressedEvent *ev = &e->xbutton;
	Key *k;
	KeySym mod = 0;

	ispressing = True;

	for(i = 0; i < LENGTH(buttonmods); i++) {
		if(ev->button == buttonmods[i].button) {
			mod = buttonmods[i].mod;
			break;
		}
	}
	if((k = findkey(ev->x, ev->y)))
		press(k, mod);
}

void
buttonrelease(XEvent *e) {
	int i;
	XButtonPressedEvent *ev = &e->xbutton;
	Key *k;
	KeySym mod = 0;

	ispressing = False;

	for(i = 0; i < LENGTH(buttonmods); i++) {
		if(ev->button == buttonmods[i].button) {
			mod = buttonmods[i].mod;
			break;
		}
	}

	if(ev->x < 0 || ev->y < 0) {
		unpress(NULL, mod);
	} else {
		if((k = findkey(ev->x, ev->y)))
			unpress(k, mod);
	}
}

void
cleanup(void) {
	if(dc.font.set)
		XFreeFontSet(dpy, dc.font.set);
	else
		XFreeFont(dpy, dc.font.xfont);
	XFreePixmap(dpy, dc.drawable);
	XFreeGC(dpy, dc.gc);
	XDestroyWindow(dpy, win);
	XSync(dpy, False);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
}

void
configurenotify(XEvent *e) {
	XConfigureEvent *ev = &e->xconfigure;

	if(ev->window == win && (ev->width != ww || ev->height != wh)) {
		ww = ev->width;
		wh = ev->height;
		XFreePixmap(dpy, dc.drawable);
		dc.drawable = XCreatePixmap(dpy, root, ww, wh,
				DefaultDepth(dpy, screen));
		updatekeys();
	}
}

void
countrows() {
	int i = 0;

	for(i = 0, rows = 1; i < LENGTH(keys); i++) {
		if(keys[i].keysym == 0)
			rows++;
	}
}

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
drawkeyboard(void) {
	int i;

	for(i = 0; i < LENGTH(keys); i++) {
		if(keys[i].keysym != 0)
			drawkey(&keys[i]);
	}
	XSync(dpy, False);
}

void
drawkey(Key *k) {
	int x, y, h, len;
	XRectangle r = { k->x, k->y, k->w, k->h};
	const char *l;
	ulong *col;

	if(k->pressed)
		col = dc.press;
	else if(k->highlighted)
		col = dc.high;
	else
		col = dc.norm;

	XSetForeground(dpy, dc.gc, col[ColBG]);
	XFillRectangles(dpy, dc.drawable, dc.gc, &r, 1);
	XSetForeground(dpy, dc.gc, dc.norm[ColFG]);
	r.height -= 1;
	r.width -= 1;
	XDrawRectangles(dpy, dc.drawable, dc.gc, &r, 1);
	XSetForeground(dpy, dc.gc, col[ColFG]);
	if(k->label) {
		l = k->label;
	} else {
		l = XKeysymToString(k->keysym);
	}
	len = strlen(l);
	h = dc.font.ascent + dc.font.descent;
	y = k->y + (k->h / 2) - (h / 2) + dc.font.ascent;
	x = k->x + (k->w / 2) - (textnw(l, len) / 2);
	if(dc.font.set) {
		XmbDrawString(dpy, dc.drawable, dc.font.set, dc.gc, x, y, l,
				len);
	} else {
		XDrawString(dpy, dc.drawable, dc.gc, x, y, l, len);
	}
	XCopyArea(dpy, dc.drawable, win, dc.gc, k->x, k->y, k->w, k->h,
			k->x, k->y);
}

void
expose(XEvent *e) {
	XExposeEvent *ev = &e->xexpose;

	if(ev->count == 0 && (ev->window == win))
		drawkeyboard();
}

Key *
findkey(int x, int y) {
	int i;

	for(i = 0; i < LENGTH(keys); i++) {
		if(keys[i].keysym && x > keys[i].x &&
				x < keys[i].x + keys[i].w &&
				y > keys[i].y && y < keys[i].y + keys[i].h) {
			return &keys[i];
		}
	}
	return NULL;
}

ulong
getcolor(const char *colstr) {
	Colormap cmap = DefaultColormap(dpy, screen);
	XColor color;

	if(!XAllocNamedColor(dpy, cmap, colstr, &color, &color))
		die("error, cannot allocate color '%s'\n", colstr);
	return color.pixel;
}

void
initfont(const char *fontstr) {
	char *def, **missing;
	int i, n;

	missing = NULL;
	if(dc.font.set)
		XFreeFontSet(dpy, dc.font.set);
	dc.font.set = XCreateFontSet(dpy, fontstr, &missing, &n, &def);
	if(missing) {
		while(n--)
			fprintf(stderr, "svkbd: missing fontset: %s\n", missing[n]);
		XFreeStringList(missing);
	}
	if(dc.font.set) {
		XFontStruct **xfonts;
		char **font_names;
		dc.font.ascent = dc.font.descent = 0;
		n = XFontsOfFontSet(dc.font.set, &xfonts, &font_names);
		for(i = 0, dc.font.ascent = 0, dc.font.descent = 0; i < n; i++) {
			dc.font.ascent = MAX(dc.font.ascent, (*xfonts)->ascent);
			dc.font.descent = MAX(dc.font.descent,(*xfonts)->descent);
			xfonts++;
		}
	} else {
		if(dc.font.xfont)
			XFreeFont(dpy, dc.font.xfont);
		dc.font.xfont = NULL;
		if(!(dc.font.xfont = XLoadQueryFont(dpy, fontstr))
		&& !(dc.font.xfont = XLoadQueryFont(dpy, "fixed")))
			die("error, cannot load font: '%s'\n", fontstr);
		dc.font.ascent = dc.font.xfont->ascent;
		dc.font.descent = dc.font.xfont->descent;
	}
	dc.font.height = dc.font.ascent + dc.font.descent;
}

void
leavenotify(XEvent *e) {
	unpress(NULL, 0);
}

void
press(Key *k, KeySym mod) {
	int i;
	k->pressed = !k->pressed;

	if(!IsModifierKey(k->keysym)) {
		for(i = 0; i < LENGTH(keys); i++) {
			if(keys[i].pressed && IsModifierKey(keys[i].keysym)) {
				XTestFakeKeyEvent(dpy,
					XKeysymToKeycode(dpy, keys[i].keysym),
					True, 0);
			}
		}
		pressedmod = mod;
		if(pressedmod) {
			XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, mod),
					True, 0);
		}
		XTestFakeKeyEvent(dpy, XKeysymToKeycode(dpy, k->keysym), True, 0);

		for(i = 0; i < LENGTH(keys); i++) {
			if(keys[i].pressed && IsModifierKey(keys[i].keysym)) {
				XTestFakeKeyEvent(dpy,
					XKeysymToKeycode(dpy, keys[i].keysym),
					False, 0);
			}
		}
	}
	drawkey(k);
}

void
unpress(Key *k, KeySym mod) {
	int i;

	if(k != NULL) {
		switch(k->keysym) {
		case XK_Cancel:
			exit(0);
		default:
			break;
		}
	}

	for(i = 0; i < LENGTH(keys); i++) {
		if(keys[i].pressed && !IsModifierKey(keys[i].keysym)) {
			XTestFakeKeyEvent(dpy,
				XKeysymToKeycode(dpy, keys[i].keysym),
				False, 0);
			keys[i].pressed = 0;
			drawkey(&keys[i]);
			break;
		}
	}
	if(i != LENGTH(keys)) {
		if(pressedmod) {
			XTestFakeKeyEvent(dpy,
				XKeysymToKeycode(dpy, pressedmod),
				False, 0);
		}
		pressedmod = 0;

		for(i = 0; i < LENGTH(keys); i++) {
			if(keys[i].pressed) {
				XTestFakeKeyEvent(dpy,
					XKeysymToKeycode(dpy,
						keys[i].keysym), False, 0);
				keys[i].pressed = 0;
				drawkey(&keys[i]);
			}
		}
	}
}

void
run(void) {
	XEvent ev;

	/* main event loop */
	XSync(dpy, False);
	while(running) {
		XNextEvent(dpy, &ev);
		if(handler[ev.type])
			(handler[ev.type])(&ev); /* call handler */
	}
}

void
setup(void) {
	XSetWindowAttributes wa;
	XTextProperty str;
	XSizeHints *sizeh = NULL;
	XClassHint *ch;
	Atom atype = -1;
	int i, sh, sw;
	XWMHints *wmh;

	/* init screen */
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);
	initfont(font);

	/* init atoms */
	if(isdock) {
		netatom[NetWMWindowType] = XInternAtom(dpy,
				"_NET_WM_WINDOW_TYPE", False);
		atype = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
	}

	/* init appearance */
	countrows();
	if(!ww)
		ww = sw;
	if(!wh)
		wh = sh * rows / 32;

	if(!wx)
		wx = 0;
	if(wx < 0)
		wx = sw + wx - ww;
	if(!wy)
		wy = sh - wh;
	if(wy < 0)
		wy = sh + wy - wh;

	dc.norm[ColBG] = getcolor(normbgcolor);
	dc.norm[ColFG] = getcolor(normfgcolor);
	dc.press[ColBG] = getcolor(pressbgcolor);
	dc.press[ColFG] = getcolor(pressfgcolor);
	dc.high[ColBG] = getcolor(highlightbgcolor);
	dc.high[ColFG] = getcolor(highlightfgcolor);
	dc.drawable = XCreatePixmap(dpy, root, ww, wh,
			DefaultDepth(dpy, screen));
	dc.gc = XCreateGC(dpy, root, 0, 0);
	if(!dc.font.set)
		XSetFont(dpy, dc.gc, dc.font.xfont->fid);
	for(i = 0; i < LENGTH(keys); i++)
		keys[i].pressed = 0;

	wa.override_redirect = !wmborder;
	wa.border_pixel = dc.norm[ColFG];
	wa.background_pixel = dc.norm[ColBG];
	win = XCreateWindow(dpy, root, wx, wy, ww, wh, 0,
			    CopyFromParent, CopyFromParent, CopyFromParent,
			    CWOverrideRedirect | CWBorderPixel |
			    CWBackingPixel, &wa);
	XSelectInput(dpy, win, StructureNotifyMask|ButtonReleaseMask|
			ButtonPressMask|ExposureMask|LeaveWindowMask|
			PointerMotionMask);

	wmh = XAllocWMHints();
	wmh->input = False;
	wmh->flags = InputHint;
	if(!isdock) {
		sizeh = XAllocSizeHints();
		sizeh->flags = PMaxSize | PMinSize;
		sizeh->min_width = sizeh->max_width = ww;
		sizeh->min_height = sizeh->max_height = wh;
	}
	XStringListToTextProperty(&name, 1, &str);
	ch = XAllocClassHint();
	ch->res_class = name;
	ch->res_name = name;

	XSetWMProperties(dpy, win, &str, &str, NULL, 0, sizeh, wmh,
			ch);

	XFree(ch);
	XFree(wmh);
	XFree(str.value);
	if(sizeh != NULL)
		XFree(sizeh);

	if(isdock) {
		XChangeProperty(dpy, win, netatom[NetWMWindowType], XA_ATOM,
				32, PropModeReplace,
				(unsigned char *)&atype, 1);
	}

	XMapRaised(dpy, win);
	updatekeys();
	drawkeyboard();
}

int
textnw(const char *text, uint len) {
	XRectangle r;

	if(dc.font.set) {
		XmbTextExtents(dc.font.set, text, len, NULL, &r);
		return r.width;
	}
	return XTextWidth(dc.font.xfont, text, len);
}

void
updatekeys() {
	int i, j;
	int x = 0, y = 0, h, base, r = rows;

	h = (wh - 1) / rows;
	for(i = 0; i < LENGTH(keys); i++, r--) {
		for(j = i, base = 0; j < LENGTH(keys) && keys[j].keysym != 0; j++)
			base += keys[j].width;
		for(x = 0; i < LENGTH(keys) && keys[i].keysym != 0; i++) {
			keys[i].x = x;
			keys[i].y = y;
			keys[i].w = keys[i].width * (ww - 1) / base;
			keys[i].h = r == 1 ? wh - y - 1 : h;
			x += keys[i].w;
		}
		if(base != 0)
			keys[i - 1].w = ww - 1 - keys[i - 1].x;
		y += h;
	}
}

void
usage(char *argv0) {
	fprintf(stderr, "usage: %s [-hdv] [-g geometry]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[]) {
	int i, xr, yr, bitm;
	unsigned int wr, hr;

	for (i = 1; argv[i]; i++) {
		if(!strcmp(argv[i], "-v")) {
			die("svkbd-"VERSION", © 2006-2010 svkbd engineers,"
				       " see LICENSE for details\n");
		} else if(!strcmp(argv[i], "-d")) {
			isdock = True;
			continue;
		} else if(!strncmp(argv[i], "-g", 2)) {
			if(i >= argc - 1)
				continue;

			bitm = XParseGeometry(argv[i+1], &xr, &yr, &wr, &hr);
			if(bitm & XValue)
				wx = xr;
			if(bitm & YValue)
				wy = yr;
			if(bitm & WidthValue)
				ww = (int)wr;
			if(bitm & HeightValue)
				wh = (int)hr;
			if(bitm & XNegative && wx == 0)
				wx = -1;
			if(bitm & YNegative && wy == 0)
				wy = -1;
			i++;
		} else if(!strcmp(argv[i], "-h")) {
			usage(argv[0]);
		}
	}

	if(!setlocale(LC_CTYPE, "") || !XSupportsLocale())
		fprintf(stderr, "warning: no locale support\n");
	if(!(dpy = XOpenDisplay(0)))
		die("svkbd: cannot open display\n");
	setup();
	run();
	cleanup();
	XCloseDisplay(dpy);
	return 0;
}

