/*
 *  menu.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  General routines
 *
 *  Copyright (C) 2006 Kuki <kukimik@poczta.onet.pl>
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


extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

// ===== GLOBAL VARIABLES =====

WINDOW *menuw, *dialogw;


// ==== FUNCTIONS ====

//
/* shows help screen */
//
void help_screen(void)
{  
wclear(menuw);
attron(A_BOLD);
mvwprintw(menuw,1,xsize/2-2,"HELP");
attroff(A_BOLD);
mvwprintw(menuw,3,2,"F1 - show menu");
mvwprintw(menuw,4,2,"F2 - export");
mvwprintw(menuw,5,2,"F3 - import");
mvwprintw(menuw,6,2,"F4 - block operations");
mvwprintw(menuw,7,2,"F5 - select foreground/background color");
mvwprintw(menuw,8,2,"F6 - select attributes");
mvwprintw(menuw,9,2,"F7 - select cursor step mode");
mvwprintw(menuw,12,2,"F10 - paste from clipboard");
mvwprintw(menuw,13,2,"F11 - undo");
mvwprintw(menuw,14,2,"F12 - clear screen");
mvwprintw(menuw,16,(xsize-2)/2-13,"Press any key to continue.");
wrefresh(menuw);
getch();
return;
}

//
/* menu and dialogs handling */
//
char menu_handle(char last, char choice, const short int x0, const unsigned int y0, WINDOW *dialogwindow)
{
short int x=choice+x0;
int key;
mvwprintw(dialogwindow,x,y0,"->");	
wrefresh(dialogwindow);
do
{
key=getch();

if (key==conf.key_up)
{	
    mvwprintw(dialogwindow,x,y0,"  ");
    x-=1;
    if (x<=x0) x=x0;
    mvwprintw(dialogwindow,x,y0,"->");	
    wrefresh(dialogwindow);
}	

if (key==conf.key_down)
{	
    mvwprintw(dialogwindow,x,y0,"  ");
    x+=1;
    if (x>=last+x0) x=x0+last-1;
    mvwprintw(dialogwindow,x,y0,"->");	
    wrefresh(dialogwindow);
}

if (key==conf.key_upborder)
{	
    mvwprintw(dialogwindow,x,y0,"  ");
    x=x0;
    mvwprintw(dialogwindow,x,y0,"->");	
    wrefresh(dialogwindow);
}

if (key==conf.key_downborder)
{	
    mvwprintw(dialogwindow,x,y0,"  ");
    x=x0+last-1;
    mvwprintw(dialogwindow,x,y0,"->");	
    wrefresh(dialogwindow);
}

if (last<=10)
{
if ((key<'0'+last) && (key>='0'))
{
mvwprintw(dialogwindow,x,y0,"  ");
x=x0+key-'0';
mvwprintw(dialogwindow,x,y0,"->");	
wrefresh(dialogwindow);
}
}

if (key==10) // enter
{
    choice=x-x0;
}

} while (key!=10); // inner loop - for moving around in the menu and choosing menu positions


return choice;
}

//
/* attribute selection dialog */
//
void attr_dialog(void)
{
if ((dialogw = newwin(NUMATTRS+1,15, 0, 0)) == NULL) printf("error creating dialog window\n");
int i;
unsigned short choice=0;
wclear(dialogw);

if (NUMATTRS<=10)
{
    for (i=0; i<NUMATTRS; i++)
    {
    mvwaddch(dialogw,i,2,'0'+i);
    mvwaddch(dialogw,i,3,'.');
    mvwprintw(dialogw,i,4,"%c%s", ( attribute & avals[i] ? '*' : ' '), attrs[i]);
    }
    mvwaddch(dialogw,i,2,'0'+i);
    mvwaddch(dialogw,i,3,'.');
    mvwprintw(dialogw,i,5,"Exit" );
}
else
{
    for (i=0; i<NUMATTRS; i++)
    {
    mvwprintw(dialogw,i,4,"%c%s", ( attribute & avals[i] ? '*' : ' '), attrs[i]);
    }
    mvwprintw(dialogw,i,5,"Exit" );
}

// Allow user to move around
do
{
    choice=menu_handle(NUMATTRS+1,choice,0,0,dialogw);
    if (choice==0) 
    {
    attribute=A_NORMAL;
    for (i=0; i<NUMATTRS; i++) mvwaddch(dialogw,i,4,' ');    
    update_status();
    }

    else if (!(choice==NUMATTRS))
    {
    if (attribute & avals[choice]) {attribute = attribute & ~(avals[choice]); mvwaddch(dialogw,choice,4,' ');}
    else { attribute = attribute | avals[choice]; mvwaddch(dialogw,choice,4,'*'); }
    update_status();
    }
} while (choice!=NUMATTRS);

touchwin(editp);
editprefresh();
delwin(dialogw);
return;
}

