/*
 *  config.h
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Configuration defaults
 *
 *  Copyright (C) 2002 Michal Miszewski <fixer@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef CONFIG_H
#define CONFIG_H

#define RANDOM		/* remove if you don't want randomizing functions */
#define HAVE_GLOB	/* remove if you don't have glob() */
#define BANNER		/* remove if you don't want the startup banner */
#define RNDDEV		"/dev/urandom"	/* randomizing device */
#define HIGHLIGHT	/* remove if you don't want block selection highlighting */
#define FIGLET		/* remove if you don't have FIGlet installed */

/* startup text parameters */
#define DEFCOLOR	COLOR_WHITE
#define DEFBGND		COLOR_BLACK
#define DEFATTR		A_NORMAL
#define DEFMODE		0		/* 0 to NUMMODES */
#define FROMZERO	0		/* coordinates display mode */

/* default terminal attributes for export */

#define EDEFCOLOR	COLOR_WHITE
#define EDEFBGND	COLOR_BLACK
#define EDEFATTR	A_NORMAL

/* config file paths */
#define PRIMCONF	"~/.caverc"	/* primary path */
#define SECCONF		"/etc/caverc"	/* secondary path */

/* default key bindings */

#define	DEF_KEY_UP		KEY_UP
#define	DEF_KEY_DOWN		KEY_DOWN
#define	DEF_KEY_LEFT		KEY_LEFT
#define	DEF_KEY_RIGHT		KEY_RIGHT
#define DEF_KEY_LEFTBORDER	KEY_HOME
#define DEF_KEY_RIGHTBORDER	KEY_END
#define DEF_KEY_UPBORDER	KEY_PPAGE
#define DEF_KEY_DOWNBORDER	KEY_NPAGE
#define DEF_KEY_DELCH		KEY_DC
#define DEF_KEY_BACKSPC		KEY_BACKSPACE
#define DEF_KEY_ADDATTRIBS	32
#define DEF_KEY_MENU		KEY_F(1) 
#define DEF_KEY_EXPORT		KEY_F(2)
#define DEF_KEY_IMPORT		KEY_F(3)
#define DEF_KEY_BLOCK		KEY_F(4)
#define DEF_KEY_COLOR		KEY_F(5)
#define DEF_KEY_ATTRIB		KEY_F(6)
#define DEF_KEY_MODE		KEY_F(7)

#define DEF_KEY_BGND		KEY_F(8) // NOT USED
#define DEF_KEY_UNKNOWN		KEY_F(9) // NOT USED

#define DEF_KEY_PASTE		KEY_F(10)
#define DEF_KEY_UNDO		KEY_F(11) 
#define DEF_KEY_CLEAR		KEY_F(12)

/* for arrays, shouldn't be changed */
#define NUMCOLORS       8
#define NUMATTRS        6
#define NUMMODES	10

#endif  /* CONFIG_H */
