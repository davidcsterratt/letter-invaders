/*
 * read and update high score file for Letter Invaders
 *
 * copyright 1991 by Larry Moss (lm03_cif@uhura.cc.rochester.edu)
 */

#ifdef __TURBOC__
# define AMIGA __TURBOC__
# define printf cprintf
# define fputs(A,B) cputs(A)
#endif

#include <stdio.h>
#ifdef AMIGA
# include <stdlib.h>
# include <string.h>
#else
# include <pwd.h>
#endif /* AMIGA */
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "term.h"
#ifndef __TURBOC__
# include "terms.h"
#endif
#include "kinput.h"
#include "turboc.h"

struct score_rec {
	char	name[9];
	int	level, words, score;
};

static char highscores[] = HIGHSCORES;

extern unsigned score, word_count, level;

static struct score_rec high_scores[10];
static struct stat	s_buf;
time_t	readtime;

void read_scores() {
	int		 i;
	FILE		 *fp;

	/*
	 * get the last modified time so we know later if we have to reread
	 * the scores before saving a new file.  Only important on
	 * multiuser systems.
	 */
	if(stat(highscores, &s_buf) == -1) {
		textattr_clr;
		fprintf(stderr, "Cannot stat %s: ", highscores);
		perror("");
		setterm(ORIG);
		exit(1);
	}

	readtime = s_buf.st_mtime;

	if((fp = fopen(highscores, "r")) == NULL) {
		textattr_clr;
		fprintf(stderr, "Cannot open %s: ", highscores);
		perror("");
		setterm(ORIG);
		exit(1);
	}

	for(i = 0; i < 10; i++) {
		fscanf(fp, "%s%d%d%d", high_scores[i].name,
		       &high_scores[i].level, &high_scores[i].words,
		       &high_scores[i].score);
	}

	fclose(fp);
}

int write_scores() {
	int	i;
	FILE	*fp;

	/*
	 * check to make sure the high score list has not been modified
	 * since we read it.
	 */
	if(stat(highscores, &s_buf) == -1) {
		textattr_clr;
		fprintf(stderr, "Cannot stat %s: ", highscores);
		perror("");
		setterm(ORIG);
		exit(1);
	}

	if(s_buf.st_mtime > readtime)
		return -1;

	if((fp = fopen(highscores, "w")) == NULL) {
		textattr_clr;
		fprintf(stderr, "Cannot write to %s: ", highscores);
		perror("");
		setterm(ORIG);
		exit(1);
	}

	for(i = 0; i < 10; i++) {
		fprintf(fp, "%s %d %d %d", high_scores[i].name,
		       high_scores[i].level, high_scores[i].words,
		       high_scores[i].score);
	}

	fclose(fp);
}



void update_scores() {
	int i, j;
#ifdef AMIGA
	char *u;
#else
	struct passwd *p;
#endif

	for(i = 0; i < 10; i++)
		if(score > high_scores[i].score) {
			for(j = 10; j > i; j--) {
				strcpy(high_scores[j].name, high_scores[j-1].name);
				high_scores[j].words = high_scores[j-1].words;
				high_scores[j].score = high_scores[j-1].score;
				high_scores[j].level = high_scores[j-1].level;
			}
#ifdef AMIGA
                        if((u = getenv("USER")) == NULL) {
				char buffer[128],*p;
				clrdisp();
				goto_xy(18, 5);
#ifndef __TURBOC__
				cooked(stdin);
				fputs("Enter your name (8 char. max, no spaces): ",stdout);
				fflush(stdout);
				gets(buffer);
				if((p=strchr(buffer,' ')))
					*p='\0';
				if(!buffer[0])
					strcpy(buffer,"nobody");
				strncpy(high_scores[i].name, buffer, 8);
				raw(stdin);
#else  /* not __TURBOC__ */
				cputs("Enter your name (8 char. max, no spaces): ");
				buffer[0] = 8;
				cgets(buffer);
				if((p=strchr(buffer+2,' ')))
					*p='\0';
				if(!buffer[2])
					strcpy(buffer+2,"nobody");
				strncpy(high_scores[i].name, buffer+2, 8);
#endif  /* __TURBOC__ */
                        } else
                                strcpy(high_scores[i].name, u);
#else  /* if not AMIGA */
			if((p = getpwuid(getuid())) == NULL)
                                strcpy(high_scores[i].name, "nobody");
			else
				strcpy(high_scores[i].name, p->pw_name);
#endif /* AMIGA */
			high_scores[i].score = score;
			high_scores[i].words = word_count;
			high_scores[i].level = level;
			if(write_scores() == -1) {
				read_scores();
				update_scores();
			}
			break;
		}
}

void show_scores() {
	int i;

	highlight(0);
	clrdisp();
	goto_xy(18, 5);
	highlight(1);
	printf("Top Ten Scores for Letter Invaders");
	highlight(0);
	goto_xy(20, 7);
	underline(1);
	printf("  name      level words score");
	underline(0);
	
	for(i = 0; i < 10; i++) {
		goto_xy(18, 8 + i);
		printf("%3d %-10s%5d%6d%6d", i+1, high_scores[i].name,
		       high_scores[i].level, high_scores[i].words,
		       high_scores[i].score);
	}

	printf("\n");
#ifdef AMIGA
	{
		char bogus;
		fputs("\nPress RETURN... ",stdout);
		fflush(stdout);
		bogus=getchar();
	}
#endif
}
