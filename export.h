/*
 *  export.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Image export routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef EXPORT_H
#define EXPORT_H

#define A_ATTRONLY	0xffff0000  /* mask for extracting attrs like bold */
#define CBUF		3000        /* size of exported line buffer */

extern int trueysize;
extern int truexsize;

int open_file(char filename[], int mode);
int end(char buf[CBUF]);
void export_ansi(void);
void export_txt(void);
void export_html(void);
void export(void);

#endif  /* EXPORT_H */
