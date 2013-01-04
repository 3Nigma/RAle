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

static void lanseazaPSAle(gboolean actualizatCuSucces);
static void inregistreazaMesaj(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);

int main(int argc, char *argv[]) {
    FILE *fMesajAplicatie = NULL;

    /* redirecționăm toate mesajele de stare ale aplicației într-un fișier fizic */
    if ((fMesajAplicatie = fopen("mesaje_sesiune_" OS_NUME_RPSALE ".txt", "w")) != NULL) {
        g_log_set_handler("",
                G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION,
                inregistreazaMesaj, fMesajAplicatie);
    }

    /* pentru Windows, la operațiunile de actualizare, ascundeți consola și păstrați doar IG-ul, dar
     * doar dacă aplicația a fost lansată individual și nu dintr-o consolă părinte. */
#ifdef G_OS_WIN32
    HWND consoleWnd;
    DWORD dwProcessId;

    consoleWnd = GetConsoleWindow();
    GetWindowThreadProcessId(consoleWnd, &dwProcessId);
    if (GetCurrentProcessId() == dwProcessId) {
        g_debug("Aplicația a fost lansată din exterior, avându-se asociată o consolă unică.");
        ShowWindow(consoleWnd, SW_HIDE);
    } else {
        g_debug("Aplicația a fost lansată din din cadrul altei console.");
    }
#endif

    gtk_init(&argc, &argv);

    ParametriiRulareAplicatie *optiuniExterne = NULL;
    gboolean pachetActualizatCuSucces = FALSE;
    
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
                g_debug("Aceasta fie nu este prima execuție a aplicației, fie procesul de verificare a fost sărit.");
            }

            if (db_obtine_este_actualizare_automata() ||
                    optiuniExterne->tipOp == ACTUALIZEAZA_DIRECT ||
                    optiuniExterne->tipOp == AFISEAZA) {
                FAInstanta *fSesiuneActualizare = fa_initializeaza(optiuniExterne);
                
                pachetActualizatCuSucces = fa_aplica_sesiune_actualizare(fSesiuneActualizare);
                fa_curata(&fSesiuneActualizare);
            }
            db_curata();
        } else {
            g_warning("Nu am putut inițializa modulul bazei de date. Procesul de actualizare nu a putut avea loc!");
        }

        if (optiuniExterne->tipOp != AFISEAZA) {
            lanseazaPSAle(pachetActualizatCuSucces);
        }

        pa_curata(&optiuniExterne);
    } else {
        g_warning("S-a dorit lansarea cu parametrii din linia de comandă, dar operația nu s-a putut finaliza pentru că au existat probleme la interpretarea parametrilor!");
    }

    if (fMesajAplicatie != NULL) fclose(fMesajAplicatie);

    return 0;
}

static void lanseazaPSAle(gboolean actualizatCuSucces) {
#ifdef G_OS_WIN32
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;

    ZeroMemory(&startupInfo, sizeof (startupInfo));
    ZeroMemory(&processInformation, sizeof (processInformation));

    startupInfo.cb = sizeof (startupInfo);

    if (!CreateProcess(RPS_CALE_PSALE, (actualizatCuSucces ? "--proaspat-actualizata" : NULL), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation)) {
        g_error("Nu am putut porni aplicația 'psAle'!");
    }
#elif defined G_OS_UNIX
    pid_t IdProcCopil;

    if ((IdProcCopil = fork()) != 0) {
        /* suntem în firul copil */
        if (execlp(RPS_CALE_PSALE, RPS_NUME_PSALE, (actualizatCuSucces ? "--proaspat-actualizata" : NULL), NULL) == -1) {
            g_error("Nu am putut porni aplicația 'psAle'! : %s", strerror(errno));
        }
    }
#endif
}

static void inregistreazaMesaj(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {
    g_assert(user_data != NULL);

    gchar *mesajDeAdaugat = NULL;
    gchar tipMesaj[20];
    FILE *fSesiuneMesaje = (FILE *) (user_data);
    time_t timpIntroducere;

    if ((mesajDeAdaugat = g_new0(gchar, strlen(message) + 5)) != NULL) {
        timpIntroducere = time(NULL);
        switch (log_level) {
            case G_LOG_FLAG_FATAL:
                g_sprintf(tipMesaj, "FATAL");
                break;
            case G_LOG_LEVEL_ERROR:
                g_sprintf(tipMesaj, "EROARE");
                break;
            case G_LOG_LEVEL_CRITICAL:
                g_sprintf(tipMesaj, "CRITIC");
                break;
            case G_LOG_LEVEL_WARNING:
                g_sprintf(tipMesaj, "ATENTIE");
                break;
            case G_LOG_LEVEL_MESSAGE:
                g_sprintf(tipMesaj, "MESAJ");
                break;
            case G_LOG_LEVEL_INFO:
                g_sprintf(tipMesaj, "INFO");
                break;
            case G_LOG_LEVEL_DEBUG:
                g_sprintf(tipMesaj, "ANALIZA");
                break;
            default:
                g_sprintf(tipMesaj, "TIP %d", log_level);
                break;
        }

        fprintf(fSesiuneMesaje, "[%ld, %s] %s\n", timpIntroducere, tipMesaj, message);

        g_free(mesajDeAdaugat);
    } else {
        fprintf(fSesiuneMesaje, "EROARE FATALĂ: Nu am putut aloca spațiul necesar pentru găzduirea mesajului de înregistrare!\n");
    }
}
