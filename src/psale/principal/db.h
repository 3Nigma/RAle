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

typedef enum {
  DB_IMG_TUSCALE_LOGO,
  DB_IMG_COLAPSEAZA,
  DB_IMG_EXPANDEAZA,
  DB_IMG_EEPROM,
  DB_IMG_PARASESTE,
  DB_IMG_PARASESTE_EEPROM,
  DB_IMG_PARASESTE_INFO,
  DB_IMG_SCRIE_EEPROM,
  DB_IMG_CITESTE_EEPROM,
  DB_IMG_REIA_COD,
  DB_IMG_SALVEAZA_COD,
  DB_IMG_TRIMITE_LA_ALE,
  DB_IMG_LICENTA,
  DB_IMG_INFO,
  DB_IMG_PAG_PREC,
  DB_IMG_PAG_URM,
  DB_IMG_PAG_SALT_CUPRINS
} db_media_type;

extern int 
db_incarca_exemple_carte(GtkListStore *st, const gchar *limbajDorit);

extern const char *
db_obtine_cod_complet(const gchar *titluLung, const gchar *limbajDorit);

extern const char *
db_obtine_adresa_actualizare();

extern int 
db_obtine_versiune_curenta();

extern GdkPixbuf *
db_obtine_imagine_media_scalata(db_media_type tp, gint w, gint h);

#endif
