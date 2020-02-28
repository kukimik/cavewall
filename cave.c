/*
 *  cave.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  General routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <stdlib.h>
#include "menu.h"
#include "block.h"
#include "cave.h"
#include "conffile.h"
#include "color.h"
#include "config.h"
#include "cursor.h"
#include "status.h"
#include "import.h"
#include "cursor.h"
#include "export.h"
#ifdef BANNER
#include "banner.h"
#endif

/* window parameters */
int xsize, ysize;

int truexsize, trueysize;
int topline,leftline;

WINDOW *editp;
FILE *rnd;

extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

bool exit_program=false;

/* puts character */
void putch(int key)
{
  char ch = key;  /* to remove all attributes */
  
  save_undo(acty,actx,acty,actx);
  
  mvwaddch(editp, acty, actx, ch | COLOR_PAIR(pair) | attribute);
  put_cursor(acty, actx);  /* because character hides our cursor */
  editprefresh();
}

/* clears edit window */
void clearedit(void)
{
short answer;
answer=status_question("Clear screen (undo not possible)","Yes No",1);
if (answer==1)
{
  save_undo(-1,-1,-1,-1); // remove old undo info
  wclear(editp);
  change_editp_size((-trueysize),(-truexsize));
  put_cursor(0, 0);
  editprefresh();
}
return;
}


