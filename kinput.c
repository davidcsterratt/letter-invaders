/*
 * do non-blocking keyboard reads
 *
 * copyright 1991 by Larry Moss (lm03_cif@uhura.cc.rochester.edu)
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include "config.h"
#ifdef __TURBOC__
# include <stdlib.h>
# include <conio.h>
#else
	/* I know some systems prefer <time.h>, but I'm not sure which */
# include <sys/time.h>
#endif
#ifdef SYSV
# include <termio.h>
#else  /* SYSV */
# ifndef __TURBOC__
#  include <sgtty.h>
# endif /* __TURBOC__ */
#endif /* SYSV */

#include "kinput.h"
#ifndef __TURBOC__
# include "terms.h"
#endif
#include "term.h"
#include "turboc.h"

extern unsigned int score, word_count, level;

#ifndef __TURBOC__
int key_pressed();
#endif

static void intrrpt();

#ifndef NOJOB
static void pause();
static void cont();
#endif

static void die();

/*
 * This function will return -1 if no key is available, or the key
 * that was pressed by the user.  It is checking stdin, without blocking.
 */
int key_pressed()
{
#ifdef SYSV
	int		chars_read;
	static char	keypressed;

	chars_read = read(0, &keypressed, 1);
	if (chars_read == 1)
		return((int)keypressed);
	return(-1);
#elif __TURBOC__
	if (kbhit())
		return getch();  /* don't echo */
	else
		return -1;
#else /* SYSV */
	int		mask = 1, chars_read;
	static char	keypressed;
	struct timeval	waittime;

	waittime.tv_sec=0;
#ifdef SYSV2
	waittime.tv_usec=PAUSE;
#else
	waittime.tv_usec=4;
#endif
	if (select(1, &mask, 0, 0, &waittime)) {
		chars_read = read(0, &keypressed, 1);
		if (chars_read == 1)
			return((int)keypressed);
	}
	return(-1);
#endif /* SYSV */
}

/*
 * Set the terminal to cbreak mode, turn off echo and prevent special
 * characters from affecting the input.  We will handle EOF and other fun
 * stuff our own way.  Backup copies of the current setup will be kept
 * to insure the terminal gets returned to its initial state.
 */
void setterm(setting)
int setting;
{
#ifndef __TURBOC__
# ifdef SYSV
	struct termio	termrec;
	static struct termio	old_termrec;
# else  /* SYSV */
	struct sgttyb	termrec;
	struct tchars	trec;
	static struct tchars	old_trec;
	static struct sgttyb	old_termrec;
# endif /* SYSV */
#endif /* __TURBOC__ */
	if(setting == NEW) {
		signal(SIGINT, intrrpt);
#ifndef __TURBOC__
		signal(SIGQUIT, die);
#endif
		signal(SIGTERM, die);
#ifndef NOJOB
		signal(SIGTSTP, pause);
		signal(SIGCONT, cont);
#endif /* NOJOB */
#ifdef SYSV
		ioctl(0, TCGETA, &termrec);
		old_termrec = termrec;
		termrec.c_iflag &= ~(IGNPAR|PARMRK|INLCR|IGNCR|ICRNL);
		termrec.c_iflag |= BRKINT;
		termrec.c_lflag &= ~(ICANON|ECHO);
#ifdef SYSV2
		termrec.c_cc[VTIME] = PAUSE / 100000;
#else
		termrec.c_cc[VTIME] = 0;
#endif
		termrec.c_cc[VMIN] = 0;
		ioctl(0, TCSETAF, &termrec);
#elif !defined(__TURBOC__)
		ioctl(0, TIOCGETP, &termrec);
		old_termrec = termrec;
		termrec.sg_flags |= CBREAK;
		termrec.sg_flags &= ~ECHO;
		ioctl(0, TIOCSETP, &termrec);

		ioctl(0, TIOCGETC, &trec);
		old_trec = trec;
		trec.t_eofc = (char) -1;
		trec.t_quitc = (char) -1;
		ioctl(0, TIOCSETC, &trec);
#endif /* SYSV */
	} else {
		signal(SIGINT, SIG_DFL);
#ifndef __TURBOC__
		signal(SIGQUIT, SIG_DFL);
#endif
		signal(SIGTERM, SIG_DFL);
#ifndef NOJOB
		signal(SIGTSTP, SIG_DFL);
#endif  /* NOJOB */

#ifdef SYSV
		ioctl(0, TCSETAF, &old_termrec);
#elif !defined(__TURBOC__)
		ioctl(0, TIOCSETP, &old_termrec);
		ioctl(0, TIOCSETC, &old_trec);
# endif /* SYSV */
	}
}


/*
 * Interrupt handlers
 */

#ifndef NOJOB
static void pause(sig)
int	sig;
{
#ifdef SYSV
	signal(sig, intrrpt);
#endif /* SYSV */
	goto_xy(0, SCREENLENGTH + 1);
	setterm(ORIG);
	putchar('\n');
	kill(getpid(), SIGSTOP);
}

static void cont(sig)
int	sig;
{
#ifdef SYSV
	signal(sig, intrrpt);
#endif /* SYSV */
	setterm(NEW);
	redraw();
}

#endif /* NOJOB */

static void intrrpt(sig)
int	sig;
{
	char	c;

	setterm(ORIG);
#ifdef __TURBOC__
	clrscr();
	cputs("are you sure you want to quit? ");
	if((c = getch()) == 'y' || c == 'Y') {
		textattr_clr;
		printf("\n\nfinal: score = %u\twords = %u\t level = %d\n",
		       score, word_count, level);
		exit(1);
#else
	printf("\n\rare you sure you want to quit? ");
	if((c = getchar()) == 'y' || c == 'Y') {
		clrdisp();
		printf("\n\nfinal: score = %u\twords = %u\t level = %d\n",
		       score, word_count, level);
		highlight(0);
		exit(1);
#endif
	} else {
#ifdef SYSV
		signal(sig, intrrpt);
#endif /* SYSV */
		setterm(NEW);
		redraw();
	}
}

static void die(sig)
int	sig;
{
	textattr_clr;
#ifndef __TURBOC__
	setterm(ORIG);
	clrdisp();
	highlight(0);
	exit(1);
#endif
}
