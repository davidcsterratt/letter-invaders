/*
 * header file for term.c
 */

extern char	*tgoto();
extern char	PC, *BC, *UP;

extern char	*term_name;
extern char	XBC[], XUP[];
extern char	bell_str[];
extern char     *cursor_home;
extern char     clear_screen[];
extern char     cursor_address[];
extern char     enter_standout_mode[], exit_standout_mode[];
extern char     enter_underline_mode[], exit_underline_mode[];

extern outc();
extern bell();
extern quiet();

extern int Lines, Columns;	/* screen size */
extern int garbage_size;	/* number of garbage chars left from so */
extern int double_garbage;	/* space needed to enter&exit standout mode */
extern int STANDOUT;		/* terminal got standout mode */
extern int TWRAP;		/* terminal got automatic margins */
