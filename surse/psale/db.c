#include "db.h"

static int 
db_incarca_exemplu_callback(void *userArg, int nrOfCols, char **colTxts, char **colNames) {
  GtkComboBoxText *origcmbx = (GtkComboBoxText *)userArg;
  int colId = 0;

  /* încărcăm în listă doar titlul exemplului */
  for(colId = 0; colId < nrOfCols; colId++)
    if(strcmp(colNames[colId], "Titlu") == 0) 
      gtk_combo_box_text_append_text(origcmbx, colTxts[colId]);

  return 0;
}

int 
db_incarca_exemple_carte(GtkComboBoxText *widget) {
  sqlite3 *db;
  char *zErrMsg = 0;

  if(sqlite3_open("date.sqlite", &db)) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 0;
  }

  if(sqlite3_exec(db, "select * from exemple", db_incarca_exemplu_callback, widget, &zErrMsg) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return 0;
  }

  sqlite3_close(db);
  return 1;
}
