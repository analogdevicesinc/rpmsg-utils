PROJECT1=rpmsgtest
PROJECT2=rpmsgbind
SDK_PATH=
CC_PREFIX=
ifndef CC
CC=gcc
endif
ifndef LDFLAGS
LDFLAGS=-pthread
endif
CFLAGS=-Wall
SOURCES= #$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
 
all: $(OBJECTS) rpmsg-xmit rpmsg-xmit-p rpmsg-bind-chardev


%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< $(LDLIBS)

clean:
	rm -rfv rpmsg-xmit
	rm -rfv rpmsg-xmit-p
	rm -rfv rpmsg-bind-chardev
	rm -rfv *.o