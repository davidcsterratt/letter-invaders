/**** 
 * raw.c	(Borrowed from UUPC)
 *
 * This is a routine for setting a given stream to raw or cooked mode.
 * This is useful when you are using Lattice C to produce programs that
 * want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis. 
 * 	     If you use it I would appreciate credit for it somewhere.
 */
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ios1.h>
#include <error.h>
#include <proto/exec.h>
#include <proto/dos.h>

extern	int	errno;		/* The error variable */

/*
 * Function raw() - Convert the specified file pointer to 'raw' mode. This
 * only works on TTY's and essentially keeps DOS from translating keys for
 * you, also (BIG WIN) it means getch() will return immediately rather than
 * wait for a return. You lose editing features though.
 */
long
raw(fp)
FILE *fp;
{
  struct MsgPort 	*mp; /* The File Handle message port */
  struct FileHandle 	*afh;
  struct UFB 		*ufb;
  long			Arg[1],res;

  ufb = (struct UFB *) chkufb(fileno(fp));  /* Step one, get the file handle */
  afh = (struct FileHandle *)(ufb->ufbfh); 

  if (!IsInteractive((BPTR)afh)) {  /* Step 2, check to see if it's a console */
    errno = ENOTTY;
    return(-1);
  }
                              /* Step three, get it's message port. */
  mp  = ((struct FileHandle *)(BADDR(afh)))->fh_Type;
  Arg[0] = -1L;
  res = SendPacket(mp,ACTION_SCREEN_MODE,Arg,1); /* Put it in RAW: mode */
  if (res == 0) {
    errno = ENXIO;
    return(-1);
  }
  return(0);
}

/*
 * Function - cooked() this function returns the designate file pointer to
 * it's normal, wait for a <CR> mode. This is exactly like raw() except that
 * it sends a 0 to the console to make it back into a CON: from a RAW:
 */

long
cooked(fp)
FILE *fp;
{
  struct MsgPort 	*mp; /* The File Handle message port */
  struct FileHandle 	*afh;
  struct UFB 		*ufb;
  long			Arg[1],res;

  ufb = (struct UFB *) chkufb(fileno(fp));
  afh = (struct FileHandle *)(ufb->ufbfh);
  if ( ! IsInteractive((BPTR)afh)) {
    errno = ENOTTY;
    return(-1);
  }
  mp  = ((struct FileHandle *)(BADDR(afh)))->fh_Type;
  Arg[0] = 0;
  res = SendPacket(mp,ACTION_SCREEN_MODE,Arg,1);
  if (res == 0) {
    errno = ENXIO;
    return(-1);
  }
  return(0);
}

/****
 * Sendpacket.c 
 *
 * An invaluable addition to your Amiga.lib file. This code sends a packet
 * the given message port. This makes working around DOS lots easier.
 * 
 * Note, I didn't write this, those wonderful folks at CBM did. I do suggest
 * however that you may wish to add it to Amiga.Lib, to do so, compile it
 * and say 'oml lib:amiga.lib -r sendpacket.o' 
 */

/*
 * Function - SendPacket written by Phil Lindsay, Carolyn Scheppner, and
 * Andy Finkel. This function will send a packet of the given type to the
 * Message Port supplied.
 */

long
SendPacket(pid,action,args,nargs)
struct MsgPort *pid;  /* process indentifier ... (handlers message port ) */
long action,          /* packet type ... (what you want handler to do )   */
     args[],          /* a pointer to a argument list */
     nargs;           /* number of arguments in list  */
{
  struct MsgPort        *replyport;
  struct StandardPacket *packet;
 
  long  count, *pargs, res1;

  replyport = (struct MsgPort *) CreatePort(NULL,0);
  if(!replyport) return(0);

  /* Allocate space for a packet, make it public and clear it */
  packet = (struct StandardPacket *) 
    AllocMem((long)sizeof(struct StandardPacket),MEMF_PUBLIC|MEMF_CLEAR);
  if(!packet) {
    DeletePort(replyport);
    return(0);
  }

  packet->sp_Msg.mn_Node.ln_Name = (char *)&(packet->sp_Pkt);
  packet->sp_Pkt.dp_Link         = &(packet->sp_Msg);
  packet->sp_Pkt.dp_Port         = replyport;
  packet->sp_Pkt.dp_Type         = action;

  /* copy the args into the packet */
  pargs = &(packet->sp_Pkt.dp_Arg1);       /* address of first argument */
  for(count=0;count < nargs;count++) 
    pargs[count]=args[count];
 
  PutMsg(pid,(struct Message *)packet); /* send packet */

  WaitPort(replyport);
  GetMsg(replyport); 

  res1 = packet->sp_Pkt.dp_Res1;

  FreeMem(packet,(long)sizeof(struct StandardPacket));
  DeletePort(replyport); 

  return(res1);
}

