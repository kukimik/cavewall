/*
 *  import.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Image import routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef IMPORT_H
#define IMPORT_H

#define CBUF		3000		/* size of exported line buffer */

FILE *fd;
extern int truexsize, trueysize;

int read_file(char filename[]);
void import_text(void);
void import_at_start(char parameter[]);
bool import(void);

#endif  /* IMPORT_H */
