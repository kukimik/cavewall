/*
 *  color.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Color management routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef COLOR_H
#define COLOR_H

void init_pairs(void);
short find_pair(short col, short bgnd);
void rotate(short *var, int max);
void color_load_defaults(void);

#endif  /* COLOR_H */
