#ifndef _DB_H_
#define _DB_H_

#include <gtk/gtk.h>
#include <string.h>
#include <sqlite3.h>

extern int 
db_incarca_exemple_carte(GtkComboBoxText *widget);

extern const unsigned char *
db_obtine_adresa_actualizare();

extern int 
db_obtine_versiune_curenta();

#endif
