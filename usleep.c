/*
 * I grabbed this from the FAQ liston comp.unix.questions
 */
/*
 * usleep -- support routine for 4.2BSD system call emulations
 * 
 * last edit:	29-Oct-1984	D A Gwyn
 */

extern int      select();


int             usleep(usec)	/* returns 0 if ok, else -1 */
long            usec;		/* delay in microseconds */
{
	static struct {		/* `timeval' */
		long            tv_sec;	/* seconds */
		long            tv_usec;	/* microsecs */
	}               delay;	/* _select() timeout */

	delay.tv_sec = usec / 1000000L;
	delay.tv_usec = usec % 1000000L;

	return select(0, (long *) 0, (long *) 0, (long *) 0, &delay);
}
