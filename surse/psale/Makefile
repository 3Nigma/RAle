PROJ_NAME := psale
CC := gcc
OBJS := db.o finfo.o fcod.o dl.o

.c.o :
	$(CC) `pkg-config --cflags gtk+-2.0 gtksourceview-2.0 sqlite3` -c $< -o $@

all : $(OBJS)
	$(CC) $(OBJS) main.c -o $(PROJ_NAME) `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 sqlite3 libcurl`

clean :
	rm -f $(PROJ_NAME) $(OBJS) *~
