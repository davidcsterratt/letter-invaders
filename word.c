/*
 * find a random word in a dictionary file as part of letters.
 *
 * copyright 1991 Larry Moss (lm03_cif@uhura.cc.rochester.edu)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "kinput.h"
#include "turboc.h"

#ifdef __TURBOC__
# include <conio.h>
# include <stdlib.h>
# define random rand
#endif

#ifdef SYSV
# define random lrand48
#endif

extern char *dictionary, *choice;
extern int choicelen;

char *getword()
{
	static char	buf[512];
	static FILE	*fp = NULL;
	static struct stat	s_buf;

	/*
	 * if there's a 'choice' string set, choose a selection of letters
	 * from it instead of reading the dictionary.
	 */
	if (choice) {

		char *start, *p = buf;
		int   wlen;

		start = choice + (random() % choicelen);
		wlen  = (MINSTRING + (random() % (MAXSTRING - MINSTRING))) 
		      % sizeof(buf);
		while(wlen--) {
			if (*start == '\0')
				start = choice;
			*p++ = *start++;
		}
		*p = '\0';
		return buf;
	}

	/*
	 * This is stuff that only needs to get done once.
	 */
	if(fp == NULL) {
		/*
		 * open the dictionary file
		 */
		if((fp = fopen(dictionary, "r")) == NULL) {
			fprintf(stderr, "can't open file: %s.\n", dictionary);
			setterm(ORIG);
			textattr_clr;
			exit(1);
		}
		
		/*
		 * Get length of dictionary in bytes so we can pick a
		 * random entry in it.
		 */
		if(stat(dictionary, &s_buf) == -1) {
			perror("stat");
			setterm(ORIG);
			textattr_clr;
			exit(1);
		}
	}
	
	/*
	 * pick a random place in the dictionary
	 */
#ifdef __TURBOC__
 	fseek(fp, ((((long)rand() << 16) | rand())) % s_buf.st_size, 0);
#else
	fseek(fp, random() % s_buf.st_size, 0);
#endif

	/*
	 * read until the end of a line, then read the next word.
	 */
	fscanf(fp, "%*s%s", buf);

	/*
	 * Since we're reading two words at a time it's possible to go past
	 * the end of the file.  If that happens, use the first word in the
	 * dictionary.
	 */
	if(buf == NULL) {
		fseek(fp, 0L, 0);
		fscanf(fp, "%s", buf);
	}

	return buf;
}


char *bonusword()
{
	static char	buf[BONUSLENGTH + 1];
	int		i;

	for(i = 0; i < BONUSLENGTH; i++)
		buf[i] = choice ? *(choice + (random() % choicelen))
				: (char)(random() % 94) + 33;

	buf[BONUSLENGTH] = 0;

	return buf;
}
