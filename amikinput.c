/*
 * do non-blocking keyboard reads (and other Amiga stuff)
 *
 * created for Amiga by Brent J. Nordquist (brent@limabean.veggard.mn.org)
 * with a lot of help from the RKM!
 */

#include <stdio.h>
#include <ios1.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <exec/types.h>
#include <devices/timer.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <workbench/startup.h>

#include "config.h"
#include "kinput.h"
#include "terms.h"
#include "term.h"

extern unsigned int score;

int key_pressed();

/*
 * This function will return -1 if no key is available, or the key
 * that was pressed by the user.  It is checking stdin, without blocking.
 */
int key_pressed()
{
	struct UFB	*in;

	if(!(in=chkufb(0)))
		return(-1);
	if(WaitForChar(in->ufbfh,1))
		return(getchar());
	else
		return(-1);
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
	void		interhdlr();
	static void	(*oldint)();
	static int	curset= -1;

	if(setting==curset)
		return;
	else if(setting==NEW) {
		oldint=signal(SIGINT, interhdlr);
		raw(stdin);
	} else {
		signal(SIGINT, oldint);
		cooked(stdin);
		unfixedwin();
	}
}

void interhdlr()
{
	char	c;

	signal(SIGINT, interhdlr);
	gotoxy(0, SCREENLENGTH);
	highlight(1);
	printf("Are you sure you want to quit?      \b\b\b\b\b");
	highlight(0);
	fflush(stdout);
	while(!isprint(c = getchar()))
		;
	putchar(c);
	fflush(stdout);
	if(c == 'y' || c == 'Y') {
		clrdisp();
		printf("\n\nfinal score = %u\n", score);
		highlight(0);
		setterm(ORIG);
		exit(1);
	}
	redraw();
	while(key_pressed()>=0)
		;
}

int 	sleep(sec)
int 	sec;
{
	Delay(sec*50);
	return(0);
}

int             usleep(usec)	/* returns 0 if ok, else -1 */
long            usec;		/* delay in microseconds */
{
	struct MsgPort		*reply,*timerport;
	struct timerequest	*timermsg;

	if(usec<2)
		return(-1);
	if(!(timerport=CreatePort(0,0)))
		return(-1);
	if(!(timermsg=(struct timerequest *)CreateExtIO(timerport,
	    sizeof(struct timerequest))))
		return(-1);
	if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)timermsg,
	    0L)!=0) {
		DeleteExtIO((struct IORequest *)timermsg);
		return(-1);
	}
	timermsg->tr_node.io_Command=TR_ADDREQUEST;
	timermsg->tr_time.tv_secs=usec/1000000L;
	timermsg->tr_time.tv_micro=usec%1000000L;
	DoIO((struct IORequest *)timermsg);
	if((reply=timermsg->tr_node.io_Message.mn_ReplyPort))
		DeletePort(reply);
	CloseDevice((struct IORequest *)timermsg);
	DeleteExtIO((struct IORequest *)timermsg);
	return(0);
}

int 	getpid()
{
	static long	t=0;
	if(!t)
		time(&t);
	return((int)(t%32768));
}

static struct Process	*process;
static struct UFB    	 o_ufb[3];
static APTR    	    	 otask=NULL;

int 	 fixedwin(argc,progname)
int 	 argc;
char	*progname;
{
	struct FileHandle	*handle;
	struct UFB    	    	*ufbp[3];
	char    	    	 wtitle[66];
	int 	    	    	 i;

	for(i=0;i<3;i++) {
		if(!(ufbp[i]=chkufb(i)))
			return(-1);
		o_ufb[i]= *(ufbp[i]);
	}
	strcpy(wtitle,"CON:0/0/640/200/");
	strncat(wtitle,progname,40);
	strcat(wtitle,"/NOSIZE");
	ufbp[0]->ufbfh=Open(wtitle,MODE_NEWFILE);
	ufbp[0]->ufbflg&=UFB_NC;
	ufbp[1]->ufbfh=ufbp[0]->ufbfh;
	if(argc==0)             /* running under workbench      */
		ufbp[2]->ufbfh=ufbp[0]->ufbfh;
	handle=(struct FileHandle *)(ufbp[0]->ufbfh << 2);
	process=(struct Process *)FindTask(0);
	if(argc!=0)             /* running under CLI            */
		otask=process->pr_ConsoleTask;
	process->pr_ConsoleTask=(APTR)handle->fh_Type;
	ufbp[0]->ufbflg |= UFB_RA | O_RAW;
	ufbp[1]->ufbflg |= UFB_WA | O_RAW;
	ufbp[2]->ufbflg |= UFB_RA | UFB_WA | O_RAW;
	return(0);
}

int 	unfixedwin()
{
	struct UFB    	    	*ufbp[3];
	int 	    	    	 i;

	fflush(stdout);
	fflush(stderr);
	sleep(2);
	if(otask)               /* running under CLI            */
		process->pr_ConsoleTask=otask;
	for(i=0;i<3;i++) {
		if(!(ufbp[i]=chkufb(i)))
			return(-1);
		if(!i)
			Close(ufbp[i]->ufbfh);
		*(ufbp[i])=o_ufb[i];
	}
	return(0);
}
