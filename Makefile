

OBJS=playmusic.o wave_header.o

LFLAGS=

CFLAGS=-I.

SFLAGS=

sincos: $(OBJS)
	gcc -o playmusic $(OBJS) -lm

.S.o:
	gcc $(SFLAGS) -c $< 

.c.o:
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o *~ playmusic tmp.data