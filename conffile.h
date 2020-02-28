/*
 *  conffile.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Configuration file reading routines - header file
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef CONFFILE_H
#define CONFFILE_H

#include "config.h"

#define LBUF 1000  /* size of config file line buffer */

extern char colors[NUMCOLORS][8];
extern char attrs[NUMATTRS][10];
extern char modes[NUMMODES][9];

void key_pair(char key[], char value[]);
void parse_line(char buf[]);
void dig_file(char filename[]);
void read_config(void);

#endif  /* CONFFILE_H */
