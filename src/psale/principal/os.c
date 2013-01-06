/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "os.h"

#include <glib/gprintf.h>
#include <stdlib.h>

#ifdef G_OS_WIN32
#elif defined G_OS_UNIX
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#endif

typedef struct {
    guint octetiInStdOut;
    guint capacitateInStdOut;
    gchar *stdOutBuff;
    guint octetiInStdErr;
    guint capacitateInStdErr;
    gchar *stdErrBuff;
} RezultatOpConsola;

static void os_elibereaza_rezultat_consola(RezultatOpConsola **rez);
static RezultatOpConsola *os_executa_comanda_si_obtine_rezultat(gchar *com);

#ifdef G_OS_WIN32

DWORD os_win_executa_com(HANDLE *hConsoleOutput, HANDLE *hConsoleInput, HANDLE *hConsoleError, gchar *command) {
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;

    ZeroMemory(&startupInfo, sizeof (startupInfo));
    ZeroMemory(&processInformation, sizeof (processInformation));

    startupInfo.cb = sizeof (startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = hConsoleOutput;
    startupInfo.hStdError = hConsoleError;
    startupInfo.hStdInput = hConsoleInput;

    if (CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation)) {
        WaitForSingleObject(processInformation.hProcess, INFINITE);

        DWORD returnCode;
        GetExitCodeProcess(processInformation.hProcess, &returnCode);
        CloseHandle(hConsoleOutput);
        CloseHandle(hConsoleError);
        CloseHandle(hConsoleInput);
        CloseHandle(processInformation.hProcess);
        CloseHandle(processInformation.hThread);

        return returnCode;
    } else {
        DWORD errorCode = GetLastError();

        return errorCode;
    }

    return -1;
}

DWORD os_win_executa_com_fara_redirectionari(gchar *command) {
    return os_win_executa_com(NULL, NULL, NULL, command);
}
#endif

static void os_elibereaza_rezultat_consola(RezultatOpConsola **rez) {
    if (*rez == NULL) return;

    if ((*rez)->stdOutBuff != NULL) g_free((*rez)->stdOutBuff);
    if ((*rez)->stdErrBuff != NULL) g_free((*rez)->stdErrBuff);
    g_free(*rez);
}

