/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _FINFO_H_
#define _FINFO_H_

#include <gtk/gtk.h>

#define PSALE_NUME_AUTOR "Victor ADĂSCĂLIȚEI"
#define PSALE_VERSIUNE "1.0"
#define PSALE_TEXT_DESPRE "Bună!\n" \
  "Aplicația de față a fost construită pentru a fi utilizată împreună cu proiectul Ale v0.9 al comunității tuscale.ro.\n"\
  "Scopul ei este acela de a facilita interacțiunea într-o manieră ușoară și intuitivă cu plăcuța proiectului.\n\n"\
  "Pentru orice nedumeriri/sugestii/reclamații ne găsiți în comunitatea proiectului."

extern GtkWidget *
initializeaza_formular_info();

#endif
