#include "db.h"

#define PSALE_BD_NUME_FIS "date.sqlite"

static sqlite3_value *
bd_obtine_rezultat_unic(const char *fisBD, const char *com, int colId) {
  sqlite3 *db = NULL;
  sqlite3_stmt *af = NULL;
  sqlite3_value *rez = NULL;
  char *zErrMsg = NULL;
  
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
  /* Legendă : [0] = TitluScurt, [1] = TextTitlu */
  GtkListStore *magazie = (GtkListStore *)userArg;
  GtkTreeIter iter;

  gtk_list_store_append(magazie, &iter);
  gtk_list_store_set(magazie, &iter, 0, colTxts[0], 1, colTxts[1], -1);

  return 0;
}

int 
db_incarca_exemple_carte(GtkListStore *st) {
  return bd_executa_comanda(PSALE_BD_NUME_FIS, "select TitluScurt, TextTitlu from exemple", db_incarca_exemplu_recurent, (void *)st);
}

const unsigned char *
db_obtine_cod_complet(const unsigned char *titluScurt) {
  char sintaxaInterogare[124];

  sprintf(sintaxaInterogare, "select TextCod from exemple where TitluScurt = '%s'", titluScurt);
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, sintaxaInterogare, 0);

  return sqlite3_value_text(rez);
}

const unsigned char *
db_obtine_adresa_actualizare() {
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, "select LinkReinoire FROM meta", 0);

  return sqlite3_value_text(rez);
}

int 
db_obtine_versiune_curenta() {
  sqlite3_value *rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, "select VersBDActuala FROM meta", 0);

  return sqlite3_value_int(rez);
}
