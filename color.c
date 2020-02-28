/*
 *  color.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Color management routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include "color.h"
#include "status.h"
#include "config.h"

/* default text attributes */
short color, background;
chtype attribute;
short pair;  /* actual color pair */

int avals[] = {A_NORMAL, A_UNDERLINE, A_REVERSE, A_BLINK, A_DIM, A_BOLD};

extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

/* color-pairs generation */
void init_pairs(void)
{
  short a, b;
  short c = 0;

  for (a = 0; a < 8; a++)
  {
    for (b = 0; b < 8; b++)
    {
      c++;
      init_pair(c, a, b);
    }
  }
  /* color pairs suck */
}

/* finds pair number for selected colour and background */
short find_pair(short col, short bgnd)
{
  int a;
  short pcol, pbgnd;

  for (a = 1; a < 64; a++)
  {
    pair_content(a, &pcol, &pbgnd);
    if ((pcol == col) && (pbgnd == bgnd)) break;
  }
  return a;
}

/* gets the start-up values from the conf structure */
void color_load_defaults(void)
{
  color = conf.defcolor;
  background = conf.defbgnd;
  attribute = 0 | conf.defattr;
}
