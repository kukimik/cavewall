/*
 *  banner.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Startup banner
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include "banner.h"
#include "config.h"

#define NUM 10
#define SPACE 7

char art[NUM][100] =
{
  "                                                        ____     _        ",
  "  __                 _      _                          / ___|   / |       ",
  " / _|  _        _   | \\    / |  _                     | ||  \\/\\/  |       ",
  "| |_  |_| |  | |_   |  \\/\\/  | |_| |  |        v.0.27 | |___      |       ",
  " \\__| | |  \\/  |_   |___/\\___| | | |_ |_               \\____|/\\___|       ",
  "                                             ()  ^                        ",
  ".-''-.-''-.-''-.-''-.-''-.-''-.-''-.-''-._ /''''\\| ____.-''-.-''-.-''-.   ",
  "'-..-'-..-'-..-'-..-'-..-'-..-'-..-'-..-'-. /\"\"\\ | _..-'-..-'-..-'-..-'   ",
  "                                                                          ",
  "  [ Colored ASCII Visual Editor Without Any Loathsome Limitations ]       "
};

void draw_banner()
{
  int a;

  for (a = 0; a < SPACE; a++) waddstr(editp, "\n");
  for (a = 0; a < NUM; a++)
  {
    waddstr(editp, art[a]);
    waddstr(editp, "\n");
  }
}
