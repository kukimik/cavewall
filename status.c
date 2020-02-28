/*
 *  status.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Status line routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "status.h"
#include "config.h"
#include "color.h"

/* names */
char colors[NUMCOLORS][8] = {"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
char attrs[NUMATTRS][10] = {"normal", "underline", "reverse", "blink", "dim", "bold"};
char modes[NUMMODES][9] = {"normal", "step_fwd", "step_bwd", "step_up", "step_dn", "step_ul", "step_ur", "step_dl", "step_dr", "step_rnd"};

WINDOW *statusw;
short spair;  /* color pair number for status line */
short keyno;
char lastname[500] = "";

extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

/* adds zero before (one and two)-digit numbers */
void zero(int number, char out[4])
{
  char buf[4] = "";
  char nbuf[4] = "";
  char digit[] = "0";

  if (number!=1000)
  {
  sprintf(nbuf, "%d", number);
  if (number < 10) strcat(buf, digit);
  if (number < 100) strcat(buf, digit);
  strcat(buf, nbuf);
  strncpy(out, buf, 4);
  }
  else
  strncpy(out," 1K\0",4);
}

/* updates information on status line */
void update_status(void)
{
  char x[4], y[4];

  status_bold;
attron(A_BOLD);
  mvwprintw(statusw, 0, 11, "colr:        bgnd:        attr:          mode:         key:");
  if (conf.fromzero == 0)
  {
    zero(actx, x);
    zero(acty, y);
  }
  else
  {
    zero(actx, x);
    zero(acty, y);
  }

  if (blockmode) mvwaddch(statusw, 0, 0, 'B' | COLOR_PAIR(find_pair(COLOR_RED,COLOR_BLUE))); else mvwaddch(statusw, 0, 0, ' ');
  mvwprintw(statusw, 0, 1, "%s,%s", x, y);
  mvwprintw(statusw, 0, 9, "%c", oldchar);
  status_normal;
  mvwprintw(statusw, 0, 16, "%s", colors[color]);
  mvwprintw(statusw, 0, 29, "%s", colors[background]);
  unsigned short i;
  for (i=1; i<NUMATTRS; i++) if (attribute & avals[i]) mvwaddch(statusw, 0, 41+i, attrs[i][0] | avals[i]);
  mvwprintw(statusw, 0, 57, "%s", modes[mode]);
  mvwaddch(statusw, 0, 72, ' ');
  mvwprintw(statusw, 0, 70, "%d", keyno);  
  wbkgdset(statusw, ' ' | COLOR_PAIR(find_pair(color,background)) | attribute);

  mvwprintw(statusw, 0, 74, "%s", "=|#|=");  
  status_normal;
  wrefresh(statusw);
}

/* clears status line */
void clear_status(void)
{
  int c;

  spair = find_pair(COLOR_WHITE, COLOR_BLUE);  // !!! configurable
  wbkgdset(statusw, ' ' | COLOR_PAIR(spair));
  for (c = 0; c < xsize; c++) mvwprintw(statusw, 0, c, " ");
  wrefresh(statusw);
}

/* string input (max can't be > 100) */
void status_input(char fname[], char def[], char text[], int max)
{
  int num = strlen(def);
  int kb, end = 0;
  char buf[100];

  strncpy(buf, def, 100);
  clear_status();
  status_bold;
  mvwprintw(statusw, 0, 1, "%s: ", fname);
  status_normal;
  mvwprintw(statusw, 0, strlen(fname) + 3, "%s", def);
  curs_set(TRUE);
  keypad(statusw, TRUE);
  wrefresh(statusw);
  while (end != 2)
  {
    kb = wgetch(statusw);
    if (kb != ERR)
    {
      end = 0;
      if (kb == 10) end = 2;  /* KEY_ENTER doesn't work, don't ask why */
      if ((kb == KEY_BACKSPACE) && (end == 0))
      {
        if (num > 0)
	{
          mvwaddch(statusw, 0, strlen(fname) + 2 + num, ' ');
	  wmove(statusw, 0, strlen(fname) + 2 + num);
	  num--;
	}
	end = 1;
      }
      if ((num < max) && (end == 0))
      {
        mvwaddch(statusw, 0, strlen(fname) + 3 + num, kb);
	buf[num] = kb;
        num++;
	end = 1;
      }
      wrefresh(statusw);
    }
  }
  buf[num] = '\0';
  strncpy(text, buf, num + 1);
  curs_set(FALSE);
  clear_status();
  update_status();
}

/* displays yes/no/cancel/whatever question */
int status_question(char fname[], char answers[], short def)
{
  int a, b = 0, end = 0;
  char kb;
  char abuf[15] = " ";

  clear_status();
  status_bold;
  mvwprintw(statusw, 0, 1, "%s? ", fname);
  for (a = 0; a < strlen(answers); a++)
  {
    if ((a == 0) || ((a > 0) && (answers[a - 1] == ' ')))
    {
      b++;
      if (b == def)  /* default option in [ ] */
      {
        status_normal;
        waddch(statusw, '[');
	status_bold;
        waddch(statusw, answers[a]);
	status_normal;
        waddch(statusw, ']');
	abuf[b] = tolower(answers[a]);
      }
      else
      {
        status_bold;
        waddch(statusw, answers[a]);
	abuf[b] = tolower(answers[a]);
      }
    }
    else
    {
      status_normal;
      waddch(statusw, answers[a]);
    }
  }
  abuf[b + 1] = '\0';
  keypad(statusw, TRUE);
  wrefresh(statusw);
  while (end == 0)
  {
    kb = wgetch(statusw);
    if (kb != ERR)
    {
      if (kb == 10)
      {
        if (def != 0) end = def; else beep();
      }
      else
      for (a = 1; a <= strlen(abuf); a++)
        if ((end == 0) && (kb == abuf[a])) end = a;
      if (end == 0) beep();
    }
  }
  clear_status();
  update_status();
  return end;
}

/* displays a message on the status line */
void status_message(char message[])
{
  clear_status();
  status_bold;
  mvwprintw(statusw, 0, 1, "%s", message);
  wrefresh(statusw);
  getch();
  clear_status();
  update_status();
}

/* displays a standard error message on the status line */
void status_error(void)
{
  char error[50] = "";

  strcat(error, "Error: ");
  strncat(error, strerror(errno), 30);
  status_message(error);
}
