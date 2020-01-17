SVKBD
=====
This is a simple virtual keyboard, intended to be used in environments,
where no keyboard is available.

Installation
------------

	% make
	% make install

This will create by default `svkbd-en`, which is svkbd using an English
keyboard layout. You can create svkbd for additional layouts by doing:

	% make svkbd-$layout

This will take the file `layout.$layout.h` and create `svkbd-$layout`.
`make install` will then pick up the new file and install it accordingly.

Usage
-----

	% svkbd-en

This will open svkbd at the bottom of the screen, showing the default
English layout.

	% svkbd-en -d

This tells svkbd-en to announce itself being a dock window, which then
is managed differently between different window managers. If using dwm
and the dock patch, then this will make svkbd being managed by dwm and
some space of the screen being reserved for it.

	% svkbd-en -g 400x200+1+1

This will start svkbd-en with a size of 400x200 and at the upper left
window corner.

Repository
----------

	git clone http://git.suckless.org/svkbd

