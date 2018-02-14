/*
 * this program will attempt to draw random stuff on the screen by using the
 * termcap library.  Much of this code was evolved from term.c from the nn
 * sources.
 *
 * Larry Moss (lm03_cif@uhura.cc.rochester.edu)
 * Kim Storm deserves a fair amount of credit for this.  It may not look
 *   too much like his code, but his code was at least more understandable
 *   than the documentation I was trying to work from and I'd like to give
 *   credit for making it easier for me to write.
 *
 * I wrote a fair amount of this based on man pages, then used Kim's code as
 * a reference to make things work.  Some of this actually ended up looking
 * more like his code than I planned on.  I hope I haven't gone overboard.
 */

#include <stdio.h>
#ifndef NEXT
# ifdef AMIGA
#  include <stdlib.h>
#  include "amitermcap.h"
# elif __TURBOC__
#  include <conio.h>
# else
#  include <termio.h>
# endif
#endif
#include <string.h>
#include "term.h"

char	*tgoto();
char	PC, *BC, *UP;

char		*term_name;
char	XBC[64], XUP[64];
char	bell_str[256] = "\007";
char     cursor_home[64];
char     clear_screen[64];
char     cursor_address[128];
char     enter_standout_mode[64], exit_standout_mode[64];
char     enter_underline_mode[64], exit_underline_mode[64];

outc(c)
{
	putchar(c);
}

bell() {
#ifdef __TURBOC__
	sound(440);
	delay(500);
	nosound();
#else
	putp(bell_str);
#endif
}

quiet() {
}

#ifndef __TURBOC__
int Lines, Columns;	/* screen size */
int garbage_size;	/* number of garbage chars left from so */
int double_garbage;	/* space needed to enter&exit standout mode */
int STANDOUT;		/* terminal got standout mode */
int TWRAP;		/* terminal got automatic margins */

/*
 * used to get the actual terminal control string.
 */
opt_cap(cap, buf)
char           *cap, *buf;
{
	char	*tgetstr();

	*buf = '\0';
	return tgetstr(cap, &buf) != NULL;
}

/*
 * call opt_cap to get control string.  report if the terminal lacks that
 * capability.
 */
get_cap(cap, buf)
char *cap, *buf;
{
	if (!opt_cap(cap, buf))
		fprintf(stderr, "TERMCAP entry for %s has no '%s' capability\n",
			term_name, cap);
}

#endif /* __TURBOC__ */

/*
 * set everythign up.  find the necessary strings to control the terminal.
 */
init_term()
{
#ifdef __TURBOC__
	highlight(0);
	clrscr();
#else
	char            tbuf[1024];

#ifndef AMIGA
	/*
	 * get terminal type from the environment or have the user enter it
	 */
	if ((term_name = (char *)getenv("TERM")) == NULL) {
		fprintf(stderr, "No TERM variable in environment\n");
		fprintf(stderr, "Enter terminal type to use: ");
		scanf("%s", term_name = (char *)malloc(30 * sizeof(char)));
	}
#endif

	/*
	 * get the termcap entry for the terminal above
	 */
	if (tgetent(tbuf, term_name) <= 0) {
		fprintf(stderr, "Unknown terminal type: %s\n", term_name);
		exit(1);
	}

	/*
	 * get the padding character for the terminal
	 */
	opt_cap("pc", cursor_address);	/* temp. usage */
	PC = cursor_address[0];

	get_cap("cm", cursor_address);
	if (!opt_cap("ho", cursor_home))
		strcpy(cursor_home, tgoto(cursor_address, 0, 0));

	get_cap("cl", clear_screen);

	Lines = tgetnum("li");
	Columns = tgetnum("co");

	opt_cap("so", enter_standout_mode);
	opt_cap("se", exit_standout_mode);

	opt_cap("us", enter_underline_mode);
	opt_cap("ue", exit_underline_mode);

	garbage_size = tgetnum("sg");

	TWRAP = tgetflag("am");

	STANDOUT = HAS_CAP(enter_standout_mode);
	if (STANDOUT) {
		if (garbage_size < 0)
			garbage_size = 0;
		double_garbage = 2 * garbage_size;
	} else
		garbage_size = double_garbage = 0;
#endif /* __TURBOC__ */
}

underline(on)
{
#ifdef __TURBOC__
	return 0;
#else
	if (garbage_size)
		return 0;
	if (!HAS_CAP(enter_underline_mode))
		return 0;
	putp(on ? enter_underline_mode : exit_underline_mode);
	return 1;
#endif /* __TURBOC__ */
}

highlight(on)
{
#ifdef __TURBOC__

	if (on)
		textattr(0x4f); /* highlight on: white on red */
	else
		textattr(0x71); /* highlight off: blue on light gray */

#else
	if (garbage_size)
		return 0;
	if (!HAS_CAP(enter_standout_mode))
		return 0;
	putp(on ? enter_standout_mode : exit_standout_mode);
#endif /* __TURBOC__ */
	return 1;
}
