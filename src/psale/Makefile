PROJ_NAME := psale
CC := gcc
CFLAGS := -Wall -g
OBJS := db.o finfo.o fcod.o dl.o al.o

.c.o :
	$(CC) $(CFLAGS) `pkg-config --cflags gtk+-2.0 gtksourceview-2.0 sqlite3` -c $< -o $@

all : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) main.c -o $(PROJ_NAME) `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 sqlite3 libcurl`

win : $(OBJS)
	$(CC) -mwindows $(CFLAGS) $(OBJS) main.c -o $(PROJ_NAME) `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 sqlite3 libcurl`

clean :
	rm -f $(PROJ_NAME) $(OBJS) *~
