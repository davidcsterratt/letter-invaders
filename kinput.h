/*
 * header file for:
 * kinput.c: Larry Moss (lm03_cif@cc.rochester.edu)
 *	    Fall, 1990
 *
 */

#define BACKSPACE	fputs("\b \b", stdout)
#define DEL		127
#define WERASE		23
#define KILL		21
#define TAB		9
#define	ESC		27
#define CTL(c)		(c & 037)
#define MAXLENGTH	1024
#define NEW		1
#define ORIG		0

void setterm();
