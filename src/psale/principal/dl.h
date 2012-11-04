/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _DL_H_
#define _DL_H_

enum {DL_NICI_O_ACTUALIZARE = 0, DL_ACTUALIZAT, DL_PROBLEME_LA_ACTUALIZARE};

extern int 
dl_initializeaza(const char *);

extern int 
dl_acutalizeaza_bd();

extern void
dl_curata();

#endif
