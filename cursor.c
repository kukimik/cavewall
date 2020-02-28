/*
 *  cursor.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Cursor movement routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <stdlib.h>
#include "config.h"
#include "cursor.h"

int actx = 0, acty = 0;

short mode = 0;  /* cursor movement mode */
chtype oldchar;  /* character under the cursor */

extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

#ifdef HIGHLIGHT
/* highlights block selection */
void highlight_block(void)
{
static int xmin,xmax,ymin,ymax;
int x,y;
static bool highlight=false;

    if (!highlight)
    {
	if (xFIN!=-1)
        {
	xmin=xSTART;
        xmax=xFIN;
        ymin=ySTART;
        ymax=yFIN;
	}	
        else
	{
	    if (xSTART>actx)
    	    {
	    xmin=actx;
	    xmax=xSTART;
	    }
	    else
	    {
	    xmin=xSTART;
	    xmax=actx;
	    }
	    if (ySTART>acty)
	    {
	    ymin=acty;
	    ymax=ySTART;
	    }
	    else
	    {
	    ymin=ySTART;
	    ymax=acty;
	    }
	}
	
	if ((xmin<leftline && xmax<leftline) || (ymin<topline && ymax<topline) || (xmin>leftline+xsize) || (ymin>topline+ysize-1))
	return;
	
	if (xmin<leftline)
	xmin=leftline;
	if (ymin<topline)
	ymin=topline;
	if (xmax>leftline+xsize)
	xmax=leftline+xsize;
	if (ymax>topline+ysize-1)
	ymax=topline+ysize-1;
	
	highlight=true;
    }
    else
    highlight=false;
    
    for (x=xmin; x<=xmax; x++)
        for (y=ymin; y<=ymax; y++)
        mvwaddch(editp,y,x,mvwinch(editp,y,x) ^ A_STANDOUT);

return;
}
#endif

/* refreshes edit pad */
void editprefresh(void)
{
touchwin(editp);
#ifdef HIGHLIGHT
if (blockmode)
{
    highlight_block();
    prefresh(editp,topline,leftline,0,0,ysize-2,xsize-1);
    highlight_block();
}
else
#endif
prefresh(editp,topline,leftline,0,0,ysize-2,xsize-1);
}

/* puts the cursor on selected position */
void put_cursor(int y, int x)
{
  acty = y;
  actx = x;
  oldchar = mvwinch(editp, y, x);
  mvwaddch(editp, y, x, oldchar | A_REVERSE);
}

/* hides the cursor (for import/export.c routines) */
void cursor_up(void)
{
  mvwaddch(editp, acty, actx, oldchar);
  wrefresh(editp);
}

/* shows the cursor (for import/export.c routines) */
void cursor_down(void)
{
  put_cursor(acty, actx);
  editprefresh();
}

/* moves the cursor to selected position */
void mv_cursor(int y, int x)
{
  mvwaddch(editp, acty, actx, oldchar);  /* restore non-inverted char */
  put_cursor(y, x);
  editprefresh();
}

/* moves the cursor up */
void move_up(void)
{
  if (acty > 0) mv_cursor(acty - 1, actx);
  if (acty < topline) editp_scroll(-1,0);
}

/* moves the cursor down */
void move_down(void)
{
  if (acty < 999)
  {
  mv_cursor(acty + 1, actx);
    if (acty > topline+ysize-2)  editp_scroll(1,0);
  }
}

/* moves the cursor left */
void move_left(void)
{
  if (actx > 0) mv_cursor(acty, actx - 1);
  if (actx < leftline) editp_scroll(0,-1);
}

/* moves the cursor right */
void move_right(void)
{
  if (actx < 999) 
  {
  mv_cursor(acty, actx + 1);
    if (actx > leftline+xsize-1) editp_scroll(0,1);
  }
}

#ifdef RANDOM

/* generates random number between min and max */
int randomize(int min, int max)
{
  int ret;

  srandom(fgetc(rnd));
  ret = ((random() % (max - min + 1)) + min);
  return ret;	  
}

#endif

/* steps using method selected in 'mode' */
void step(void)
{
  int one = mode;

#ifdef RANDOM
  if (mode == 9) one = randomize(1, 8);  /* the move_rnd mode */
#endif
  switch (one)
  {
    case 1: { move_right(); break; }
    case 2: { move_left(); break; }
    case 3: { move_up(); break; }
    case 4: { move_down(); break; }
    case 5: { move_up(); move_left(); break; }
    case 6: { move_up(); move_right(); break; }
    case 7: { move_down(); move_left(); break; }
    case 8: { move_down(); move_right(); break; }
  }
}

/* steps backwards using method selected in 'mode' */
void step_bwd(void)
{
  int one = mode;

#ifdef RANDOM
  if (mode == 9) one = randomize(1, 8);  /* the move_rnd mode */
#endif
  switch (one)
  {
    case 1: { move_left(); break; }
    case 2: { move_right(); break; }
    case 3: { move_down(); break; }
    case 4: { move_up(); break; }
    case 5: { move_down(); move_right(); break; }
    case 6: { move_down(); move_left(); break; }
    case 7: { move_up(); move_right(); break; }
    case 8: { move_up(); move_left(); break; }
  }
}

/* gets the start-up values from the conf structure */
void cursor_load_defaults(void)
{
  mode = conf.defmode;
}

/* makes editp wider or higher by 1 screen */
void change_editp_size(int down, int right)
{
	if ((right==0) && (down==0))
	return;
	
    truexsize+=right;
    trueysize+=down;

    if (truexsize>1000) truexsize=1000;
    if (trueysize>1000) trueysize=1000;

    if (truexsize<=xsize) truexsize=xsize+1;
    if (trueysize<=ysize-1) trueysize=ysize;

wresize(editp,trueysize,truexsize);

return;
}

/* changes topline and leftline coordinates and checks if we are not oustide the edit pad (if so it calls change_editp_size) */
void editp_scroll(int dlines, int rlines)
{
int resizex=0, resizey=0;

leftline+=rlines;
topline+=dlines;

if (topline<0) topline=0;
if (leftline<0) leftline=0;

if (topline>1000-ysize+1) topline=1000-ysize+1;
if (leftline>1000-xsize) leftline=1000-xsize;

if (topline+ysize-1>=trueysize) resizey=ysize-1;
if (leftline+xsize>=truexsize) resizex=xsize;

change_editp_size(resizey, resizex);

editprefresh();
return;    
}
