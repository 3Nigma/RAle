/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _DB_H_
#define _DB_H_

#include <gtk/gtk.h>

#define PSALE_BD_NUME_FIS "date.sqlite"
#define TB_NUME_TABEL_MEDIA "media"
#define TB_NUME_TABEL_META "meta"

extern gboolean 
db_initializeaza();

extern void 
db_curata();

extern const char *
db_obtine_adresa_actualizare();

extern double 
db_obtine_versiune_curenta();

extern gboolean 
db_obtine_este_prima_rulare();

extern gboolean
db_consuma_prima_rulare();

extern gboolean 
db_obtine_este_actualizare_automata();

extern gboolean
db_seteaza_actualizare_automata();

extern gboolean
db_seteaza_actualizare_manuala();

#endif
