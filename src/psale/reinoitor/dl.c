/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <assert.h>

#include <glib/gprintf.h>

#include "db.h"
#include "dl.h"

static size_t citeste_date_actualizare_recurent(void *ptr, size_t size, size_t nmemb, void *userdata);
static gboolean dl_descarca_lista_actualizari(DLInstanta *dli, FILE *fisLocal);
static gboolean dl_descarca_actualizare_tinta(DLInstanta *dli, FILE *fisLocal);
static gboolean dl_descarca_fisier(CURL *legaturaRetea, const char *adresaInternet, FILE *fisLocal);
static gint dl_fct_comparator_cautare_versiune_sepcifica(IntrareActualizare *elemDinListaDeActualizari, Versiune *versTinta);
static int instiintare_progres_retea(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

DLInstanta *dl_initializeaza(const char *adresa) {
    DLInstanta *deReturnat = NULL;

    if ((deReturnat = g_new0(DLInstanta, 1)) != NULL) {
        if ((deReturnat->adresaActualizare = g_new0(gchar, DL_LUNGIME_MAX_URL)) != NULL) {
            g_sprintf(deReturnat->adresaActualizare, "%s", adresa);

            if ((deReturnat->resursaRetea = curl_easy_init()) != NULL) {
                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_TIMEOUT, DL_CURL_TIMEOUT);

                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_SSL_VERIFYPEER, TRUE);
                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_CAINFO, "cacert.pem");

                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_WRITEFUNCTION, citeste_date_actualizare_recurent);

                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_PROGRESSFUNCTION, instiintare_progres_retea);
                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_PROGRESSDATA, NULL);
                curl_easy_setopt(deReturnat->resursaRetea, CURLOPT_NOPROGRESS, 0);

                if ((deReturnat->versiuneaServerTinta = sda_intrare_actualizare_initializeaza()) != NULL) {
                    /* știm adresa de actualizare, am inițializat resursa rețelei, iar lista de intrări de actualizare nu necesită inițializare.
                     * Avem tot ce ne trebuie pentru a putea continua procedeul de actualizare în siguranță. */
                    g_debug("Structura 'DLInstanta' a fost inițializată cu succes!");
                } else {
                    dl_curata(&deReturnat);
                    deReturnat = NULL;
                }
            } else {
                g_warning("Nu am putut inițializa biblioteca de rețea CURL a structurei 'DLInstanta'!");
                dl_curata(&deReturnat);
                deReturnat = NULL;
            }
        } else {
            g_warning("Nu am putut aloca memorie suficientă pentru adresa de actualizare (URL) a structurei 'DLInstanta'!");
            dl_curata(&deReturnat);
            deReturnat = NULL;
        }
    } else {
        g_warning("Nu am putut aloca memorie suficientă pentru structura 'DLInstanta'!");
    }

    return deReturnat;
}

void dl_curata(DLInstanta **dli) {
    if(dli == NULL) return;

    g_free((*dli)->adresaActualizare);
    g_free((*dli)->adresaPachetLocal);
    if ((*dli)->resursaRetea != NULL) curl_easy_cleanup((*dli)->resursaRetea);
    sda_intrare_actualizare_curata(&(*dli)->versiuneaServerTinta);
    g_free(*dli);
    (*dli) = NULL;
}

gboolean dl_obtine_lista_actualizari(DLInstanta* dli) {
    g_assert(dli != NULL);

    FILE *fTemp = NULL;
    GRegex *tipar = NULL;
    gchar buffLinie[DL_LUNGIME_MAX_URL + DL_LUNGIME_MAX_MESAJ_VERSIUNE + 20];
    GMatchInfo *containerPotriviri = NULL;
    IntrareActualizare *dateVersiune = NULL;
    gboolean rezultatFunctie = TRUE;

    if ((fTemp = tmpfile()) != NULL) {
        if (dl_descarca_lista_actualizari(dli, fTemp)) {
            rewind(fTemp);

            tipar = g_regex_new(PSALE_FORMAT_VERSIUNE_REGEX ":\"([http|www]\\S+)\":\"(.+)\"\\\\0", 0, 0, NULL);
            while (fgets(buffLinie, sizeof (buffLinie), fTemp) != NULL) {
                /* extragem informația utilă pentru verificarea versiunii */
                g_debug("Linia curentă interpretată : %s", buffLinie);
                g_regex_match(tipar, buffLinie, 0, &containerPotriviri);
                if (g_match_info_matches(containerPotriviri)) {
                    /* [1] = versiune majoră, [2] = versiune minoră, [3] = adresă de descărcare, [4] = mesajul modificării */
                    if ((dateVersiune = sda_intrare_actualizare_initializeaza()) != NULL) {

                        dateVersiune->vers.major = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 1), NULL, 10);
                        dateVersiune->vers.minor = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 2), NULL, 10);
                        g_sprintf(dateVersiune->adresaDeDescarcare, "%s", g_match_info_fetch(containerPotriviri, 3));
                        g_sprintf(dateVersiune->mesajModificari, "%s", g_match_info_fetch(containerPotriviri, 4));

                        dli->intrariActualizare = g_slist_append(dli->intrariActualizare, dateVersiune);
                    }
                }
            }
            g_regex_unref(tipar);
        }
        fclose(fTemp);
    } else {
        g_warning("Nu am putut aloca fișierul temporar pentru lista de versiuni disponibile!");
        rezultatFunctie = FALSE;
    }

    g_match_info_free(containerPotriviri);

    return rezultatFunctie;
}

