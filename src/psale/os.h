#ifndef _OS_H_
#define _OS_H_

#include <gtk/gtk.h>

#include "bic.h"

#ifdef G_OS_WIN32
#include <windows.h>

extern DWORD 
os_win_executa_com(HANDLE *hConsoleOutput, HANDLE *hConsoleInput, HANDLE *hConsoleError, gchar *command);

extern DWORD 
os_win_executa_com_fara_redirectionari(gchar *command);

#endif

extern unsigned long 
os_system(gchar *command);

extern gboolean
os_executa_si_completeaza_bic_fc(gchar *comanda, BaraInfoCod *baraInfoTinta);

extern void 
os_obtine_nume_fis_temporar(gchar *buff, gint buffLen);

#endif