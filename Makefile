CC = cc
CFLAGS = -g -O2
LDFLAGS = -pthread -ldiscord -lcurl

OBJS = main.o handler.o

all: toasty

toasty: $(OBJS)
	$(CC) -o toasty $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: all clean
.SUFFIXES: .c
