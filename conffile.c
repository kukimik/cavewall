/*
 *  conffile.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Configuration file reading routines
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "conffile.h"
#include "config.h"
#ifdef HAVE_GLOB
#include <glob.h>
#endif

FILE *fd;
int line = 0;  /* line number in file */

/* configuration variables */
struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf =
{ DEFCOLOR, DEFBGND, DEFATTR, DEFMODE, FROMZERO, DEF_KEY_UP, DEF_KEY_DOWN,
  DEF_KEY_LEFT, DEF_KEY_RIGHT, DEF_KEY_LEFTBORDER, DEF_KEY_RIGHTBORDER,
  DEF_KEY_UPBORDER, DEF_KEY_DOWNBORDER, DEF_KEY_DELCH, DEF_KEY_BACKSPC,
  DEF_KEY_ADDATTRIBS, DEF_KEY_EXPORT, DEF_KEY_IMPORT, DEF_KEY_COLOR,
  DEF_KEY_BGND, DEF_KEY_ATTRIB, DEF_KEY_MODE, DEF_KEY_CLEAR, DEF_KEY_BLOCK, DEF_KEY_PASTE, DEF_KEY_UNDO, DEF_KEY_MENU, DEF_KEY_UNKNOWN };

/* works with key-value pair */
void key_pair(char key[], char value[])
{
  int a, end = 0, valerror = 1;
  char yesno[][4] = {"no", "yes"};

  /* no empty keys and values */
  if ((strcmp(key, "") != 0) && (strcmp(value, "") != 0))
  {
    if ((strcmp(key, "defcolor") == 0) && (end == 0))
    {
      for (a = 0; a <= NUMCOLORS; a++)
        if (strcmp(value, colors[a]) == 0)
	{
	  conf.defcolor = a;
	  valerror = 0;
	}
      end = 1;
    }
    if ((strcmp(key, "defbgnd") == 0) && (end == 0))
    {
      for (a = 0; a <= NUMCOLORS; a++)
        if (strcmp(value, colors[a]) == 0)
	{
	  conf.defbgnd = a;
	  valerror = 0;
	}
      end = 1;
    }
    if ((strcmp(key, "defattr") == 0) && (end == 0))
    {
      for (a = 0; a <= NUMATTRS; a++)
        if (strcmp(value, attrs[a]) == 0)
	{
  	  conf.defattr = a;
	  valerror = 0;
	}
      end = 1;
    }
    if ((strcmp(key, "defmode") == 0) && (end == 0))
    {
      for (a = 0; a <= NUMMODES; a++)
        if (strcmp(value, modes[a]) == 0)
	{
  	  conf.defmode = a;
	  valerror = 0;
	}
      end = 1;
    }
    if ((strcmp(key, "from_zero") == 0) && (end == 0))
    {
      for (a = 0; a <= 1; a++)
        if (strcmp(value, yesno[a]) == 0)
	{
  	  conf.fromzero = a;
	  valerror = 0;
	}
      end = 1;
    }
    /* key bindings */
    if ((strcmp(key, "key_up") == 0) && (end == 0))
    {
      conf.key_up = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_down") == 0) && (end == 0))
    {
      conf.key_down = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_left") == 0) && (end == 0))
    {
      conf.key_left = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_right") == 0) && (end == 0))
    {
      conf.key_right = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_leftborder") == 0) && (end == 0))
    {
      conf.key_leftborder = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_rightborder") == 0) && (end == 0))
    {
      conf.key_rightborder = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_upborder") == 0) && (end == 0))
    {
      conf.key_upborder = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_downborder") == 0) && (end == 0))
    {
      conf.key_downborder = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_delch") == 0) && (end == 0))
    {
      conf.key_delch = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_backspc") == 0) && (end == 0))
    {
      conf.key_backspc = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_addattribs") == 0) && (end == 0))
    {
      conf.key_addattribs = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_export") == 0) && (end == 0))
    {
      conf.key_export = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_import") == 0) && (end == 0))
    {
      conf.key_import = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_color") == 0) && (end == 0))
    {
      conf.key_color = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_bgnd") == 0) && (end == 0))
    {
      conf.key_bgnd = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_attrib") == 0) && (end == 0))
    {
      conf.key_attrib = atoi(value);
      valerror = 0;
      end = 1;
    }
    if ((strcmp(key, "key_mode") == 0) && (end == 0))
    {
      conf.key_mode = atoi(value);
      valerror = 0;
      end = 1;
    }
    /* "alles pass nicht" */
    if (end == 0)
    {
      printf("  Error at line %d: Unknown key %s.\n", line, key);
      exit(EXIT_FAILURE);
    }
    if (valerror != 0)
    {
      printf("  Error at line %d: Incorrect value %s.\n", line, value);
      exit(EXIT_FAILURE);
    }
  }
}

/* breaks the config file line into the key-value pair */
void parse_line(char buf[])
{
  int a = 0, b = 0, waskey = 0, wasvalue = 0;
  char key[40] = "";
  char value[40] = "";

  if (strcmp(buf, "") != 0)
  {
    /* comment chars */
    while ((buf[a] != '\0') && (wasvalue == 0) && (buf[a] != '#') && (buf[a] != '%') && (buf[a] != ';'))
    {
      if (b < 40)
      {
        if (buf[a] == ' ')
	{
	  if (waskey == 0) waskey = 1; else wasvalue = 1;
	  b = 0;
	}
	else
	if ((isalpha(buf[a]) != 0) || (isdigit(buf[a]) != 0) || (ispunct(buf[a]) != 0))
	{
          if (waskey == 0) key[b] = buf[a]; else value[b] = buf[a];
          b++;
	}
      }
      a++;
    }
    key_pair(key, value);
  }
}

/* opens a file */
void dig_file(char filename[])
{
  char *buf;

  fprintf(stderr, "Reading config file %s.\n", filename);
  fd = fopen(filename, "r");
  if (fd == NULL)  /* error message on status */
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  else
  {
    buf = (char *)malloc(sizeof(char) * LBUF);
    if (buf == NULL)
    {
      fclose(fd);
      fprintf(stderr, "  Error reading config file: Can't allocate memory.");
      exit(EXIT_FAILURE);
    }
    while(fgets(buf, LBUF, fd) != NULL)
    {
      line++;
      parse_line(buf);
    }
    fclose(fd);
    free(buf);
  }
}

/* parses config files */
void read_config(void)
{
  char filename[200] = PRIMCONF;
#ifdef HAVE_GLOB
  glob_t result;
#endif

#ifdef HAVE_GLOB
  glob(filename, GLOB_TILDE, NULL, &result);
  if (result.gl_pathc == 1) strncpy(filename, result.gl_pathv[0], 200);
  globfree(&result);
#endif

  /* can we read primary/secondary config file? */
  if (access(filename, R_OK) == 0) dig_file(filename);
  else if (access(SECCONF, R_OK) == 0) dig_file(SECCONF);
}
