 /*
 *  import.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Image import routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "config.h"
#ifdef HAVE_GLOB
#include <glob.h>
#endif
#include "import.h"
#include "color.h"
#include "cursor.h"
#include "status.h"

FILE *fd;
extern char lastname[500];
static int oldx,oldy;

struct attribs
{
  short color, background, colorpair;
  chtype attrmask;
};

/* opens a file */
int read_file(char filename[])
{
  int ret = 0;

  fd = fopen(filename, "r");
  if (fd == NULL)  /* error message on status */
    ret = 1;

  return ret;
}

/* parses the ansi code */
void ansicode(char code[], int values[5])
{
  int a, c = 0, d = 0;
  char buf[4] = "";

  for (a = 0; a < 5; a++) values[a] = -1;
  for (a = 0; a < strlen(code); a++)
  {
    if (isdigit(code[a]) != 0)  /* digits */
    {
      if (c < sizeof(buf))
      {
        buf[c] = code[a];
        c++;
      }
    }
    else
    if ((d <= 5) && (strcmp(buf, "") != 0))
    {
      buf[c] = '\0';
      values[d] = atoi(buf);
      strcpy(buf, "");
      c = 0;
      d++;
    }
  }
}

/* zeroes an attribs structure */
void clean_attribs(struct attribs *act)
{
  act->color = 20;
  act->background = 20;
  act->colorpair = 20;
  act->attrmask = A_NORMAL;
}

/* interprets the ansi codes */
void interpret_ansi(char code[], struct attribs *act, int *x, int *y)
{

  int values[5];
  short color=act->color, bgnd=act->background;
  int a = 0;

  ansicode(code, values);
  switch (code[strlen(code) - 1])
  {
    case 'm':  /* color/attribute change */
    {
      for (a = 0; a < 5; a++)
      if (values[a] != -1)
      {
        if (values[a] == 0) clean_attribs(act);   /* reset code */
	if ((values[a] > 0) && (values[a] <= 7))  /* text attributes */
	{
	  switch (values[a])
	  {
            case 1: { act->attrmask = act->attrmask | A_BOLD; break; }
            case 4: { act->attrmask = act->attrmask | A_UNDERLINE; break; }
	    case 5: { act->attrmask = act->attrmask | A_BLINK; break; }
	    case 7: { act->attrmask = act->attrmask | A_REVERSE; break; }
	    case 8: { act->attrmask = act->attrmask | A_INVIS; break; }
	  }
	}
	/* colour && background */
        if ((values[a] >= 30) && (values[a] <= 37)) color = values[a] - 30;
	if ((values[a] >= 40) && (values[a] <= 47)) bgnd = values[a] - 40;
	if ((color != act->color) || (bgnd != act->background))
	{
	  act->color = color;
	  act->background = bgnd;
	  act->colorpair = find_pair(color, bgnd);
	}
      }
      break;
    }

    case 'H':
    case 'f':
    {
      for (a = 0; a < 2; a++)
      {
	if (values[a]!=-1)
	{
            switch (a)
	    {
		case 0: { *y = values[a]; break; }
		case 1: { *x = values[a]; break; }
	    }
	}
      }
      break;
    }

    case 'A':
    {
        if (values[0] != -1)
	    *y=*y-values[0];
      break;
    }

    case 'B':
    {
        if (values[0] != -1)
	    *y=*y+values[0];
      break;
    }

    case 'C':
    {
        if (values[0] != -1)
	    *x=*x+values[0];
      break;
    }

    case 'D':
    {
        if (values[0] != -1)
	    *x=*x-values[0];
      break;
    }

    case 's':
    {
        oldx=*x;
	oldy=*y;
      break;
    }

    case 'u':
    {
        *x=oldx;
	*y=oldy;
      break;
    }
    
    case 'J':
    {
	if (values[0]==2)
	{
	    wclear(editp);
	    *x=0;
	    *y=0;
	}    
      break;
    }
    
    case 'K':
    {
    int i;
	for (i=*x; i<truexsize; i++)
	    mvwaddch(editp,*y,i,' ' | DEFATTR | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND))); //A_NORMAL | COLOR_PAIR(find_pair(COLOR_WHITE,COLOR_BLACK)));
    break;
    }



  }
}

