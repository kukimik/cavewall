/*
 *  block.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Block operations and undo - header file
 *
 *  Copyright (C) 2004 Kuki <kukimik@poczta.onet.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef BLOCK_H
#define BLOCK_H

extern int avals[];
extern short pair;
extern chtype attribute;

void block_windows_remove(void);
void save_undo(int y0, int x0, int yend, int xend);
void do_undo(void);

void block(void);
void block_boxdraw(void);
void block_colorize(void);
void block_copy(void);
void block_fill(void);
void block_clear(void);
void block_mirrorH(void);
void block_mirrorV(void);
void paste(void);
void block_rotate(void);
void block_remove(void);

#endif  /* BLOCK_H */
