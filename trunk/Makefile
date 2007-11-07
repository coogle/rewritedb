# If building a Debug build
CFLAGS = -ggdb -Wall -DDEBUG

# Normal build
#CFLAGS = -O3

SRCS = rewriteDB.c rewriteFuncs.c
CC = gcc
LD = gcc
SQLITE = ./sqlite
SQLITEFLAGS = 

LDFLAGS = -lsqlite

RM = /bin/rm -f
OBJS = rewriteDB.o rewriteFuncs.o
PROG = rewriteDB
DBS = rewrite.db

all: $(PROG) $(DBS)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

rewriteDB.o: rewriteDB.c rewriteDB.h
	$(CC) $(CFLAGS) -c rewriteDB.c

rewriteFuncs.o: rewriteFuncs.c rewriteFuncs.h
	$(CC) $(CFLAGS) -c rewriteFuncs.c

%.db: %.db.in
	${SQLITE} ${SQLITEFLAGS} $@ < $<

dbclean:
	$(RM) $(PROG) $(OBJS) $(DBS)

clean:
	$(RM) $(PROG) $(OBJS)
