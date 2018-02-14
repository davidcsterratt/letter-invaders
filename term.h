/*
 * header file to be used with term.c.  defines some macros to place the
 * cursor in various places on the screen.
 *
 * Larry Moss (lm03_cif@uhura.cc.rochester.edu)
 */

#ifdef __TURBOC__
# include <conio.h>

# define putp(str)	cputs(str)
# define HAS_CAP(str)	(*str)
# define clrdisp()	clrscr()
# define home()		putp(cursor_home)
# define goto_xy(c, l)	gotoxy((c)+1, (l)+1)

#else

# define putp(str)	tputs(str, 0, outc)
# define HAS_CAP(str)	(*str)
# define clrdisp()	tputs(clear_screen, Lines, outc)
# define home()		putp(cursor_home)
# define goto_xy(c, l)	putp(tgoto(cursor_address, c, l))

#endif

#define SP ' '
