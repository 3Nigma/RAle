/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <glib/gprintf.h>

#include "main.h"

static void lanseazaPSAle();

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    ParametriiRulareAplicatie *optiuniExterne = NULL;

    /* încărcăm eventualele informații venite din exterior (în parametrii dați prin linia de comandă) */
    if ((optiuniExterne = pa_incarca_parametrii(argc, argv)) != NULL) {
        if (db_initializeaza()) {

            if (db_obtine_este_prima_rulare() &&
                    optiuniExterne->tipOp == ACTUALIZEAZA_NORMAL) {
                g_debug("Afișez formularul de 'primă rulare' ...");

                /* pare-se că este prima execuție a aplicației (de la instalare ?).
                 * Interoghează-l pe utilizator privind modul de actualizare dorit */
                FPRInstanta *frmPrimaIntrebare = fpr_instantiaza();
                switch (fpr_lanseaza_si_asteapta_raspuns(frmPrimaIntrebare)) {
                    case FPR_RASPUNS_MAI_TARZIU:
                        break;
                    case FPR_RASPUNS_MANUAL:
                        db_seteaza_actualizare_manuala();
                        db_consuma_prima_rulare();
                        break;
                    case FPR_RASPUNS_AUTOMAT:
                        db_seteaza_actualizare_automata();
                        db_consuma_prima_rulare();
                        break;
                }
                fpr_elibereaza(&frmPrimaIntrebare);

            } else {
                g_debug("Aceasta nu este prima execuție a aplicației ...");
            }

            if (db_obtine_este_actualizare_automata() || optiuniExterne->tipOp == ACTUALIZEAZA_DIRECT) {
                FAInstanta *fSesiuneActualizare = fa_initializeaza(optiuniExterne);

                fa_lanseaza_sesiune_actualizare(fSesiuneActualizare);
                fa_curata(&fSesiuneActualizare);
            }
            db_curata();
        } else {
            g_warning("Nu am putut inițializa modulul bazei de date. Procesul de actualizare nu a putut avea loc!");
        }

        if (optiuniExterne->tipOp != AFISEAZA) {
            lanseazaPSAle();
        }
        pa_curata(&optiuniExterne);
    } else {
        g_debug("S-a dorit lansarea cu parametrii din linia de comandă, dar operația a fost anulată deoarece ");
    }

    return 0;
}

static void lanseazaPSAle() {
#ifdef G_OS_WIN32
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;

    ZeroMemory(&startupInfo, sizeof (startupInfo));
    ZeroMemory(&processInformation, sizeof (processInformation));

    startupInfo.cb = sizeof (startupInfo);

    if (!CreateProcess(NULL, RPS_CALE_PSALE, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation)) {
        g_error("Nu am putut porni aplicația 'psAle'!");
    }
#elif defined G_OS_UNIX
    pid_t IdProcCopil;

    if ((IdProcCopil = fork()) != 0) {
        /* suntem în firul copil */
        if (execlp(RPS_CALE_PSALE, RPS_NUME_PSALE, NULL) == -1) {
            g_error("Nu am putut porni aplicația 'psAle'! : %s", strerror(errno));
        }
    }
#endif
}
