/*
 *  cave.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  General routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef CAVE_H
#define CAVE_H

extern short pair;
extern short keyno;
extern int avals[];

void putch(int key);
void rmch(void);
void open_rnd(void);
void draw_screen(void);
void shut_screen(void);
void check_key(int key);
int main(int argc, char *argv[]);

#endif  /* CAVE_H */
