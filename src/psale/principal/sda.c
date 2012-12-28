/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <glib/gprintf.h>
#include <string.h>
#include <stdlib.h>

#include "sda.h"

#ifdef G_OS_WIN32
#include <direct.h>
#elif defined G_OS_UNIX
#include <sys/stat.h>
#include <sys/types.h>
#endif

Versiune *sda_obtineVersiuneDinSir(gchar *sir) {
    GRegex *tipar = NULL;
    GMatchInfo *containerPotriviri = NULL;
    Versiune *vers = NULL;

    tipar = g_regex_new(PSALE_FORMAT_VERSIUNE_REGEX, 0, 0, NULL);
    g_regex_match(tipar, sir, 0, &containerPotriviri);
    if (g_match_info_matches(containerPotriviri)) {
        vers = g_new0(Versiune, 1);
        vers->major = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 1), NULL, 10);
        vers->minor = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 2), NULL, 10);
    }

    g_regex_unref(tipar);
    g_match_info_free(containerPotriviri);
    return vers;
}

gint sda_comparaVersiuni(Versiune *v1, Versiune *v2) {
    /* Returnează: 0 dacă v1 == v2, 1 dacă v1 > v2 și -1 dacă v1 < v2 */
    gint deReturnat = 0;

    if (v1->major == v2->major && v1->minor == v2->minor) {
        deReturnat = 0;
    } else if (v1->major > v2->major || (v1->major == v2->major && v1->minor > v2->minor)) {
        deReturnat = 1;
    } else {
        deReturnat = 2;
    }

    return deReturnat;
}

IntrareActualizare *sda_intrare_actualizare_initializeaza() {
    IntrareActualizare *intrare = NULL;
    gboolean esteStructAlocata = TRUE;

    if ((intrare = g_new0(IntrareActualizare, 1)) != NULL) {
        if ((intrare->adresaDeDescarcare = g_new0(gchar, DL_LUNGIME_MAX_URL)) == NULL ||
                (intrare->mesajModificari = g_new0(gchar, DL_LUNGIME_MAX_MESAJ_VERSIUNE)) == NULL) {
            esteStructAlocata = FALSE;
        }
    } else {
        esteStructAlocata = FALSE;
    }

    if (esteStructAlocata == FALSE) {
        g_warning("Nu am putut inițializa o structură standard 'IntrareActualizare'!");
        sda_intrare_actualizare_curata(&intrare);
        intrare = NULL;
    }

    return intrare;
}

void sda_intrare_actualizare_curata(IntrareActualizare **iai) {
    if (iai == NULL) return;

    g_free((*iai)->adresaDeDescarcare);
    g_free((*iai)->mesajModificari);
    g_free(*iai);
}

void sda_intrare_actualizare_copiaza(IntrareActualizare *sursa, IntrareActualizare *destinatie) {
    g_assert(sursa != NULL);
    g_assert(destinatie != NULL);

    g_sprintf(destinatie->adresaDeDescarcare, "%s", sursa->adresaDeDescarcare);
    g_sprintf(destinatie->mesajModificari, "%s", sursa->mesajModificari);
    destinatie->vers = sursa->vers;
}

gchar *sda_versiune_printf(Versiune *vers) {
    g_assert(vers != NULL);

    static gchar versFormatata[10];

    g_sprintf(versFormatata, PSALE_FORMAT_VERSIUNE_PRINTF, vers->major, vers->minor);

    return versFormatata;
}

FILE *sda_fopen_mkdir(const char *caleFis, const char *mode) {
    char *pCaleOriginala = strdup(caleFis);
    char delimitatorDirector;
    char *pCale = pCaleOriginala;

    while ((*pCale) != '\0') {
        if ((*pCale) == '\\' || (*pCale) == '/') {
            delimitatorDirector = (*pCale);
            (*pCale) = '\0';
#ifdef G_OS_WIN32
            _mkdir(pCaleOriginala);
#elif defined G_OS_UNIX
            mkdir(pCaleOriginala, S_IRWXG | S_IRWXO | S_IRWXU);
#endif
            (*pCale) = delimitatorDirector;
        }
        pCale += sizeof (char);
    }
    free(pCaleOriginala);

    return fopen(caleFis, mode);
}

gboolean sda_este_cale_fisier_valida(const char *caleFis) {
    g_assert(caleFis != NULL);
    
    size_t lungCale = strlen(caleFis);
            
    return caleFis[lungCale - 1] != '/' && caleFis[lungCale - 1] != '\\';
}