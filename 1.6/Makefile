CC=gcc
CFLAGS= -g -Wall -pthread
AR = ar
ARFLAGS = rcs

LIBSRC = mythread.c
LIBOBJ = $(LIBSRC:.c=.o)
LIBNAME = libmythread.a

TESTSRC = main.c
TESTBIN = test_threads

all: $(LIBNAME) $(TESTBIN)

$(LIBNAME): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTBIN): $(TESTSRC) $(LIBNAME)
	$(CC) $(CFLAGS) $< -L. -lmythread -o $@

clean:
	rm -f $(LIBOBJ) $(LIBNAME) $(TESTBIN)