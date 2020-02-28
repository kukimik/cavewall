/*
 *  export.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Image export routines
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
#include "export.h"
#include "config.h"
#include "cursor.h"
#include "color.h"
#include "status.h"

FILE *fd;
extern char lastname[500];

  int realxsize;
  int realysize;

/* opens a file */
int open_file(char filename[], int mode)
{
  int ret = 0;

  if (mode == 1) fd = fopen(filename, "w"); else fd = fopen(filename, "a+");
  if (fd == NULL)  /* error message on status */
  {
    ret = 1;
    status_error();
  }
  return ret;
}

/* finds the real end of line, without unimportant spaces */
int end(char *buf)
{
  int last;

  for (last = strlen(buf) - 1; last >= 0; last--)
    if (buf[last] != ' ') break;
  return last;
}


/* ansi text export */
/* sorry because of the obscure code */
void export_ansi(void)
{
  int x, y, was = 0, waszero;
  short color, bgnd, attrnum;
  short lastcolor = EDEFCOLOR;
  unsigned short i;
  chtype lastbgnd = EDEFBGND, lastattr = EDEFATTR;
  chtype ch;
  chtype attr;
  char sbuf[10];
  char *buf;  /* line buffer */
  
  buf = (char *)malloc(sizeof(char) * CBUF);
  if (buf == NULL)
  {
    status_message("Error: Can't allocate memory.");
    return;
  }
  for (y = 0; y < realysize; y++)
  {
    strcpy(sbuf, "");
    strcpy(buf, "");
    for (x = 0; x <= realxsize; x++)
    {
      if (x != realxsize) ch = mvwinch(editp, y, x);
      else  /* we are outside the screen */
      ch = ' ' | EDEFATTR | COLOR_PAIR(find_pair(EDEFCOLOR, EDEFBGND));
      /* if something goes wrong */
      if (pair_content(PAIR_NUMBER(ch & A_COLOR), &color, &bgnd) == ERR)
      {
        color = EDEFCOLOR;
	bgnd = EDEFBGND;   // !!! shouldn't it be chtype ???
      }

    attr= ch & A_ATTRONLY;
    
      was = 0;
      waszero = 0;
      /* if any codes are needed */
      if ((color != lastcolor) || (bgnd != lastbgnd) || (attr != lastattr))
      {
        sprintf(sbuf, "\x1b[");
	strcat(buf, sbuf);
        if (attr != lastattr)  /* the attribute has changed */
        {
	  if (lastattr != 0)  /* clear all attributes before */
	  {                                    /* setting another one */
	    sprintf(sbuf, "0;");
	    strcat(buf, sbuf);
	    waszero = 1;  /* inform that attributes were cleared */
          }
	    for (i=1; i<NUMATTRS; i++)
	    {
	      if (attr & avals[i])
	      {
	        switch (avals[i])  /* changes curses attr names into ansi */
	        {
		case A_BOLD:      { attrnum = 1; break; }
		case A_UNDERLINE: { attrnum = 4; break; }
		case A_BLINK:     { attrnum = 5; break; }
		case A_REVERSE:   { attrnum = 7; break; }
		default: { attrnum = 13; } // non-existant value
	        }
              if (attrnum!=13)
	      {
	      sprintf(sbuf, "%d;", attrnum);
              strcat(buf, sbuf);
	      }
	      
	      }
	    }
	  if ((attr == 0) || (waszero != 0))
	  {
	    /* force adding color codes by changing last colors/bgnds to
             * non-existant values */
	    lastcolor = 20;
	    lastbgnd = 20;
	  }
	  was = 1;
        }
     
        if ((color != lastcolor))  /* the color has changed */
        {
          sprintf(sbuf, "%d;", 30 + color);
          strcat(buf, sbuf);
	  was = 1;
        }
        if ((bgnd != lastbgnd))  /* the background color has changed */
        {
          sprintf(sbuf, "%d", 40 + bgnd);
          strcat(buf, sbuf);
	  was = 1;
        }
	if (was == 1)  /* the code was generated, must be ended with 'm' */
	{
	  sprintf(sbuf, "m");
          strcat(buf, sbuf);
	}
      }
      if (x != realxsize)  /* if we are not outside the screen */
      {
        sprintf(sbuf, "%c", (char)ch);
        strcat(buf, sbuf);
      }
      lastcolor = color;
      lastbgnd = bgnd;
      lastattr = attr;
    }

    /* remove all unneeded spaces */
    buf[end(buf)+1] = '\0';
    strcpy(sbuf, "");
    /* all attributes off at the end of line ( ESC[0m ) */
    if ((strlen(buf) != 0) && (was != 0))
    {
      sprintf(sbuf, "\x1b[0m");
      strcat(buf, sbuf);
    }
    fputs(buf, fd);
    fputc('\n', fd);
    lastcolor = EDEFCOLOR;
    lastbgnd = EDEFBGND;
    lastattr = EDEFATTR;
  }
  free(buf);
}