/* deletes character */
void rmch(void)
{
  save_undo(acty,actx,acty,actx);

    mvwaddch(editp, acty, actx, ' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR); // !!! Trying to defaultize things
//  mvwaddch(editp, acty, actx, ' ' | COLOR_PAIR(0) | A_NORMAL);

  put_cursor(acty, actx);
  editprefresh();
}

/* clears to end of line */
void clear_to_eol(void)
{
  int i;

  save_undo(acty,actx,trueysize,actx);

    for (i=acty; i<=trueysize; i++)
	mvwaddch(editp, i, actx, ' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR); // !!! Trying to defaultize things
//  mvwaddch(editp, acty, actx, ' ' | COLOR_PAIR(0) | A_NORMAL);

  put_cursor(acty, actx);
  editprefresh();
}

#ifdef RANDOM

/* opens /dev/urandom */
void open_rnd(void)  // !!! dodaæ gdzie¶ close(rnd); (przy wy³±czaniu programu)
{
  rnd = fopen(RNDDEV, "r");

  if (rnd == NULL)
  {
    fprintf(stderr, "Error: Can't open /dev/urandom.");
    exit(EXIT_FAILURE);
  }
}

#endif

/* prepares curses and editor screen */
void draw_screen(void)
{  
  initscr();
  start_color();
  getmaxyx(stdscr, ysize, xsize);
  if ((statusw = newwin(1, xsize, ysize - 1, 0)) == NULL) printf("error creating status window\n");
  truexsize=xsize+1;
  trueysize=ysize;
  if ((editp = newpad(trueysize, truexsize)) == NULL) printf("error creating edit pad\n");
  topline=0;
  leftline=0;
  curs_set(FALSE);  /* we have own cursor */
  noecho();

  
  keypad(stdscr, TRUE);
  put_cursor(acty, actx);
  refresh();
  init_pairs();
  pair = find_pair(color, background);
  wbkgdset(editp,' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR); // !!! Trying to defaultize things
  clear_status();
}

/* curses shutdown */
void shut_screen(void)
{
  delwin(editp);
  delwin(statusw);

  block_windows_remove();
  curs_set(TRUE);
  endwin();
}

/* keyboard interface */
void check_key(int key)
{
  int end = 0;

  keyno = key;
  /* cursor keys */
  if ((end == 0) && (key == conf.key_up)) { move_up(); update_status(); end = 1; }
  if ((end == 0) && (key == conf.key_down)) { move_down(); update_status(); end = 1; }
  if ((end == 0) && (key == conf.key_left)) { move_left(); update_status(); end = 1; }
  if ((end == 0) && (key == conf.key_right)) { move_right(); update_status(); end = 1; }

  /* pgup, pgdn, end, home - special handling */
  if ((end == 0) && (key == conf.key_leftborder)) 
  { 
    if (actx==leftline && actx!=0)
    editp_scroll(0,-xsize);
  mv_cursor(acty, leftline); 
  update_status(); 
  end = 1; 
  }
  
  if ((end == 0) && (key == conf.key_rightborder))
  { 
    if (actx==leftline+xsize-1 && actx!=999)
    editp_scroll(0,xsize);
  mv_cursor(acty, leftline+xsize-1); 
  update_status(); 
  end = 1; 
  }
  
  if ((end == 0) && (key == conf.key_upborder))
  { 
    if (acty==topline && acty!=0)
    editp_scroll(-ysize+1,0);
  mv_cursor(topline, actx); 
  update_status(); 
  end = 1; 
  }

  if ((end == 0) && (key == conf.key_downborder))
  { 
    if (acty==topline+ysize-2 && acty!=999)
    editp_scroll(ysize-1,0);
  mv_cursor(topline+ysize-2, actx); 
  update_status(); 
  end = 1; 
  }

  /* character deletion */
  if ((end == 0) && (key == conf.key_delch)) { rmch(); update_status(); end = 1; }
  if ((end == 0) && (key == conf.key_backspc)) { rmch(); update_status(); step_bwd(); end = 1; }
  if ((end == 0) && (key == KEY_ENTER)) { clear_to_eol(); update_status(); end = 1; }
  if ((end == 0) && (key == conf.key_addattribs)) { putch(oldchar); step(); update_status(); end = 1; }  /* only add attributes */
  /* function keys */
  if ((end == 0) && (key == conf.key_menu)) { menu_screen(); end = 1; }  /* show help screen */
    if (exit_program) return;
  if ((end == 0) && (key == conf.key_export)) { export(); end = 1; }
  if ((end == 0) && (key == conf.key_import)) {if (import()) save_undo(-1,-1,-1,-1); end = 1; }
  if ((end == 0) && (key == conf.key_color)) { color_dialog() ; end = 1; }
  if ((end == 0) && (key == conf.key_bgnd)) { end=1; }
  if ((end == 0) && (key == conf.key_attrib)) { attr_dialog() ; end = 1; }
  if ((end == 0) && (key == conf.key_mode)) { step_dialog() ; end=1; }
  if ((end == 0) && (key == conf.key_unknown)) { end = 1; }
  if ((end == 0) && (key == conf.key_clear)) { clearedit(); update_status(); end = 1; } /* clears screen */
  if ((end == 0) && (key == conf.key_block)) { block(); end = 1; }  /* block selection - NOT "USER FRIENDLY", FIND SOME BETTER WAY !!! */
  if ((end == 0) && (key == conf.key_paste)) { paste(); end = 1; }  /* paste clipboard */
  if ((end == 0) && (key == conf.key_undo)) { do_undo(); end = 1; }  /* undo last operation */

  /* default */
  if (end == 0) { putch(key); step(); update_status(); }
}

/* prints usage */
inline void usage(void)
{
printf("Error: Too many parameters\nUsage: cave [FILE]\n");
return;
}


/* init */
int main(int argc, char *argv[])
{
    if (argc>2)
    {
    usage();
    return 1;
    }

  int key;

#ifdef RANDOM
  open_rnd();
#endif
  read_config();
  color_load_defaults();
  cursor_load_defaults();
  draw_screen();

    if (argc==1)
    {
#ifdef BANNER
    draw_banner();
#endif
    editprefresh();
    wclear(editp);
    }
    
    if (argc==2)
    import_at_start(argv[1]);    
 
  update_status();
  while(exit_program==false)
  {
    key = getch();
    if (key != ERR) check_key(key);
  }
shut_screen();
return 0;
}
