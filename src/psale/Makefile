# psAle - If Ale's board ever needed an app, then this would be IT!
# Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
# 
# This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
# COPYING file that resides in the same directory as this file does or send a letter to 
# Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

PROJ_NAME := psale
CC := gcc
CFLAGS := -Wall -g
OBJS := fprin.o db.o finfo.o fcod.o dl.o al.o vc.o

.c.o :
	$(CC) $(CFLAGS) `pkg-config --cflags gtk+-2.0 gtksourceview-2.0 sqlite3 poppler-glib` -c $< -o $@

all : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) main.c -o $(PROJ_NAME) `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 sqlite3 libcurl poppler-glib`

win : $(OBJS)
	$(CC) -mwindows $(CFLAGS) $(OBJS) main.c -o $(PROJ_NAME) `pkg-config --cflags --libs gtk+-2.0 gtksourceview-2.0 sqlite3 libcurl`

clean :
	rm -f $(PROJ_NAME) $(OBJS) *~
