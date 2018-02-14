/*
 * configurable stuff in letters.  Most things here probably shouldn't need
 * to be changed but are here because someone may want to tinker with this
 * stuff to affect the way it performs on different systems.  The stuff
 * most likely to require changes is at the top of the file.
 */
#ifndef DICTIONARY
#define DICTIONARY "/usr/dict/words"
#endif

#ifndef HIGHSCORES
#define HIGHSCORES "letters.high"
#endif

#ifdef SYSV2
# define SYSV
#endif

/*
 * probably best to leave these so it's the same everywhere.  Otherwise,
 * anyone with an xterminal is likely to get higher scores.
 */
#ifdef AMIGA
#define SCREENLENGTH	22
#elif __TURBOC__
#define SCREENLENGTH	24
#else
#define SCREENLENGTH	23
#endif

#ifdef __TURBOC__
#define SCREENWIDTH	79
#else
#define SCREENWIDTH	80
#endif

/*
 * initial delay in usecs before words move to the next line
 */
#define START_DELAY	750000

/*
 * this implements "graduated" (non-linear) decreasing delay times:
 * each level, delay gets reduced by smaller and smaller amounts
 * (eventually, when delay would get below PAUSE, it is simply set to PAUSE)
 *
 * if you change START_DELAY or DELAY_CHANGE, DECEL must be tuned carefully,
 * otherwise DELAY(lev) will drop suddenly to PAUSE at some point
 */
#define DELAY_CHANGE	60000
#define DECEL		1200
#define DELAY(lev)	( (((long)(lev))*DECEL > DELAY_CHANGE/2) ? PAUSE :\
			  (START_DELAY-(DELAY_CHANGE-(lev)*DECEL)*(lev)) )

/*
 * number of words to be completed before level change
 */
#define LEVEL_CHANGE	15

/*
 * length of pause before reading keyboard again (in usecs).  There has to
 * be some pause.
 */
#ifdef SYSV2
# define PAUSE		100000
#else
# define PAUSE		10000
#endif

/*
 * This is how likely it is that another word will appear on the screen
 * while words are falling.  there isa 1/ADDWORD chance of a new word
 */
#define ADDWORD		6

/*
 * length of words in bonus round
 */
#define BONUSLENGTH	10

/* 
 * minimum and maximum length of character strings chosen from
 * -s argument
 */

#define MINSTRING	3
#define MAXSTRING	8

