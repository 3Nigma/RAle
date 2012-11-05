/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */
 
#include <string.h>
#include <sqlite3.h>
#include <glib/gprintf.h>

#include "db.h"

static sqlite3 *db = NULL;

gboolean 
db_initializeaza() {
  if(sqlite3_open(PSALE_BD_NUME_FIS, &db)) {
    g_warning("Nu pot deschide baza de date : %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return FALSE;
  }
  
  return TRUE;
}

void 
db_curata() {
  g_assert(NULL != db);
  
  sqlite3_close(db);
}

static sqlite3_stmt *
db_aplica_afirmatie(const char *com) {
  g_assert(NULL != db);

  sqlite3_stmt *af = NULL;
  
  if(sqlite3_prepare_v2(db, com, -1, &af, NULL) != SQLITE_OK ||
     sqlite3_step(af) != SQLITE_ROW) {
    g_warning("Nu pot pregăti baza de date : %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }
  
  return af;
}

const char *
db_obtine_adresa_actualizare() {
  static char adresaReinoire[512];
  sqlite3_stmt *af = NULL;
  sqlite3_value *rez = NULL;

  if((af = db_aplica_afirmatie("select LinkReinoire FROM " TB_NUME_TABEL_META)) != NULL) {
    rez = sqlite3_column_value(af, 0);
    strcpy(adresaReinoire, (const char *)sqlite3_value_text(rez));
    sqlite3_finalize(af);
  }
  
  return adresaReinoire;
}

double 
db_obtine_versiune_curenta() {
  double versActuala = 0.0;
  sqlite3_stmt *af = NULL;
  sqlite3_value *rez = NULL;

  if((af = db_aplica_afirmatie("select VersApActuala FROM " TB_NUME_TABEL_META)) != NULL) {
    rez = sqlite3_column_value(af, 0);
    versActuala = sqlite3_value_double(rez);
    sqlite3_finalize(af);
  }

  return versActuala;
}

gboolean 
db_obtine_este_prima_rulare() {
  double rezInterogare = TRUE;
  
  sqlite3_stmt *af = NULL;
  sqlite3_value *rez = NULL;

  if((af = db_aplica_afirmatie("select EstePrimaRulare FROM " TB_NUME_TABEL_META)) != NULL) {
    rez = sqlite3_column_value(af, 0);
    rezInterogare = (sqlite3_value_int(rez) == 1);
    sqlite3_finalize(af);
  }
  
  return rezInterogare;
}
