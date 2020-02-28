/*
 *  status.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Status line routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef STATUS_H
#define STATUS_H

#define status_normal wbkgdset(statusw, ' ' | COLOR_PAIR(spair) | A_NORMAL)
#define status_bold   wbkgdset(statusw, ' ' | COLOR_PAIR(spair) | A_BOLD)

extern WINDOW *statusw;
extern int actx, acty, xsize;
extern int leftline, topline;
extern short color, background, mode;
extern chtype attribute;
extern chtype oldchar;
extern int avals[];
extern bool blockmode;

void zero(int number, char out[4]);
void update_status(void);
void clear_status(void);
void status_input(char fname[], char def[], char text[], int max);
int status_question(char fname[], char answers[], short def);
void status_message(char message[]);
void status_error(void);

#endif  /* STATUS_H */
