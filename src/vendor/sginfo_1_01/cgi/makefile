 CC = cc
 CCFLAGS = $(CFLAGS)

sginfo-query: sginfo-query.o util.o
	$(CC) sginfo-query.o util.o -o sginfo-query

.c.o: 
	$(CC) -c $(CCFLAGS) $<

clean:
	rm -f sginfo-query sginfo-query.o util.o
