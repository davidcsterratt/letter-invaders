/*
 * I don't know what this does, but it was scattered throughout the turbo C
 * port with ifdefs surrounding it and I wanted to isolate it.  This code
 * really aught to get cleaned up.
 */
#ifdef __TURBOC__
#define textattr_clr;	textattr(0x07);	clrscr();
#else
#define textattr_clr
#endif
