PROJECT1=rpmsgtest
PROJECT2=rpmsgbind
SDK_PATH=
CC_PREFIX=
CC=gcc
LDFLAGS=-pthread
CFLAGS=-Wall
SOURCES= #$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
 
all: $(OBJECTS) rpmsg-xmit-p rpmsg-bind-chardev
 
rpmsgtest: rpmsgtest.c
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) rpmsg-xmit-p.c -o rpmsg-xmit-p 

rpmsgbind: rpmsgbind.c
	$(CC) $(OBJECTS) $(CFLAGS) rpmsg-bind-chardev.c -o rpmsg-bind-chardev

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rfv rpmsg-xmit-p
	rm -rfv rpmsg-bind-chardev
	rm -rfv *.o