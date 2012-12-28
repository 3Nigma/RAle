/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _DL_H_
#define _DL_H_

#include <gtk/gtk.h>
#include <curl/curl.h>
#include "sda.h"

typedef struct {
    CURL *resursaRetea;
    gchar *adresaActualizare;
    gchar *adresaPachetLocal;
    GSList *intrariActualizare;
    IntrareActualizare *versiuneaServerTinta;
} DLInstanta;

extern DLInstanta *dl_initializeaza(const char *adresa);
extern void dl_curata(DLInstanta **dli);

extern gboolean dl_obtine_lista_actualizari(DLInstanta* dli);
extern gboolean dl_contine_lista_actualizari_elemente(DLInstanta *dli);
extern void dl_incarca_ultima_versiune_server(DLInstanta *dli);
extern gboolean dl_incarca_versiune_specifica_server(DLInstanta *dli, Versiune *versDorita);
extern gboolean dl_exista_versiune_mai_buna_decat(DLInstanta *dli, Versiune *versCurentaLocal);
extern Versiune *dl_obtine_vers_curenta_server();
extern char *dl_descarca_actualizare_tinta_local(DLInstanta *dli);

#endif
