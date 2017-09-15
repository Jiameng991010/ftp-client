CC=gcc

SRCS=main.c\
		ftp_socket.c\
		ftp_command.c\
		ftp_string.c
	

OBJS=$(SRCS:.c=.o)

EXEC=main

start:$(OBJS)
	$(CC) $(SRCS) -o $(EXEC)

.c.o:
	$(CC) -o $@ -c $<

clean:
	rm -rf $(OBJS) $(EXEC) *~