RezultatOpConsola *os_executa_comanda_si_obtine_rezultat(gchar *com) {
    g_assert(com != NULL);
    RezultatOpConsola *rezConsola = NULL;

    gint nrOctetiCititi;
    char *auxBfPointer;
#ifdef G_OS_WIN32
    HANDLE opStdOutPipe[2]; /* [0] = Read, [1] = Write */
    HANDLE opStdErrPipe[2]; /* [0] = Read, [1] = Write */
    SECURITY_ATTRIBUTES securityAttributes;

    /* construim structura de siguranță a pipe-ului */
    securityAttributes.nLength = sizeof (SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.lpSecurityDescriptor = NULL;

    /* construim efectiv țevile */
    CreatePipe(&opStdOutPipe[0], &opStdOutPipe[1], &securityAttributes, 0);
    CreatePipe(&opStdErrPipe[0], &opStdErrPipe[1], &securityAttributes, 0);

    if (os_win_executa_com(opStdOutPipe[1], NULL, opStdErrPipe[1], com) == 0) {
        rezConsola = g_new0(RezultatOpConsola, 1);

        /* golim conținutul fișierului standard de ieșire */
        rezConsola->stdOutBuff = g_new0(gchar, OS_BUCATA_CONS_BUFF);
        rezConsola->capacitateInStdOut += OS_BUCATA_CONS_BUFF;
        while (ReadFile(opStdOutPipe[0], (LPVOID) (&rezConsola->stdOutBuff[rezConsola->octetiInStdOut]), 1, (LPDWORD) (&nrOctetiCititi), NULL)) {
            if (nrOctetiCititi == 0) {
                break;
            }
            if (rezConsola->stdOutBuff[rezConsola->octetiInStdOut] == '\r') {
                rezConsola->stdOutBuff[rezConsola->octetiInStdOut] = '\0';
            } else {
                rezConsola->octetiInStdOut++;
            }
            if (rezConsola->octetiInStdOut >= rezConsola->capacitateInStdOut) {
                rezConsola->capacitateInStdOut += OS_BUCATA_CONS_BUFF;
                auxBfPointer = g_renew(gchar, rezConsola->stdOutBuff, rezConsola->capacitateInStdOut);
                rezConsola->stdOutBuff = auxBfPointer;
            }
        }
        CloseHandle(opStdOutPipe[0]);
        CloseHandle(opStdOutPipe[1]);

        /* golim conținutul fișierului standard de erori */
        rezConsola->stdErrBuff = g_new0(gchar, OS_BUCATA_CONS_BUFF);
        rezConsola->capacitateInStdErr += OS_BUCATA_CONS_BUFF;
        while (ReadFile(opStdErrPipe[0], (LPVOID) (&rezConsola->stdErrBuff[rezConsola->octetiInStdErr]), 1, (LPDWORD) (&nrOctetiCititi), NULL)) {
            if (nrOctetiCititi == 0) {
                break;
            }

            rezConsola->octetiInStdOut++;

            if (rezConsola->octetiInStdErr >= rezConsola->capacitateInStdErr) {
                rezConsola->capacitateInStdErr += OS_BUCATA_CONS_BUFF;
                auxBfPointer = g_renew(gchar, rezConsola->stdErrBuff, rezConsola->capacitateInStdErr);
                rezConsola->stdErrBuff = auxBfPointer;
            }
        }
        CloseHandle(opStdErrPipe[0]);
        CloseHandle(opStdErrPipe[1]);
    }
#elif defined G_OS_UNIX
    int stdOutFD[2];
    int stdErrFD[2];
    int stdOutVechi;
    int stdErrVechi;
    pid_t pidComanda;

    if (pipe(stdOutFD) == -1 || pipe(stdErrFD) == -1) {
        g_warning("Nu am putut crea țevile proprii de redirecționare a rezultatului : %s", strerror(errno));
    } else if ((stdOutVechi = dup(1)) == -1 || (stdErrVechi = dup(2)) == -1) {
        g_warning("Nu am putut salva descriptorii de fișier standard: %s", strerror(errno));
    } else if (close(1) == -1 || close(2) == -1) {
        g_warning("Nu am putut închide descriptorii impliciți de fișiere: %s", strerror(errno));
    } else if (dup2(stdOutFD[1], 1) == -1 || dup2(stdErrFD[1], 2) == -1) {
        g_warning("Nu am putut seta descriptorii de fișiere proprii: %s", strerror(errno));
    } else {
        if ((pidComanda = fork()) == 0) {
            close(stdOutFD[0]);
            close(stdOutFD[1]);
            close(stdErrFD[0]);
            close(stdErrFD[1]);

            if (execl("/bin/sh", "sh", "-c", com, NULL) == -1) {
                /* nu am putut lansa comanda */
                g_warning("Eroare la lansarea comenzii '%s' : %s", com, strerror(errno));
            }
        } else {
            waitpid(pidComanda, NULL, 0);
            /*if (close(0) == -1 || close(1) == -1) {
                g_warning("Nu am putut închide descriptorii de fișiere proprii (cei redirecționați): %s", strerror(errno));
            } else*/ if (dup2(stdOutVechi, 1) == -1 || dup2(stdErrVechi, 2) == -1) {
                g_warning("Nu am putut restaura vechii descriptori de fișiere: %s", strerror(errno));
            } else if (close(stdOutFD[1]) == -1 || close(stdErrFD[1]) == -1) {
                g_warning("Nu am putut închide capetele de scriere ale țevilor proprii: %s", strerror(errno));
            } else {
                rezConsola = g_new0(RezultatOpConsola, 1);

                /* golește fișierul standard de ieșire */
                rezConsola->stdOutBuff = g_new0(gchar, OS_BUCATA_CONS_BUFF);
                rezConsola->capacitateInStdOut += OS_BUCATA_CONS_BUFF;
                while ((nrOctetiCititi = read(stdOutFD[0], (gchar *) (rezConsola->stdOutBuff + rezConsola->octetiInStdOut), OS_BUCATA_CONS_BUFF)) != 0) {
                    if (nrOctetiCititi == -1) {
                        /* a apărut o eroare la citirea țevii, ne oprim aici din golire */
                        g_warning("Eroare la captura ieșirii standard : %s", strerror(errno));
                        break;
                    }
                    rezConsola->octetiInStdOut += nrOctetiCititi;
                    if (nrOctetiCititi + OS_BUCATA_CONS_BUFF >= rezConsola->capacitateInStdOut) {
                        rezConsola->capacitateInStdOut += OS_BUCATA_CONS_BUFF;
                        auxBfPointer = g_renew(gchar, rezConsola->stdOutBuff, rezConsola->capacitateInStdOut);
                        rezConsola->stdOutBuff = auxBfPointer;
                    }
                }
                close(stdOutFD[0]);

                /* golește fișierul standard de eroare */
                rezConsola->stdErrBuff = g_new0(gchar, OS_BUCATA_CONS_BUFF);
                rezConsola->capacitateInStdErr += OS_BUCATA_CONS_BUFF;
                while ((nrOctetiCititi = read(stdErrFD[0], (gchar *) (rezConsola->stdErrBuff + rezConsola->octetiInStdErr), OS_BUCATA_CONS_BUFF)) != 0) {
                    if (nrOctetiCititi == -1) {
                        /* a apărut o eroare la citirea țevii, ne oprim aici din golire */
                        g_warning("Problemă la captura erorii : %s", strerror(errno));
                        break;
                    }
                    rezConsola->octetiInStdErr += nrOctetiCititi;
                    if (rezConsola->octetiInStdErr + OS_BUCATA_CONS_BUFF >= OS_BUCATA_CONS_BUFF) {
                        rezConsola->capacitateInStdErr += OS_BUCATA_CONS_BUFF;
                        auxBfPointer = g_renew(gchar, rezConsola->stdErrBuff, rezConsola->capacitateInStdErr);
                        rezConsola->stdErrBuff = auxBfPointer;
                    }
                }
                close(stdErrFD[0]);
            }
        }
    }
#endif

    return rezConsola;
}

gboolean os_executa_functie_asincron(gpointer fct, gpointer param) {
#ifdef G_OS_WIN32 
    if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) (fct), (LPVOID) (param), 0, NULL) == NULL) {
        g_warning("Nu am putut lansa în execuție funcția solicitată! Motiv : %ld", GetLastError());

        return FALSE;
    }
