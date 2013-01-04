/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "par_aplicatie.h"

#include <glib/gprintf.h>
#include <getopt.h>

static enum OperatieInstanta obtineTipulOp(gchar *sirTipOp);

ParametriiRulareAplicatie *pa_incarca_parametrii(int argc, char *argv[]) {
    ParametriiRulareAplicatie *parAp = NULL;
    char optiune;
    int indexArgInStructura;
    
    struct option opt_lungi_aplicatie[] = {
        {
            "tipul-operatiei", required_argument, NULL, 't'
        },
        {
            "versiune-server", optional_argument, NULL, 'v'
        },
        {
            "fara-ig", no_argument, NULL, 'f'
        },
        {
            "ajutor", no_argument, NULL, 'a'
        },
        {
            NULL, 0, NULL, 0
        }
    };

    parAp = g_new0(ParametriiRulareAplicatie, 1);
    parAp->tipOp = ACTUALIZEAZA_NORMAL;
    parAp->versServer = NULL;
    parAp->folsesteIG = TRUE;
    
    while ((optiune = getopt_long(argc, argv, "t:v::fa", opt_lungi_aplicatie, &indexArgInStructura)) != -1) {
        switch (optiune) {
            case 't': /* tipul-operatiei */
                if ((parAp->tipOp = obtineTipulOp(optarg)) == INVALID) {
                    g_warning("Nu recunosc tipul operației cerute din consolă!");
                    pa_afiseaza_meniu();
                    pa_curata(&parAp);
                    return NULL;
                }
                break;
            case 'v': /* versiune-server */
                if (optarg != NULL && (parAp->versServer = sda_obtineVersiuneDinSir(optarg)) == NULL) {
                    g_warning("Formatul versiunii este incorect! Aplicația consideră următorul tipar valid: '%s'", PSALE_FORMAT_VERSIUNE_PRINTF);
                    pa_afiseaza_meniu();
                    pa_curata(&parAp);
                    return NULL;
                }
                break;
            case 'f': /* fara-ig */
                parAp->folsesteIG = FALSE;
                break;
            case 'a': /* ajutor */
                pa_afiseaza_meniu();
                pa_curata(&parAp);
                return NULL;
                break;   
            case '?': /* argumentul este necunoscut */
                g_warning("Nu recunosc argumentul prezentat așa că îl ignor! Invocați aplicația cu arugmentul '--ajutor' pentru a afla combinațiile posibile.");
                break;
        }
        
        if(optiune != '?') {
		  g_debug("Am recunosct comanda '%s', subparam '%s'.", opt_lungi_aplicatie[indexArgInStructura].name,
		                                                       (optarg == NULL ? "Nici unul" : optarg));
		}
    }
    
    return parAp;
}

void pa_curata(ParametriiRulareAplicatie **pr) {
    if(pr == NULL) return;
    
    g_free((*pr)->versServer);
    g_free(*pr);
    (*pr) = NULL;
}

void pa_afiseaza_meniu() {
    printf("Mod de folosire: \n"
           OS_NUME_RPSALE " [--tipul-operatiei actualizeaza-direct [--fara-ig] --versiune-server <vers. țintă>]\n"
           OS_NUME_RPSALE " [--tipul-operatiei afiseaza [--fara-ig] --versiune-server]\n"
           OS_NUME_RPSALE " [--ajutor]\n"
           "Unde:\n"
           "<vers. țintă> : versiunea la care se dorește realizată actualizarea în formatul '%s'\n\n"
           "Notă:\n"
           "Invocarea aplicației fără niciun argument duce la încărcarea comportamentului implicit al acesteia care este 'dictat de preferințele "
           "de actualizare (automată/manuală) alese de utilizator la prima rulare.\n", PSALE_FORMAT_VERSIUNE_PRINTF);
}

static enum OperatieInstanta obtineTipulOp(gchar *sirTipOp) {
    gchar *sirTipOpMinuscule = g_ascii_strdown(sirTipOp, -1);
    enum OperatieInstanta tipOp = INVALID;

    if (g_strcmp0(sirTipOpMinuscule, "actualizeaza-direct") == 0) {
        tipOp = ACTUALIZEAZA_DIRECT;
    } else if (g_strcmp0(sirTipOpMinuscule, "afiseaza") == 0) {
        tipOp = AFISEAZA;
    }

    g_free(sirTipOpMinuscule);
    return tipOp;
}
