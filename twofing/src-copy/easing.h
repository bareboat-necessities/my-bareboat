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

#ifndef EASING_H_
#define EASING_H_


/* Maximum amount of milliseconds between two scrolling steps before easing stops. */
#define MAX_EASING_INTERVAL 200
/* Maximum amount of milliseconds between the last two scrolling steps for easing to start. */
#define MAX_EASING_START_INTERVAL 200


void startEasing(Profile *, int, int, int);
void stopEasing();
int isEasingActive();
TimeVal* getEasingStepTimeVal();
void checkEasingStep();

#endif /* EASING_H_ */