/* image import */
void import_text(void)
{
  int x = 0, y = 0, maxx=0, maxy=0;    /* screen coordinates */
  int a, b;
  char *buf;           /* line buffer */
  char code[15];       /* ansi code buffer */
  struct attribs out;  /* actual character attributes */

  oldx=0;
  oldy=0;

change_editp_size(1000,1000);
wclear(editp);

  buf = (char *)malloc(sizeof(char) * CBUF);
  if (buf == NULL)
  {
    status_message("Error: Can't allocate memory.");
    return;
  }
  /* startup text attributes */
  {
    out.color = DEFCOLOR; //COLOR_WHITE;
    out.background = DEFBGND; //COLOR_BLACK;
    out.colorpair = find_pair(out.color, out.background);
    out.attrmask = DEFATTR; //A_NORMAL;
  }
  while (fgets(buf, CBUF, fd) != NULL)
  {
    x = 0;
    for (a = 0; a < CBUF; a++)
    {
      if (buf[a] == '\0') break;  /* end of the string */
      if (buf[a] == '\x1b')       /* escape char */
      {
        strcpy(code, "");
	b = 0;
	a++;
	while (buf[a] != '\0')
	{
	  code[b] = buf[a];
	  /* alphabetical char ends the sequence */
	  if (isalpha(buf[a]) != 0)
	  {
	    code[b + 1] = '\0';
	    interpret_ansi(code, &out, &x, &y);
	    break;
	  }
	  a++;
	  b++;
	}
      }
      else
      {
        mvwaddch(editp, y, x, buf[a] | out.attrmask | COLOR_PAIR(out.colorpair));
        x++;
        if (maxx<x) maxx=x;
	if (maxy<y) maxy=y;
      }
    }
    y++;
    if (maxx<x) maxx=x;
    if (maxy<y) maxy=y;
  }
  free(buf);
  change_editp_size(maxy-1000,maxx-1000);
  actx=0;
  acty=0;
  topline=0;
  leftline=0;
  cursor_down();
  editprefresh();
}

void import_at_start(char parameter[])
{
  char filename[200]; 
  strncpy(filename,parameter,200);
  char realname[200] = "";
  
#ifdef HAVE_GLOB
  glob_t result;

    glob(filename, GLOB_TILDE, NULL, &result);
    if (result.gl_pathc > 1) 
    {
      status_message("Wildcards not allowed.");
      globfree(&result);
      editprefresh();
      return;
    }
    else
    {
      if (result.gl_pathc > 0) strncpy(filename, result.gl_pathv[0], 200);
    }
    globfree(&result);
#endif

    realpath(filename, realname);
    strcpy(lastname, realname);
    
    if (read_file(realname) == 0)
    {
      cursor_up();
      import_text();
      fclose(fd);
    }
    else
    editprefresh();
return;
}


bool import(void)
{
  char filename[200] = "";
  char realname[200] = "";

#ifdef HAVE_GLOB
  glob_t result;
#endif
  
  status_input("Image import. File name", "", filename, 60);

  if (strncmp(filename, "", 200) != 0)
  {

#ifdef HAVE_GLOB
    glob(filename, GLOB_TILDE, NULL, &result);
    if (result.gl_pathc > 1) 
    {
      status_message("Wildcards not allowed.");
      globfree(&result);
      return false;
    }
    else
    {
      if (result.gl_pathc > 0) strncpy(filename, result.gl_pathv[0], 200);
    }
    globfree(&result);
#endif

    realpath(filename, realname);
    if (read_file(realname) == 0)
    {
      cursor_up();
      import_text();
      fclose(fd);
      strcpy(lastname, filename);
    }
    else
    {
    status_error();
    return false;
    }
  return true;
  }
return false;
}
