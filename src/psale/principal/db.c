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

static sqlite3_value *
bd_obtine_rezultat_unic(const char *fisBD, const char *com, int colId) {
  sqlite3 *db = NULL;
  sqlite3_stmt *af = NULL;
  sqlite3_value *rez = NULL;
  
  if(sqlite3_open(fisBD, &db)) {
    fprintf(stderr, "Nu pot deschide baza de date : %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }

  if(sqlite3_prepare_v2(db, com, -1, &af, NULL) != SQLITE_OK ||
     sqlite3_step(af) != SQLITE_ROW) {
    fprintf(stderr, "Nu pot pregati baza de date : %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }
  
  rez = sqlite3_column_value(af, colId);

  sqlite3_close(db);

  return rez;
}

static int 
bd_executa_comanda(const char *fisBD, const char *com, int (*frecurenta)(void *, int, char **, char **), void *paramUtilizator) {
  sqlite3 *db = NULL;
  char *zErrMsg = NULL;

  if(sqlite3_open(fisBD, &db)) {
    fprintf(stderr, "Nu pot deschide baza de date : %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 0;
  }

  if(sqlite3_exec(db, com, frecurenta, paramUtilizator, &zErrMsg) != SQLITE_OK) {
    fprintf(stderr, "Eroare la executarea comenzii pe BD : %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return 0;
  }

  sqlite3_close(db);
  return 1;
}

static int 
db_incarca_exemplu_recurent(void *userArg, int nrOfCols, char **colTxts, char **colNames) {
  /* Legendă : [0] = IndiceCarte, [1] = TipSursa, [2] = Titlu */
  GtkListStore *magazie = (GtkListStore *)userArg;
  GtkTreeIter iter;
  gchar sectiuneCompacta[6];
  
  g_sprintf(sectiuneCompacta, "%s%s", colTxts[0], colTxts[1]);
  gtk_list_store_append(magazie, &iter);
  gtk_list_store_set(magazie, &iter, 0, sectiuneCompacta, 1, colTxts[2], -1);

  return 0;
}

int 
db_incarca_exemple_carte(GtkListStore *st, const gchar *limbajDorit) {
  gchar propSQL[256];

  g_sprintf(propSQL, "select IndiceCarte, TipSursa, Titlu from exemple where TipSursa='%s' order by OrdineAfis asc", limbajDorit);

  return bd_executa_comanda(PSALE_BD_NUME_FIS, propSQL, db_incarca_exemplu_recurent, (void *)st);
}

const char *
db_obtine_cod_complet(const gchar *titluLung, const gchar *limbajDorit) {
  char sintaxaInterogare[124];

  sprintf(sintaxaInterogare, "select TextCod from exemple where Titlu = '%s' and TipSursa ='%s'", titluLung, limbajDorit);
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, sintaxaInterogare, 0);

  return (const char *)sqlite3_value_text(rez);
}

const char *
db_obtine_adresa_actualizare() {
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, "select LinkReinoire FROM meta", 0);

  return (const char *)sqlite3_value_text(rez);
}

int 
db_obtine_versiune_curenta() {
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, "select VersBDActuala FROM meta", 0);

  return sqlite3_value_int(rez);
}

