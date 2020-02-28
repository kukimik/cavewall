/*
 *  block.c
 *
 *  CAVE WALL - Colored Ascii Visual Editor Without Any Loathsome Limitations
 *  Block operations and undo
 *
 *  Copyright (C) 2004 Kuki (kukimik@poczta.onet.pl)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <curses.h>
#include <string.h>
#include "block.h"
#include "config.h"
#include "color.h"
#include "status.h"
#include "cursor.h"
#include "export.h"

extern struct config
{
  int defcolor, defbgnd, defattr, defmode, fromzero,
  key_up, key_down, key_left, key_right,
  key_leftborder, key_rightborder, key_upborder, key_downborder,
  key_delch, key_backspc, key_addattribs,
  key_export, key_import, key_color, key_bgnd, key_attrib, key_mode, key_clear, key_block, key_paste, key_undo, key_menu, key_unknown;
} conf;

// ================================== GLOBAL VARIABLES ====================================================

int xSTART=-1,ySTART=-1,xFIN=-1,yFIN=-1;
int clipx=-1,clipy=-1;
int undoxsize=-1,undoysize=-1;
int undox=-1,undoy=-1;
bool blockmode=false;

WINDOW *clipboardp, *undop;

// ======================================== FUNCTIONS ====================================================


// *************************************
//
// void block_colorize(void)
// give current attributes and colors to all chars in the block
//
// *************************************

void block_colorize(void)
{
int x,y;

save_undo(ySTART,xSTART,yFIN,xFIN);

cursor_up();
for (x=xSTART; x<=xFIN; x++)
{
    for (y=ySTART; y<=yFIN; y++)
    {
    char character=mvwinch(editp,y,x);
    mvwaddch(editp,y,x,character | COLOR_PAIR(pair) | attribute);
    }
}
cursor_down();
return;
}

// ***********************************
//
// void block_clear(void)
// clears the block
//
// ***********************************

void block_clear(void)
{
int x,y;

save_undo(ySTART,xSTART,yFIN,xFIN);

cursor_up();

for (x=xSTART; x<=xFIN; x++)
{
    for (y=ySTART; y<=yFIN; y++)
    mvwaddch(editp,y,x,' ' | COLOR_PAIR(find_pair(DEFCOLOR, DEFBGND)) | DEFATTR );
}

cursor_down();
return;
}

// **********************************
//
// void block_drawbox(void)
// draws a nice box around the block
//
// **********************************

void block_drawbox(void)
{
int x,y;
char top_wall[10],bottom_wall[10],left_wall[10],right_wall[10];
int top_len,bottom_len,left_len,right_len;
int act_char1,act_char2;

save_undo(ySTART,xSTART,yFIN,xFIN);

cursor_up();
status_input("Enter top wall character sequence","",top_wall,10);
status_input("Enter bottom wall character sequence","",bottom_wall,10);
status_input("Enter left wall character sequence","",left_wall,10);
status_input("Enter right wall character sequence","",right_wall,10);

top_len=strlen(top_wall);
bottom_len=strlen(bottom_wall);
right_len=strlen(right_wall);
left_len=strlen(left_wall);

act_char1=0;
act_char2=0;
    for (y=ySTART+1; y<=yFIN-1; y++)
    {
	if (left_len!=0)
	{
	mvwaddch(editp,y,xSTART, left_wall[act_char1] | COLOR_PAIR(pair) | attribute);
	act_char1++;
	    if (act_char1>=left_len) act_char1=0;
	}

	if (right_len!=0)
	{
	mvwaddch(editp,y,xFIN, right_wall[act_char2] | COLOR_PAIR(pair) | attribute);
	act_char2++;    
	    if (act_char2>=right_len) act_char2=0;
	}
    }

act_char1=0;
act_char2=0;
    for (x=xSTART+1; x<=xFIN-1; x++)
    {
	if (top_len!=0)
	{
	mvwaddch(editp,ySTART,x, top_wall[act_char1] | COLOR_PAIR(pair) | attribute);
	act_char1++;
	    if (act_char1>=top_len) act_char1=0;
	}
	
	if (bottom_len!=0)
	{
	mvwaddch(editp,yFIN,x, bottom_wall[act_char2] | COLOR_PAIR(pair) | attribute);
	act_char2++;    
	    if (act_char2>=bottom_len) act_char2=0;
	}
    }
cursor_down();
return;
}

// ************************************
//
// void block_fill(void)
// fills block with pattern saved in clipboard (saved colors) or string entered from keyboard with vert/horiz selection (current color)
//
// ************************************

void block_fill(void)
{
int x,y,cx,cy;
int answer;
cx=0;
cy=0;

answer=status_question("What kind of fill","Clipboard String",1);

if (answer==1)
{

    if (clipx<0 || clipy<0)
    {
    status_message("Clipboard does not exist!");
    return;
    }    

    save_undo(ySTART,xSTART,yFIN,xFIN);

    cursor_up();
    for (y=ySTART; y<=yFIN; y++)
    {
	for (x=xSTART; x<=xFIN; x++)
	{
	mvwaddch(editp,y,x,mvwinch(clipboardp,cy,cx));
	cx++;
	    if (cx>=clipx) cx=0;
	}
    cy++;
    cx=0;
	if (cy>=clipy) cy=0;
    }
    cursor_down();
}
else
{

    char sequence[20];
    int cmax;
    int answer=1;

    status_input("Enter char sequence to fill block with","",sequence,20);
    cmax=strlen(sequence);
    if (cmax!=0)
    {
    if (cmax>1) answer=status_question("How to fill","a.Vert_lines b.Horiz_lines c.Vertically d.Horizontally", 1);

    cursor_up();
    save_undo(ySTART,xSTART,yFIN,xFIN);
    
    switch(answer)
    {
	case 1:
		for (y=ySTART; y<=yFIN; y++)
    		{
	    		for (x=xSTART; x<=xFIN; x++)
	    		{
	    		mvwaddch(editp,y,x,sequence[cx] | COLOR_PAIR(pair) | attribute);
	    		cx++;
			if (cx>=cmax) cx=0;
	    		}
		cx=0;
		}
	break;

	case 2:
		for (y=ySTART; y<=yFIN; y++)
		{
    	    		for (x=xSTART; x<=xFIN; x++)
	    		{
	    		mvwaddch(editp,y,x,sequence[cx] | COLOR_PAIR(pair) | attribute);
	    		}
		cx++;
	    	if (cx>=cmax) cx=0;
		}
    	break;

	case 3:
		for (x=xSTART; x<=xFIN; x++)
    		{
	    		for (y=ySTART; y<=yFIN; y++)
	    		{
	    		mvwaddch(editp,y,x,sequence[cx] | COLOR_PAIR(pair) | attribute);
	    		cx++;
			if (cx>=cmax) cx=0;
	    		}
		}
    	break;

	case 4:
		for (y=ySTART; y<=yFIN; y++)
    		{
	    		for (x=xSTART; x<=xFIN; x++)
	    		{
	    		mvwaddch(editp,y,x,sequence[cx] | COLOR_PAIR(pair) | attribute);
	    		cx++;
			if (cx>=cmax) cx=0;
	    		}
		}
    	break;

    }
    cursor_down();    
    }
}

return;
}

// ************************************
//
// void block_mirrorH(void)
// mirror the block horizontally
//
// ************************************

void block_mirrorH(void)
{

// ******* chtype mirrorH_convert(chtype) - converts H-mirrorable characters - MOVE CONVERSION TABLES TO SEPARATE FILES !!! *********

chtype mirrorH_convert(chtype character_and_attr)
{
char character=character_and_attr;

    switch (character)
    {
    case '/':
	character='\\';
	break;
    case '\\':
	character='/';
	break;
    case '\'':
    	character='.';
	break;
    case '`':
    	character=',';
	break;
    case '.':
    	character='\'';
	break;
    case ',':
    	character='`';
	break;
    case '_':
    	character='"';
	break;
    case '"':
    	character='_';
	break;
    case '^':
    	character='v';
	break;
    case 'v':
    	character='^';
	break;
    case 'M':
    	character='W';
	break;
    case 'W':
    	character='M';
	break;
    case 'm':
    	character='w';
	break;
    case 'w':
    	character='m';
	break;
    default:
	return character_and_attr;
	break;
    }

int color_pair=(character_and_attr & A_COLOR);
int attributes=(character_and_attr & A_ATTRONLY);
character_and_attr=(character | color_pair | attributes) ;
return character_and_attr;
}

// ***********************************************************************************

int x,y;
unsigned short answer;

answer=status_question("Change mirrorable chracters (\"\\\" into \"/\", etc.)","Yes No",1);

save_undo(ySTART,xSTART,yFIN,xFIN);
cursor_up();

if (answer==1)
{
	for (x=xSTART; x<=xFIN; x++)
	{
    	    for (y=ySTART; y<=ySTART+(yFIN-ySTART)/2; y++)
    	    {
		chtype character1, character2;
    
    		character1=mirrorH_convert(mvwinch(editp,y,x));
		character2=mirrorH_convert(mvwinch(editp,yFIN-y+ySTART,x));    

    		mvwaddch(editp,y,x,character2);
		mvwaddch(editp,yFIN-y+ySTART,x,character1);
	    }
	}
}
else
{
    for (x=xSTART; x<=xFIN; x++)
    {
        for (y=ySTART; y<=yFIN/2+ySTART/2; y++) 	// yFIN/2+ySTART/2 != ySTART+(yFIN-ySTART)/2, but here it will do
        {
        chtype character1, character2;
        character1=mvwinch(editp,y,x);
        character2=mvwinch(editp,yFIN-y+ySTART,x);
        mvwaddch(editp,y,x,character2);
	mvwaddch(editp,yFIN-y+ySTART,x,character1);
	}
    }
}
cursor_down();
return;


}

// ************************************
//
// void block_mirrorV(void)
// mirror the block vertically
//
// ************************************

void block_mirrorV(void)
{

// ******* chtype mirrorV_convert(chtype) - converts H-mirrorable characters - MOVE CONVERSION TABLES TO SEPARATE FILES !!! *********

chtype mirrorV_convert(chtype character_and_attr)
{
char character=character_and_attr;

    switch (character)
    {
    case '/':
	character='\\';
	break;
    case '\\':
	character='/';
	break;
    case '[':
    	character=']';
	break;
    case ']':
    	character='[';
	break;
    case '{':
    	character='}';
	break;
    case '}':
    	character='{';
	break;
    case '(':
    	character=')';
	break;
    case ')':
    	character='(';
	break;
    case '<':
    	character='>';
	break;
    case '>':
    	character='<';
	break;
    case 'p':
    	character='q';
	break;
    case 'q':
    	character='p';
	break;
    case 'b':
    	character='d';
	break;
    case 'd':
    	character='b';
	break;
    default:
	return character_and_attr;
	break;
    }

int color_pair=(character_and_attr & A_COLOR);
int attributes=(character_and_attr & A_ATTRONLY);
character_and_attr=(character | color_pair | attributes) ;
return character_and_attr;
}

// ***********************************************************************************


int x,y;
unsigned short answer;

answer=status_question("Change mirrorable chracters (\"\\\" into \"/\", etc.)","Yes No",1);

save_undo(ySTART,xSTART,yFIN,xFIN);
cursor_up();

if (answer==1)
{
	for (y=ySTART; y<=yFIN; y++)
	{
    	    for (x=xSTART; x<=xSTART+(xFIN-xSTART)/2; x++)
    	    {
		chtype character1, character2;
    
    		character1=mirrorV_convert(mvwinch(editp,y,x));
		character2=mirrorV_convert(mvwinch(editp,y,xFIN-x+xSTART));    

		mvwaddch(editp,y,x,character2);
		mvwaddch(editp,y,xFIN-x+xSTART,character1);
	    }
	}
}
else
{
    for (y=ySTART; y<=yFIN; y++)
    {
	for (x=xSTART; x<=xFIN/2+xSTART/2; x++)
	{
	chtype character1, character2;
	character1=mvwinch(editp,y,x);
	character2=mvwinch(editp,y,xFIN-x+xSTART);
	mvwaddch(editp,y,x,character2);
	mvwaddch(editp,y,xFIN-x+xSTART,character1);
	}
    }
}
cursor_down();
return;
}

// ****************************************
//
// void block_rotate(void)
// rotates block 90,180 or 270 degrees
//
// ****************************************

void block_rotate(void)
{
int x,y;
unsigned short answer;
WINDOW *rotatep;
int rotx,roty;


answer=status_question("Rotate by how many degrees","a.90 b.180 c.270",1);
switch(answer)
{
    case 1:
	rotx=yFIN-ySTART+1;
	roty=xFIN-xSTART+1;


	if (rotx+xSTART>truexsize || roty+ySTART>trueysize)
	{
	    if ((rotx+xSTART>=1000 || roty+ySTART>=1000) && (status_question("Image larger than 1000x1000 after rotation. Proceed","Yes No",1)==1)) return;
	change_editp_size(roty+ySTART-trueysize, rotx+xSTART-truexsize);
	}

	{								// additional undo save for rotate, cause output coords!=block coords
	int endx,endy;
	if (roty+ySTART>yFIN) endy=roty+ySTART; else endy=yFIN;
	if (rotx+xSTART>xFIN) endx=rotx+xSTART; else endx=xFIN;
	save_undo(ySTART,xSTART,endy,endx);
	}
		
	rotatep=newpad(roty,rotx);
	cursor_up();

	for (x=xSTART;x<=xFIN;x++)
	{
	    for (y=ySTART;y<=yFIN;y++)
	    {
	    mvwaddch(rotatep,x-xSTART,rotx-y+ySTART-1,mvwinch(editp,y,x));
	    mvwaddch(editp,y,x,' ' | COLOR_PAIR(0) | A_NORMAL);
	    }
	}

	for (x=0;x<rotx;x++)
	{
	    for (y=0;y<roty;y++)
	    {
	    mvwaddch(editp,ySTART+y,xSTART+x,mvwinch(rotatep,y,x));
	    }
	}


	delwin(rotatep);
	break;

    case 2:
	rotx=xFIN-xSTART+1;
	roty=yFIN-ySTART+1;
	rotatep=newpad(roty,rotx);

	save_undo(ySTART,xSTART,yFIN,xFIN);
	cursor_up();

	for (x=xSTART;x<=xFIN;x++)
	{
	    for (y=ySTART;y<=yFIN;y++)
	    {
	    mvwaddch(rotatep,roty-y+ySTART-1,rotx-x+xSTART-1,mvwinch(editp,y,x));
	    mvwaddch(editp,y,x,' ' | COLOR_PAIR(0) | A_NORMAL);
	    }
	}

	for (x=0;x<rotx;x++)
	{
	    for (y=0;y<roty;y++)
	    {
	    mvwaddch(editp,ySTART+y,xSTART+x,mvwinch(rotatep,y,x));
	    }
	}
	
	delwin(rotatep);
	break;

    case 3:
	rotx=yFIN-ySTART+1;
	roty=xFIN-xSTART+1;
	
	if (rotx+xSTART>truexsize || roty+ySTART>trueysize)
	{
	    if ((rotx+xSTART>=1000 || roty+ySTART>=1000) && (status_question("Image larger than 1000x1000 after rotation. Proceed","Yes No",1)==1)) return;
	change_editp_size(roty+ySTART-trueysize, rotx+xSTART-truexsize);
	}
	
	{								// additional undo save for rotate, cause output coords!=block coords
	int endx,endy;
	if (roty+ySTART>yFIN) endy=roty+ySTART; else endy=yFIN;
	if (rotx+xSTART>xFIN) endx=rotx+xSTART; else endx=xFIN;
	save_undo(ySTART,xSTART,endy,endx);
	}

	rotatep=newpad(roty,rotx);
	cursor_up();

	for (x=xSTART;x<=xFIN;x++)
	{
	    for (y=ySTART;y<=yFIN;y++)
	    {
	    mvwaddch(rotatep,roty-x+xSTART-1,y-ySTART,mvwinch(editp,y,x));
	    mvwaddch(editp,y,x,' ' | COLOR_PAIR(0) | A_NORMAL);
	    }
	}

	for (x=0;x<rotx;x++)
	{
	    for (y=0;y<roty;y++)
	    {
	    mvwaddch(editp,ySTART+y,xSTART+x,mvwinch(rotatep,y,x));
	    }
	}

	delwin(rotatep);
	break;
}

cursor_down();
return;
}


// *************************************
//
// void block_copy(void)
// copy the block to clipboard
//
// ************************************

void block_copy(void)
{
int x,y;
cursor_up();

delwin(clipboardp);

clipx=xFIN-xSTART+1;
clipy=yFIN-ySTART+1;

clipboardp = newpad(clipy,clipx);
for (y=ySTART; y<=yFIN; y++)
{
    for (x=xSTART; x<=xFIN; x++)
    {
    mvwaddch(clipboardp,y-ySTART,x-xSTART,mvwinch(editp,y,x));
    }
}
cursor_down();
return;
}


// **********************************
//
// void block_cut(void)
// copy the block to clipboard and earse it in editp
//
// **********************************

void block_cut(void)
{
int x,y;

save_undo(ySTART,xSTART,yFIN,xFIN);
cursor_up();

delwin(clipboardp);
clipx=xFIN-xSTART+1;
clipy=yFIN-ySTART+1;

clipboardp = newpad(clipy,clipx);

for (y=ySTART; y<=yFIN; y++)
{
    for (x=xSTART; x<=xFIN; x++)
    {
    mvwaddch(clipboardp,y-ySTART,x-xSTART,mvwinch(editp,y,x));
    mvwaddch(editp,y,x,' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR);
    }
}
cursor_down();
return;
}

// ****************************
// 
// void paste(void)
// paste the clipboard into editp (acty,actx) is upper left-hand corner
//
// ****************************

void paste(void)
{
int x,y;
unsigned short answer;
chtype character;
chtype defchar = (' ' | COLOR_PAIR(find_pair(DEFCOLOR,DEFBGND)) | DEFATTR); // !!! Trying to defaultize things
// chtype defchar = (' ' | COLOR_PAIR(0) | A_NORMAL);

save_undo(acty,actx,acty+clipy,actx+clipx);

cursor_up();

if (clipx>=0 && clipy>=0)
{    
    if (actx+clipx-1>truexsize || acty+clipy-1>trueysize)
    {
	if ((actx+clipx-1>=1000 || acty+clipy-1>=1000) && (status_question("Image larger than 1000x1000 after pasting. Proceed","Yes No",1)==2)) return;
    change_editp_size(acty+clipy-1-trueysize, actx+clipx-1-truexsize);
    }

answer=status_question("Use transparency","Yes No",1);
    
    for (y=0; y<clipy; y++)
	for (x=0; x<clipx; x++)
	{
//	character=mvwinch(clipboardp,y,x);
	    if (((character=mvwinch(clipboardp,y,x))!=defchar) || (answer==2))
		mvwaddch(editp,acty+y,actx+x,character);
	}
}
else
status_message("Clipboard does not exist!");

cursor_down();
return;
}


// **************************
//
// void block_remove(void)
// removes block selection
//
// **************************

void block_remove(void)
{
blockmode=false;
xSTART=-1;
xFIN=-1;
ySTART=-1;
yFIN=-1;
editprefresh();
update_status();
return;
}

// **************************
//
// void save_undo(int,int,int,int)
// saves data for undo
//
// **************************

void save_undo(int y0, int x0, int yend, int xend)
{

if (y0==-1)
{
undoxsize=-1;
undoysize=-1;
undox=-1;
undoy=-1;
wresize(undop,1,1);
return;
}

int x,y;

if (xend>truexsize) xend=truexsize;
if (yend>trueysize) yend=trueysize;

undoxsize=xend-x0+1;
undoysize=yend-y0+1;
undox=x0;
undoy=y0;

if (undop==NULL) undop=newpad(undoysize,undoxsize);
else wresize(undop,undoysize,undoxsize); 
wclear(undop);

cursor_up();

for (x=x0;x<=xend;x++)
    for (y=y0;y<=yend;y++)
    mvwaddch(undop,y-y0,x-x0,mvwinch(editp,y,x));

cursor_down();

return;
}


// **************************
//
// void do_undo(void)
// undoes last action
//
// **************************

void do_undo(void)
{
int x,y;
chtype character;

if (undox!=-1)
{
cursor_up();

    for (x=0;x<undoxsize;x++)
	for (y=0;y<undoysize;y++)
	{
	character=mvwinch(editp,undoy+y,undox+x);
	mvwaddch(editp,undoy+y,undox+x,mvwinch(undop,y,x));
	mvwaddch(undop,y,x,character);
	}

cursor_down();
}
else status_message("No undo information available!");

return;
}


// ****************************
//
// void block(void)
// select block and block operations menu
//
// ****************************

void block(void)
{
int blockoperation;

if (!blockmode && xSTART==-1)
{
    xSTART=actx;
    ySTART=acty;
    blockmode=true;
    update_status();
    return;
}
else if (xFIN==-1)
{
    xFIN=actx;
    yFIN=acty;
    	if (xFIN==xSTART && yFIN==ySTART)
        {
	block_remove();
	return;
	}
	
	if (xFIN<xSTART)
	{
	int a;
	a=xFIN;
	xFIN=xSTART;
	xSTART=a;
	}
	
	if (yFIN<ySTART)
	{
	int a;
	a=yFIN;
	yFIN=ySTART;
	ySTART=a;
	}
}

do 
{
    blockoperation=status_question("What to do","a.Copy b.Cut c.Clear d.Fill e.DelBlock f.Cancel g.More-->",1);

    	    switch(blockoperation)
	    {
	    case 1:
		block_copy();
		break;
	    case 2:
		block_cut();
		break;
	    case 3:
		block_clear();
		break;
	    case 4:
		block_fill();
		break;
	    case 5:
		block_remove();		
		break;
	    case 7:
	    blockoperation=status_question("","a.Colorize b.Rotate c.MirrorH d.MirrorV e.DrawBox f.Cancel g.Previous-->",1);

		switch(blockoperation)
		{
		case 1:
		    block_colorize(); 
		    break;
		case 2:
		    block_rotate();
		    break;
		case 3:
		    block_mirrorH();
		    break;
		case 4:
		    block_mirrorV();
		    break;
		case 5:
		    block_drawbox();
		    break;
		}
	    break;
	    }
} while (blockoperation==7);

return;
}

// ************************************
//
// void block_windows_remove(void)
// remove all block  pads (undo,clipboard)
//
// ************************************

void block_windows_remove(void)
{
delwin(clipboardp);
delwin(undop);
return;
}