//
/* color selection dialog */
//
void color_dialog(void)
{  
if ((dialogw = newwin(11,29, 0, 0)) == NULL) printf("error creating dialog window\n");

// draw the color box
int i, j;
wclear(dialogw);
mvwprintw(dialogw,0,15,"FG");
mvwprintw(dialogw,5,0,"B");
mvwprintw(dialogw,6,0,"G");
mvwprintw(dialogw,1,4," 0  1  2  3  4  5  6  7");
for (i=0; i<8; i++)
{
    for (j=0; j<8; j++)
    {
    mvwaddch(dialogw,i+2,3*j+5,'#' | COLOR_PAIR(find_pair(i,j)) | attribute );
    }
mvwaddch(dialogw,i+2,2,'0'+i);
}
wrefresh(dialogw);


// Allow user to move around
char y=(color+1)*3;
char x=background+2;
int key;
mvwprintw(dialogw,x,y,"->");	
wrefresh(dialogw);
do
{
key=getch();

if (key==conf.key_left)
{
    mvwprintw(dialogw,x,y,"  ");
    y-=3;
    if (y<3) y=3;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==conf.key_right)
{
    mvwprintw(dialogw,x,y,"  ");
    y+=3;
    if (y>24) y=24;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==conf.key_up)
{	
    mvwprintw(dialogw,x,y,"  ");
    x-=1;
    if (x<2) x=2;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}	

if (key==conf.key_down)
{	
    mvwprintw(dialogw,x,y,"  ");
    x+=1;
    if (x>9) x=9;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}	

if (key==conf.key_upborder)
{	
    mvwprintw(dialogw,x,y,"  ");
    x=2;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==conf.key_downborder)
{	
    mvwprintw(dialogw,x,y,"  ");
    x=9;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==conf.key_leftborder)
{	
    mvwprintw(dialogw,x,y,"  ");
    y=3;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==conf.key_rightborder)
{	
    mvwprintw(dialogw,x,y,"  ");
    y=24;
    mvwprintw(dialogw,x,y,"->");	
    wrefresh(dialogw);
}

if (key==10) // enter
{
    color=y/3-1;
    background=x-2;
    pair=find_pair(y/3-1,x-2);
    update_status();
}

} while (key!=10); // inner loop - for moving around in the menu and choosing menu positions
delwin(dialogw);

touchwin(editp);
editprefresh();
return;
}

//
/* step mode selection dialog */
//
void step_dialog(void)
{
if ((dialogw = newwin(NUMMODES,14, 0, 0)) == NULL) printf("error creating dialog window\n");
int i;
wclear(dialogw);

if (NUMMODES<=10)
{
    for (i=0; i<NUMMODES; i++)
    {
    mvwaddch(dialogw,i,2,'0'+i);
    mvwaddch(dialogw,i,3,'.');
    mvwprintw(dialogw,i,4,modes[i]);
    }
}
else
{
    for (i=0; i<NUMMODES; i++)
    {
    mvwprintw(dialogw,i,2,modes[i]);
    }
}

// Allow user to move around
mode=menu_handle(NUMMODES,mode,0,0,dialogw);

update_status();
touchwin(editp);
editprefresh();
delwin(dialogw);
return;
}


//
/* show usable menu window */
//
void menu_screen(void)
{
bool exit_menu=false;
char choice=0;
const int MAXPOSITION=7;
if ((menuw = newwin(ysize-1,xsize, 0, 0)) == NULL) printf("error creating menu window\n");

do
{
wclear(menuw);
attron(A_BOLD);
mvwprintw(menuw,1,xsize/2-2,"MENU");
mvwprintw(menuw,2,xsize/2-3,"------");
attroff(A_BOLD);
mvwprintw(menuw,3,3,"0. Help");
mvwprintw(menuw,4,3,"1. Select fg/bg color");
mvwprintw(menuw,5,3,"2. Select attributes");
mvwprintw(menuw,6,3,"3. Select step mode");
#ifdef FIGLET
mvwprintw(menuw,7,3,"4. Insert FIGletters");
#else
mvwprintw(menuw,7,3,"4. Insert FIGletters (OFF)");
#endif
mvwprintw(menuw,8,3,"5. Exit menu");
mvwprintw(menuw,9,3,"6. Quit CAVE WALL");
wrefresh(menuw);

choice=menu_handle(MAXPOSITION,choice,3,1,menuw);

    switch (choice)
    {    
    case 0: // show help screen
    help_screen();
    break; 
    
    case 1: // color selection dialog
    color_dialog();
    break;

    case 2: // attributes selection dialog
    attr_dialog();
    break;

    case 3: // step mode selection dialog
    step_dialog();
    break;

    case 5: // exit menu - handled in the while loop
    exit_menu=true;
    break;
        
    case 6: // maxposition - quit program
    exit_program=true;
    break;
    }

} while (exit_menu==false && exit_program==false); // outer loop - shows menu

delwin(menuw);

if (exit_program==false) // when exiting menu only
{
touchwin(editp);
editprefresh();
}

return;
}


