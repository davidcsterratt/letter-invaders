	/*
	 * subseconds sleeps for System V - or anything that has poll() Don
	 * Libes, 4/1/1991
	 * 
	 * The BSD analog to this function is defined in terms of microseconds
	 * while poll() is defined in terms of milliseconds.  For
	 * compatibility, this function provides accuracy "over the long run"
	 * by truncating actual requests to milliseconds and accumulating
	 * microseconds across calls with the idea that you are probably
	 * calling it in a tight loop, and that over the long run, the error
	 * will even out.
	 * 
	 * If you aren't calling it in a tight loop, then you almost certainly
	 * aren't making microsecond-resolution requests anyway, in which
	 * case you don't care about microseconds.  And if you did, you
	 * wouldn't be using UNIX anyway because random system indigestion
	 * (i.e., scheduling) can make mincemeat out of any timing code.
	 * 
	 * Returns 0 if successful timeout, -1 if unsuccessful.
	 * 
	 */

#include <poll.h>

	int
	                usleep(usec)
	unsigned int    usec;	/* microseconds */
	{
		static          subtotal = 0;	/* microseconds */
		int             msec;	/* milliseconds */

		/*
		 * 'foo' is only here because some versions of 5.3 have a bug
		 * where the first argument to poll() is checked for a valid
		 * memory address even if the second argument is 0.
		 */
		struct pollfd   foo;

		subtotal += usec;
		/* if less then 1 msec request, do nothing but remember it */
		if (subtotal < 1000)
			return (0);
		msec = subtotal / 1000;
		subtotal = subtotal % 1000;
		return poll(&foo, (unsigned long) 0, msec);
	}