gboolean dl_contine_lista_actualizari_elemente(DLInstanta *dli) {
    g_assert(dli != NULL);
    
    return dli->intrariActualizare != NULL;
}

void dl_incarca_ultima_versiune_server(DLInstanta *dli) {
    g_assert(dli->intrariActualizare != NULL);

    GSList *ptElemDinLista = dli->intrariActualizare;
    IntrareActualizare *elemDeIntrare = NULL;

    while (ptElemDinLista != NULL) {
        elemDeIntrare = (IntrareActualizare *) ptElemDinLista->data;
        if (sda_comparaVersiuni(&elemDeIntrare->vers, &dli->versiuneaServerTinta->vers) == 1) {
            sda_intrare_actualizare_copiaza(elemDeIntrare, dli->versiuneaServerTinta);
        }
        ptElemDinLista = g_slist_next(ptElemDinLista);
    }
}

gboolean dl_incarca_versiune_specifica_server(DLInstanta *dli, Versiune *versDorita) {
    g_assert(dli != NULL);
    g_assert(versDorita != NULL);

    dli->versiuneaServerTinta = (IntrareActualizare *) (g_slist_find_custom(dli->intrariActualizare,
            versDorita,
            (GCompareFunc) (&dl_fct_comparator_cautare_versiune_sepcifica)));

    return dli->versiuneaServerTinta != NULL;
}

gboolean dl_exista_versiune_mai_buna_decat(DLInstanta *dli, Versiune *versCurentaLocal) {
    g_assert(NULL != versCurentaLocal);
    g_assert(NULL != dli);
    g_assert(NULL != dli->versiuneaServerTinta);

    return sda_comparaVersiuni(&dli->versiuneaServerTinta->vers, versCurentaLocal) == 1;
}

char *dl_descarca_actualizare_tinta_local(DLInstanta *dli) {
    g_assert(dli != NULL);

    /* ajungând aici avem certitudinea că există o versiunea de aplicație mai bună decât cea locală,
     * iar datele necesare sunt încărcate în variabila de instanță 'dli'. */

    FILE *tempFisArhiva = NULL;
    char *numeArhivaTemp = NULL;

    if ((numeArhivaTemp = g_new(char, L_tmpnam)) != NULL) {
        tmpnam(numeArhivaTemp);
        if ((tempFisArhiva = fopen(numeArhivaTemp, "wb")) != NULL) {
            if (dl_descarca_actualizare_tinta(dli, tempFisArhiva) == FALSE) {
                g_warning("Am întâmpinat probleme la descărcarea versiunii ținte astfel încât nu am putut definitiva această etapă importantă!");
                g_free(numeArhivaTemp);
                numeArhivaTemp = NULL;
            }
            fclose(tempFisArhiva);
        } else {
            g_warning("Nu am putut crea fișierul local ce găzduiește actualizarea.");
        }
    }

    return numeArhivaTemp;
}

static size_t citeste_date_actualizare_recurent(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t octetiScrisi = fwrite(ptr, size, nmemb, (FILE *) userdata);

    return octetiScrisi;
}

static int instiintare_progres_retea(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    g_debug("Progresul actual: %f/%f", dlnow, dltotal);
    return 0;
}

static gboolean dl_descarca_lista_actualizari(DLInstanta *dli, FILE *fisLocal) {
    g_assert(dli != NULL);
    g_assert(fisLocal != NULL);
    
    return dl_descarca_fisier(dli->resursaRetea, dli->adresaActualizare, fisLocal);
}

static gboolean dl_descarca_actualizare_tinta(DLInstanta *dli, FILE *fisLocal) {
    g_assert(dli != NULL);
    g_assert(fisLocal != NULL);
    
    return dl_descarca_fisier(dli->resursaRetea, dli->versiuneaServerTinta->adresaDeDescarcare, fisLocal);
}

static gboolean dl_descarca_fisier(CURL *legaturaRetea, const char *adresaInternet, FILE *fisLocal) {
    g_assert(legaturaRetea != NULL);
    g_assert(adresaInternet != NULL);
    g_assert(fisLocal != NULL);

    gboolean opIncheiataCuSucces = FALSE;
    char errBuff[CURL_ERROR_SIZE];

    curl_easy_setopt(legaturaRetea, CURLOPT_URL, adresaInternet);
    curl_easy_setopt(legaturaRetea, CURLOPT_ERRORBUFFER, errBuff);
    curl_easy_setopt(legaturaRetea, CURLOPT_WRITEDATA, fisLocal);
    if (curl_easy_perform(legaturaRetea) == CURLE_OK) {
        opIncheiataCuSucces = TRUE;
    } else {
        g_warning("S-a ivit o eroare la descărcarea fișierului : %s!", errBuff);
    }

    return opIncheiataCuSucces;
}

static gint dl_fct_comparator_cautare_versiune_sepcifica(IntrareActualizare *elemDinListaDeActualizari, Versiune *versTinta) {
    g_assert(elemDinListaDeActualizari != NULL);
    g_assert(versTinta != NULL);

    return sda_comparaVersiuni(&elemDinListaDeActualizari->vers, versTinta);
}