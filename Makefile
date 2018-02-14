#
# Makefile for letters, a game to help improve typing skills
#
# copyright 1991 by Larry Moss (lm03_cif@uhura.cc.rochester.edu)
#

#CC = cc
CC = gcc

SYSTYPE = SYSV
#SYSTYPE = NEXT
#SYSTYPE = BSD
#SYSTYPE = SYSV2

# if you don't have job control add -DNOJOB to CFLAGS.
CFLAGS = -O -D$(SYSTYPE) -DHIGHSCORES=\"$(LIBDIR)/letters.high\" \
	-DDICTIONARY=\"$(DICTIONARY)\"

LDFLAGS = -ltermcap

BINDIR = /usr/local/bin
MANDIR = /usr/local/man/man$(MANEXT)
MANEXT = 6
LIBDIR = /usr/local/lib
DICTIONARY = /usr/dict/words
#DICTIONARY = dictfile
INSTALL = /usr/ucb/install

# next line only needed if you need to create a dictionary file.  The files
# in this directory will be used to make a wordlist.
DOCDIR = /usr/local/man

# If your machine doesn't have usleep uncomment it in the following line.
# I know this includes ultrix 4.2 and hp-ux 7.? and many sys V based machines.
# Don't know about others. if you need usleep and your machine does not have
# select, change usleep.o to usleep5.o (mostly sysV machines).
OBJS = letters.o kinput.o term.o word.o highscore.o # usleep.o

# The following line will stop gcc from complaining about the arguments
# sun's make uses.  It shouldn't bother anyone else.
.c.o:
	$(CC) $(CFLAGS) -c $<

letters: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o letters $(LDFLAGS)

letters.o word.o kinput.o: \
		kinput.h
letters.o term.o highscore.o kinput.o: \
		term.h
letters.o highscore.o kinput.o: \
		terms.h
word.o highscore.o letters.o: \
		config.h

install: letters
	$(INSTALL) -s -m 2755 letters $(BINDIR)
	sed -e 's;LIBDIR;$(LIBDIR);' -e 's;DICTIONARY;$(DICTIONARY);'\
		letters.man > letters.$(MANEXT)
	$(INSTALL) -c -m 0644 letters.$(MANEXT) $(MANDIR)/letters.$(MANEXT)
	if [ ! -f $(LIBDIR)/letters.high ] ;then \
		$(INSTALL) -c -m 0664 letters.high $(LIBDIR) ;fi

install_hpux: letters
	install -c $(BINDIR) letters
	install -c $(MANDIR) letters.man
	install -c $(LIBDIR) letters.high
	chmod 0666 $(LIBDIR)/letters.high

clean:
	rm -f rm *.o letters letters.$(MANEXT)

shar:
	shar -o letters.shar *.[ch] Makefile* letters.man letters.high README \
		README.Amiga

tar:
	tar cf letters.tar *.[ch] Makefile* letters.man letters.high README \
		README.Amiga

dict:
	awk '{ for (i = 0; i <= NF; ++i)\
		if($$i ~ /^[a-zA-Z][a-z]*$$/) { print $$i } }'\
		`find $(DOCDIR) -type f -print` |\
		sort -u > $(DICTIONARY)