#elif defined G_OS_UNIX
    GThread *firCautatorDeActualizari = NULL;
    GError *err = NULL;

    if ((firCautatorDeActualizari = g_thread_try_new("cautActualizari", (GThreadFunc) (fct), param, &err)) == NULL) {
        g_warning("Nu am putut lansa în execuție funcția dorită! Motiv : %s", err->message);
        g_error_free(err);

        return FALSE;
    }
#endif

    return TRUE;
}

unsigned long os_system(gchar *command) {
    g_debug("Execut următoarea comandă : %s", command);
#ifdef G_OS_WIN32
    return os_win_executa_com_fara_redirectionari(command);
#elif defined G_OS_UNIX
    int retVal = system(command);

    if (WIFEXITED(retVal)) {
        return WEXITSTATUS(retVal);
    } else {
        return -1;
    }
#endif
}

gboolean os_compilator_executa_si_completeaza_bic_fc(gchar *comanda, BaraInfoCod *baraInfoTinta) {
    g_assert(comanda != NULL);
    g_assert(baraInfoTinta != NULL);

    char *ptLinie = NULL;
    char *tmpContinut = NULL;
    gboolean gccCompilatCuSucces = TRUE;
    RezultatOpConsola *rezOperatiunii = NULL;

    if ((rezOperatiunii = os_executa_comanda_si_obtine_rezultat(comanda)) != NULL && rezOperatiunii->octetiInStdErr != 0) {
        bic_text_initializeaza(baraInfoTinta);
        tmpContinut = (char *) strdup(rezOperatiunii->stdErrBuff);
        ptLinie = strtok(tmpContinut, "\n");
        while (NULL != ptLinie) {
            bic_text_adauga_linie(baraInfoTinta, ptLinie);
            ptLinie = strtok(NULL, "\n");
        }

        gccCompilatCuSucces = FALSE;
    }

    os_elibereaza_rezultat_consola(&rezOperatiunii);
    return gccCompilatCuSucces;
}

