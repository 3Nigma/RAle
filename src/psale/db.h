#ifndef _DB_H_
#define _DB_H_

#include <gtk/gtk.h>
#include <string.h>
#include <sqlite3.h>

#define PSALE_BD_NUME_FIS "date.sqlite"

extern int 
db_incarca_exemple_carte(GtkListStore *st);

extern const char *
db_obtine_cod_complet(gchar *titluScurt);

extern const char *
db_obtine_adresa_actualizare();

extern int 
db_obtine_versiune_curenta();

#endif
