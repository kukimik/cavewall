/*
 *  menu.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  General routines - header file
 *
 *  Copyright (C) 2006 Kuki <kukimik@poczta.onet.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef MENU_H
#define MENU_H

extern int xsize, ysize;
extern bool exit_program;
void attr_dialog(void);
void color_dialog(void);
void menu_screen(void);
void step_dialog(void);

#endif  /* MENU_H */
