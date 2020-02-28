/*
 *  cursor.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Cursor movement routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef CURSOR_H
#define CURSOR_H

#ifdef RANDOM
extern FILE *rnd;
#endif
extern WINDOW *editp;
extern int xsize, ysize;
extern int truexsize, trueysize;
extern int leftline, topline;

#ifdef HIGHLIGHT
void highlight_block(void);
#endif
void editprefresh(void);
void editp_scroll(int dlines, int rlines);
void change_editp_size(int down, int right);

void put_cursor(int y, int x);
void cursor_up(void);
void cursor_down(void);
void mv_cursor(int y, int x);
void move_up(void);
void move_down(void);
void move_left(void);
void move_right(void);
int randomize(int min, int max);
void step(void);
void step_bwd(void);
void cursor_load_defaults(void);

extern int xSTART, ySTART, xFIN, yFIN;
extern bool blockmode;

#endif  /* CURSOR_H */
