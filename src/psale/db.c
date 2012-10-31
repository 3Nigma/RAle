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
db_incarca_exemple_carte(GtkListStore *st) {
  return bd_executa_comanda(PSALE_BD_NUME_FIS, "select IndiceCarte, TipSursa, Titlu from exemple order by OrdineAfis asc", db_incarca_exemplu_recurent, (void *)st);
}

const char *
db_obtine_cod_complet(const gchar *titluLung) {
  char sintaxaInterogare[124];

  sprintf(sintaxaInterogare, "select TextCod from exemple where Titlu = '%s'", titluLung);
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

static const void *
db_obtine_blob(const char *numeTbl, const char *numeRes, int *dim) {
  sqlite3_value *rez = NULL;
  char afirmatie[255];
  const void *deRet = NULL;
  
  sprintf(afirmatie, "select Date from %s where Nume='%s'", numeTbl, numeRes);
  rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, afirmatie, 0);
  
  deRet = sqlite3_value_blob(rez);
  if(dim != NULL) (*dim) = sqlite3_value_bytes(rez);
  
  return deRet;
}

static const void *
db_obtine_img_info_aleatoare(const char *numeTbl, int *dim) {
  sqlite3_value *rez = NULL;
  char afirmatie[255];
  const void *deRet = NULL;
  
  sprintf(afirmatie, "select Date from %s where Nume like 'fi_img_info_%%' order by random() limit 1", numeTbl);
  rez = bd_obtine_rezultat_unic(PSALE_BD_NUME_FIS, afirmatie, 0);
  
  deRet = sqlite3_value_blob(rez);
  if(dim != NULL) (*dim) = sqlite3_value_bytes(rez);
  
  return deRet;
}

GdkPixbuf *
db_obtine_imagine_media_scalata(db_media_type tp, gint w, gint h) {
  GInputStream *memImg = NULL;
  GdkPixbuf *bufImg = NULL;
  const void *vMem = NULL;
  int dimMem = 0;
  
  switch(tp) {
  case DB_IMG_TUSCALE_LOGO:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fp_img_logo_tuscale_ale", &dimMem);
    break;
  case DB_IMG_COLAPSEAZA:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_colapseaza", &dimMem);
    break;
  case DB_IMG_EXPANDEAZA:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_expandeaza", &dimMem);
    break;
  case DB_IMG_EEPROM:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_eeprom", &dimMem);
    break;
  case DB_IMG_PARASESTE:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_paraseste_frm_cod", &dimMem);
    break;
  case DB_IMG_PARASESTE_INFO:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_paraseste_frm_info", &dimMem);
    break;
  case DB_IMG_PARASESTE_EEPROM:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_paraseste_frm_eeprom", &dimMem);
    break;
  case DB_IMG_SCRIE_EEPROM:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_scrie_eeprom", &dimMem);
    break;
  case DB_IMG_CITESTE_EEPROM:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fc_img_bt_icoana_citeste_eeprom", &dimMem);
    break;
  case DB_IMG_REIA_COD:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fp_img_bt_icoana_reia_cod", &dimMem);
    break;
  case DB_IMG_SALVEAZA_COD:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fp_img_bt_icoana_salveaza_cod", &dimMem);
    break;
  case DB_IMG_TRIMITE_LA_ALE:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fp_img_bt_icoana_trimite_la_ale", &dimMem);
    break;
  case DB_IMG_LICENTA:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "fi_img_licenta", &dimMem);
    break;
  case DB_IMG_INFO:
    vMem = db_obtine_img_info_aleatoare(TB_NUME_TABEL_MEDIA, &dimMem);
    break;
  case DB_IMG_PAG_PREC:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "vc_img_bt_pag_prec", &dimMem);
    break;
  case DB_IMG_PAG_URM:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "vc_img_bt_pag_urm", &dimMem);
    break;
  case DB_IMG_PAG_SALT_CUPRINS:
    vMem = db_obtine_blob(TB_NUME_TABEL_MEDIA, "vc_img_bt_pag_cuprins", &dimMem);
    break;
  default:
    /* nu ar trebui să ajungem aici */
    break;
  }
  memImg = g_memory_input_stream_new_from_data(vMem, dimMem, NULL);
  bufImg = gdk_pixbuf_new_from_stream_at_scale(memImg, w, h, FALSE, NULL, NULL); 
  
  g_input_stream_close(memImg,NULL,NULL);
  g_object_unref(G_OBJECT(memImg));
  
  return bufImg;
}