/* plain text export */
void export_txt(void)
{
  int x, y;
  char *buf;  /* line buffer */

  buf = (char *)malloc(sizeof(char) * CBUF);
  if (buf == NULL)
  {
    status_message("Error: Can't allocate memory.");
    return;
  }
  for (y = 0; y < realysize; y++)
  {
    for (x = 0; x < realxsize; x++) buf[x] = mvwinch(editp, y, x);
    buf[x] = '\0';
    buf[end(buf) + 1] = '\0';
    fputs(buf, fd);
    fputc('\n', fd);
  }
  free(buf);
}

/* finds real number of rows */
int findrealysize(void)
{
  int x, y;
  int maxy=0;
  
  for (y=realysize-1; y>=0; y--)
  {
      for (x = 0; x < realxsize; x++)
       if (mvwinch(editp, y, x)!=(' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR))
       {
       maxy=y;
       x=realxsize;
       y=-1;
       }
      
  }
return maxy+1;
}

/* html text export */
void export_html_css(void)
{
  int x, y;
  short color, bgnd;
  chtype attr = EDEFATTR, lastattr = EDEFATTR;
  short lastcolor = EDEFCOLOR, lastbgnd = EDEFBGND;
  chtype ch;
  bool firsttime=true;
  char sbuf[20];
  char *buf;  /* line buffer */
  buf = (char *)malloc(sizeof(char) * CBUF);

  if (buf == NULL)
  {
    status_message("Error: Can't allocate memory.");
    return;
  }

  fputs("<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"cavewall.css\"/></head><body bgcolor=\"black\">", fd);

  fputs("<pre>", fd);


  for (y = 0; y < realysize; y++)
  {
    strcpy(sbuf, "");
    strcpy(buf, "");
    for (x = 0; x < realxsize; x++)
    {
      ch = mvwinch(editp, y, x);
      /* if something goes wrong */
      if (pair_content(PAIR_NUMBER(ch & A_COLOR), &color, &bgnd) == ERR)
      {
        color = EDEFCOLOR;
	bgnd = EDEFBGND;   // !!! shouldn't it be chtype ???
      }

      attr = (ch & A_ATTRONLY);

	if (attr & A_REVERSE)
	{
	short a = color;
	color = bgnd;
	bgnd = a;
	}
      
      if ((color != lastcolor) || (bgnd != lastbgnd) || (attr != lastattr) || (firsttime))
      {
        if (firsttime!=true)
	{ 
	sprintf(sbuf, "</span><span class=");
	strcat(buf, sbuf);
	}
	else
	{
	sprintf(sbuf, "<span class=");
	strcat(buf, sbuf);
	firsttime=false;
	}

	
	switch (bgnd)
	{
	case COLOR_WHITE: sprintf(sbuf, "\"b7 c"); break;
	case COLOR_BLACK: sprintf(sbuf, "\"b0 c"); break;
	case COLOR_BLUE: sprintf(sbuf, "\"b1 c"); break;
	case COLOR_GREEN: sprintf(sbuf, "\"b2 c"); break;
	case COLOR_CYAN: sprintf(sbuf, "\"b3 c"); break;
	case COLOR_RED: sprintf(sbuf, "\"b4 c"); break;
	case COLOR_MAGENTA: sprintf(sbuf, "\"b5 c"); break;
	case COLOR_YELLOW: sprintf(sbuf, "\"b6 c"); break;
	default: sprintf(sbuf, "\"b0 c"); break;
	}
	strcat(buf, sbuf);

      if (attr & A_BOLD)
      {
      sprintf(sbuf, "b");
      strcat(buf, sbuf);
      }
	
      switch (color)
      {
      case COLOR_WHITE: sprintf(sbuf, "7"); break;
      case COLOR_BLACK: sprintf(sbuf, "0"); break;
      case COLOR_BLUE: sprintf(sbuf, "1"); break;
      case COLOR_GREEN: sprintf(sbuf, "2"); break;
      case COLOR_CYAN: sprintf(sbuf, "3"); break;
      case COLOR_RED: sprintf(sbuf, "4"); break;
      case COLOR_MAGENTA: sprintf(sbuf, "5"); break;
      case COLOR_YELLOW: sprintf(sbuf, "6"); break;
      default: sprintf(sbuf, "7"); break;
      }
      strcat(buf, sbuf);

      if (attr & A_BLINK)
      {
      sprintf(sbuf, " bl");
      strcat(buf, sbuf);
      }

      if (attr & A_UNDERLINE)
      {
      sprintf(sbuf, " ul");
      strcat(buf, sbuf);
      }
    
      sprintf(sbuf, "\">");
      strcat(buf, sbuf);
  lastcolor = color;
  lastbgnd = bgnd;
  lastattr= attr;
      }
      
        if (((char)ch != '\"') && ((char)ch!='<') && ((char)ch!='>') && ((char)ch!='&'))
    	sprintf(sbuf, "%c", (char)ch);
        else
	    switch ((char)ch)
	    {
	    case '\"': sprintf(sbuf, "&quot;"); break;
	    case '<': sprintf(sbuf, "&lt;"); break;
	    case '>': sprintf(sbuf, "&gt;"); break;
	    case '&': sprintf(sbuf, "&amp;"); break;
	    }
	
	strcat(buf, sbuf);
    }

    /* remove all unneeded spaces */
    buf[end(buf)+1] = '\0';
    strcpy(sbuf, "");

    fputs(buf, fd);
    fputs("<br>", fd);
  }
  
  fputs("</pre>", fd);
    fputs("</body></html>", fd);
  free(buf);
}


void export(void)
{
  char filename[200] = "";
  char realname[200] = "";
  int format, a, wr = 1;

 realxsize=truexsize;
 realysize=trueysize;
 realysize=findrealysize();

  format = status_question("Image export. Which format", "ANSI Text HTML Cancel", 1);
  if (format != 4)  /* check this after adding new filters! */
  {
    status_input("File name", lastname, filename, 60);
    if (strncmp(filename, "", 200) != 0)
    {
      for (a = 0; a < strlen(filename); a++)
        if (filename[a] == '~')
	{
	  status_message("~ not allowed; enter full pathname.");
	  return;
	}
      realpath(filename, realname);
      if (access(realname, F_OK) == 0)
        wr = status_question("File Exists. Overwrite", "Yes No Append", 1);
      if (wr != 2)
      {
        if (open_file(realname, wr) == 0)
        {
          cursor_up();
          switch (format)
          {
            case 1: { export_ansi(); break; }
            case 2: { export_txt(); break; }
            case 3: { export_html_css(); break; }
          }
          fclose(fd);
          strcpy(lastname, filename);
          cursor_down();
        }
      }
    }
  }
}