Versiune *os_rpsale_obtine_versiune_server() {
    Versiune *verUltimapsAle = NULL;
    char comandaAp[256];
    RezultatOpConsola *rezOperatiunii = NULL;

    sprintf(comandaAp, "\"%s\" --tipul-operatiei afiseaza --fara-ig --versiune-server", OS_CALE_RPSALE);
    if ((rezOperatiunii = os_executa_comanda_si_obtine_rezultat(comandaAp)) != NULL && rezOperatiunii->octetiInStdOut != 0) {
        verUltimapsAle = sda_obtineVersiuneDinSir(rezOperatiunii->stdOutBuff);
    }

    os_elibereaza_rezultat_consola(&rezOperatiunii);
    return verUltimapsAle;
}

gboolean os_rpsale_forteaza_actualizare(Versiune *vers) {
    gboolean stareExecutieActualizator = TRUE;

#ifdef G_OS_WIN32
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;
    gchar listaArgumente[5120];

    ZeroMemory(&startupInfo, sizeof (startupInfo));
    ZeroMemory(&processInformation, sizeof (processInformation));

    startupInfo.cb = sizeof (startupInfo);

    g_sprintf(listaArgumente, "--tipul-operatiei actualizeaza-direct --versiune-server%s", sda_versiune_printf(vers));
    if (!CreateProcess(OS_CALE_RPSALE, listaArgumente, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation)) {
        g_warning("Nu am putut porni actualizatorul 'rpsAle'!");
        stareExecutieActualizator = FALSE;
    }
#elif defined G_OS_UNIX
    pid_t IdProcCopil;
    gchar optVersTinta[32];

    g_sprintf(optVersTinta, "--versiune-server=%s", sda_versiune_printf(vers));
    if ((IdProcCopil = fork()) != 0) {
        /* suntem în firul copil */
        if (execlp(OS_CALE_RPSALE, OS_NUME_RPSALE, "--tipul-operatiei", "actualizeaza-direct", optVersTinta, NULL) == -1) {
            g_warning("Nu am putut porni actualizatorul 'rpsAle'! : %s", strerror(errno));
            stareExecutieActualizator = FALSE;
        }
    }
#endif

    return stareExecutieActualizator;
}

void os_obtine_nume_fis_temporar(gchar *buff, gint buffLen) {
    g_assert(buffLen > L_tmpnam + 20);

#ifdef G_OS_WIN32
    gchar tempDir[MAX_PATH];
    gint dimSirTempDir = 0;

    if (GetTempPath(sizeof (tempDir) / sizeof (gchar), tempDir) == 0) {
        g_sprintf(tempDir, ".");
    } else {
        dimSirTempDir = strlen(tempDir);
        if (tempDir[dimSirTempDir - 1] == '\\') {
            tempDir[dimSirTempDir - 1] = '\0';
        }
    }
    g_sprintf(buff, "%s%s", tempDir, tmpnam(NULL));
#elif defined G_OS_UNIX
    g_sprintf(buff, "%s", tmpnam(NULL));
#endif
    g_debug("Am creeat fișierul temporal '%s'", buff);
}

extern gchar *os_avrdude_obtine_cod_mcu_prezent(gchar *comanda) {
    gchar *codMCUGasit = NULL;
    GRegex *tipar = NULL;
    GMatchInfo *containerPotriviri = NULL;
    RezultatOpConsola *rezOperatiunii = NULL;

    tipar = g_regex_new("Device signature = ([\\w]+)", 0, 0, NULL);
    codMCUGasit = g_new0(gchar, 10);
    if ((rezOperatiunii = os_executa_comanda_si_obtine_rezultat(comanda)) != NULL) {
        if (rezOperatiunii->octetiInStdErr != 0) {
            /* datorită modului de construcție a lui AvrDude (varsă rezultate bune pe stderr), aici ne așteptăm să citim rezultatele sale */
            g_regex_match(tipar, rezOperatiunii->stdErrBuff, 0, &containerPotriviri);
            if (g_match_info_matches(containerPotriviri)) {
                g_sprintf(codMCUGasit, "%s", g_match_info_fetch(containerPotriviri, 1));
            }
        }
    } else {
        g_warning("S-a ivit o problemă la invocarea lui 'avrdude' : '%s'! Acțiunea a fost anulată ...", comanda);
    }

    g_regex_unref(tipar);
    g_match_info_free(containerPotriviri);
    os_elibereaza_rezultat_consola(&rezOperatiunii);

    return codMCUGasit;
}
