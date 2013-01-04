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
#include <stdlib.h>
#include <sqlite3.h>
#include <glib/gprintf.h>

#include "db.h"

static gboolean db_executa_comanda(const char *comanda);
static gboolean db_executa_comanda_cu_recurenta(const char *comanda, int (*fRecurenta)(void *, int, char **, char **), void *paramUtilizator);

static sqlite3_stmt *db_aplica_afirmatie(const char *com);
static int db_incarca_exemplu_recurent(void *userArg, int nrOfCols, char **colTxts, char **colNames);
static int db_incarca_modificari_recurent(void *userArg, int nrOfCols, char **colTxts, char **colNames);

static sqlite3 *db = NULL;

gboolean db_initializeaza() {
    if (sqlite3_open_v2(PSALE_BD_NUME_FIS, &db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK) {
        g_warning("Nu pot deschide baza de date : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return FALSE;
    }

    return TRUE;
}

void db_curata() {
    g_assert(NULL != db);

    sqlite3_close(db);
}

const char *db_obtine_adresa_actualizare() {
    static char adresaReinoire[512];
    sqlite3_stmt *af = NULL;
    sqlite3_value *rez = NULL;

    if ((af = db_aplica_afirmatie("select LinkReinoire from " DB_NUME_TABEL_META)) != NULL) {
        rez = sqlite3_column_value(af, 0);
        strcpy(adresaReinoire, (const char *) sqlite3_value_text(rez));
        sqlite3_finalize(af);
    }

    return adresaReinoire;
}

Versiune *db_obtine_versiune_curenta() {
    Versiune *versActuala = NULL;
    sqlite3_stmt *af = NULL;
    sqlite3_value *rez = NULL;

    if ((af = db_aplica_afirmatie("select VersMajoraCurenta, VersMinoraCurenta from " DB_NUME_TABEL_META)) != NULL) {
        versActuala = g_new(Versiune, 1);

        /* obține partea majoră a versiunii */
        rez = sqlite3_column_value(af, 0);
        versActuala->major = sqlite3_value_int(rez);

        /* obține partea minoră a versiunii */
        rez = sqlite3_column_value(af, 1);
        versActuala->minor = sqlite3_value_int(rez);

        sqlite3_finalize(af);
    }

    return versActuala;
}

gboolean db_obtine_este_prima_rulare() {
    double rezInterogare = TRUE;

    sqlite3_stmt *af = NULL;
    sqlite3_value *rez = NULL;

    if ((af = db_aplica_afirmatie("select EstePrimaRulare from " DB_NUME_TABEL_META)) != NULL) {
        rez = sqlite3_column_value(af, 0);
        rezInterogare = (sqlite3_value_int(rez) == 1);
        sqlite3_finalize(af);
    }

    return rezInterogare;
}

gboolean db_consuma_prima_rulare() {
    return db_executa_comanda("update " DB_NUME_TABEL_META " set EstePrimaRulare = 0");
}

gboolean db_obtine_este_actualizare_automata() {
    double rezInterogare = TRUE;

    sqlite3_stmt *af = NULL;
    sqlite3_value *rez = NULL;

    if ((af = db_aplica_afirmatie("select ActualizariAutomate from " DB_NUME_TABEL_META)) != NULL) {
        rez = sqlite3_column_value(af, 0);
        rezInterogare = (sqlite3_value_int(rez) == 1);
        sqlite3_finalize(af);
    }

    return rezInterogare;
}

gboolean db_seteaza_actualizare_automata() {
    return db_executa_comanda("update " DB_NUME_TABEL_META " set ActualizariAutomate = 1");
}

gboolean db_seteaza_actualizare_manuala() {
    return db_executa_comanda("update " DB_NUME_TABEL_META " set ActualizariAutomate = 0");
}

int db_incarca_exemple_carte(GtkListStore *st, const gchar *limbajDorit) {
    gchar propSQL[256];

    g_sprintf(propSQL, "select IndiceCarte, TipSursa, Titlu from " DB_NUME_TABEL_EXEMPLE " where TipSursa='%s' order by OrdineAfis asc", limbajDorit);

    return db_executa_comanda_cu_recurenta(propSQL, db_incarca_exemplu_recurent, (void *) st);
}

char *db_obtine_cod_complet(const gchar *titluLung, const gchar *limbajDorit) {
    char propSQL[256];
    char *codRezultat = NULL;
    sqlite3_stmt *af = NULL;
    sqlite3_value *rez = NULL;

    sprintf(propSQL, "select TextCod from " DB_NUME_TABEL_EXEMPLE " where Titlu = '%s' and TipSursa ='%s'", titluLung, limbajDorit);
    if ((af = db_aplica_afirmatie(propSQL)) != NULL) {
        rez = sqlite3_column_value(af, 0);
#ifdef G_OS_WIN32
        codRezultat = (char *) malloc((strlen((const char *) sqlite3_value_text(rez)) + 1) * sizeof (char));
        strcpy(codRezultat, (const char *) sqlite3_value_text(rez));
#elif defined G_OS_UNIX
        codRezultat = strdup((const char *) sqlite3_value_text(rez));
#endif
        sqlite3_finalize(af);
    }

    return codRezultat;
}

gboolean db_incarca_informatii_despre_versiuni(GSList **lst) {
    gchar propSQL[256];

    g_sprintf(propSQL, "select vMajora, vMinora, TextNoutati from " DB_NUME_TABEL_MODIFICARI " order by vMajora desc, vMinora desc");

    return db_executa_comanda_cu_recurenta(propSQL, db_incarca_modificari_recurent, (void *) lst);
}

static gboolean db_executa_comanda(const char *comanda) {
    return db_executa_comanda_cu_recurenta(comanda, NULL, NULL);
}

static gboolean db_executa_comanda_cu_recurenta(const char *comanda, int (*fRecurenta)(void *, int, char **, char **), void *paramUtilizator) {
    char *zErrMsg = NULL;

    if (sqlite3_exec(db, comanda, fRecurenta, paramUtilizator, &zErrMsg) != SQLITE_OK) {
        g_warning("Eroare la executarea comenzii pe BD : %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return FALSE;
    }

    return TRUE;
}

static sqlite3_stmt *db_aplica_afirmatie(const char *com) {
    g_assert(NULL != db);

    sqlite3_stmt *af = NULL;

    if (sqlite3_prepare_v2(db, com, -1, &af, NULL) != SQLITE_OK ||
            sqlite3_step(af) != SQLITE_ROW) {
        g_warning("Nu pot pregăti baza de date : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    return af;
}

static int db_incarca_exemplu_recurent(void *userArg, int nrOfCols, char **colTxts, char **colNames) {
    g_assert(nrOfCols == 3);

    /* Legendă : [0] = IndiceCarte, [1] = TipSursa, [2] = Titlu */
    GtkListStore *magazie = (GtkListStore *) userArg;
    GtkTreeIter iter;
    gchar sectiuneCompacta[6];

    g_sprintf(sectiuneCompacta, "%s%s", colTxts[0], colTxts[1]);
    gtk_list_store_append(magazie, &iter);
    gtk_list_store_set(magazie, &iter, 0, sectiuneCompacta, 1, colTxts[2], -1);

    return 0;
}

static int db_incarca_modificari_recurent(void *userArg, int nrOfCols, char **colTxts, char **colNames) {
    g_assert(nrOfCols == 3);

    /* Legendă : [0] = vMajora, [1] = vMinora, [2] = TextNoutati */
    GSList **lstModificari = (GSList **) userArg;
    BDIntrareTabelModificare *intr = NULL;

    intr = g_new0(BDIntrareTabelModificare, 1);
    intr->vers.major = (guint) g_ascii_strtoull(colTxts[0], NULL, 10);
    intr->vers.minor = (guint) g_ascii_strtoull(colTxts[1], NULL, 10);
    intr->detalii = g_strdup(colTxts[2]);
    (*lstModificari) = g_slist_append((*lstModificari), intr);

    return 0;
}
