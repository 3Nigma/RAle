#ifndef _DB_H_
#define _DB_H_

#include <gtk/gtk.h>
#include <string.h>
#include <sqlite3.h>

#define PSALE_BD_NUME_FIS "date.sqlite"
#define TB_NUME_TABEL_MEDIA "media"

typedef enum {
  DB_IMG_TUSCALE_LOGO,
  DB_IMG_COLAPSEAZA,
  DB_IMG_EXPANDEAZA,
  DB_IMG_EEPROM,
  DB_IMG_PARASESTE,
  DB_IMG_REIA_COD,
  DB_IMG_SALVEAZA_COD,
  DB_IMG_TRIMITE_LA_ALE,
  DB_IMG_LICENTA,
  DB_IMG_INFO,
  DB_IMG_PAG_PREC,
  DB_IMG_PAG_URM
} db_media_type;

extern int 
db_incarca_exemple_carte(GtkListStore *st);

extern const char *
db_obtine_cod_complet(gchar *titluScurt);

extern const char *
db_obtine_adresa_actualizare();

extern int 
db_obtine_versiune_curenta();

extern GdkPixbuf *
db_obtine_imagine_media_scalata(db_media_type tp, gint w, gint h);

#endif
