/*++
/* NAME
/*	amitermcap.h 5
/* SUMMARY
/*	internal type declarations for termcap functions, ported to the
/*	Amiga
/* PROJECT
/*	ms-dos/unix compatibility
/* PACKAGE
/*	termcap
/* SYNOPSIS
/*	#include "amitermcap.h"
/* DESCRIPTION
/*	amitermcap.h contains declarations for internal use of the Amiga
/*	implementation of the termcap(3) functions
/* .nf

/* /* function types */

extern tgetent();
extern tgetnum();
extern tgetflag();
extern char *tgetstr();
extern char *tgoto();
extern tputs();

/* /* static storage for pc console capabilities */

typedef struct {
    char *name;
    char *cap;
} Cap;

extern Cap _console[];
/* SEE ALSO
/*	termcap(3), Berkeley extensions to UNIX.
/* AUTHOR(S)
/*	W.Z. Venema
/*	Eindhoven University of Technology
/*	Department of Mathematics and Computer Science
/*	Den Dolech 2, P.O. Box 513, 5600 MB Eindhoven, The Netherlands
/*
/*	Ported to the Amiga by Brent J. Nordquist
/*	(brent@limabean.veggard.mn.org)
/*--*/
