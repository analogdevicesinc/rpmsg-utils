PROJECT1=rpmsgtest
PROJECT2=rpmsgbind
SDK_PATH=
CC_PREFIX=
CC=gcc
LDFLAGS=
CFLAGS=-O2
SOURCES= #$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
 
all: $(OBJECTS) rpmsgtest rpmsgbind
 
rpmsgtest: rpmsgtest.c
	$(CC) $(OBJECTS) $(CFLAGS) rpmsgtest.c -o rpmsgtest

rpmsgbind: rpmsgbind.c
	$(CC) $(OBJECTS) $(CFLAGS) rpmsgbind.c -o rpmsgbind



%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rfv rpmsgtest
	rm -rfv rpmsgbind
	rm -rfv *.o