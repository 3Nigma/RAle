/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _OS_H_
#define _OS_H_

#include <gtk/gtk.h>

#include "bic.h"
#include "dl.h"

#define OS_NUME_RPSALE "rpsale"
#define OS_CALE_RPSALE "./"OS_NUME_RPSALE
#define OS_BUCATA_CONS_BUFF 4096

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
os_executa_actualizator(const IntrareActualizare *ia);

extern gboolean
os_executa_si_completeaza_bic_fc(gchar *comanda, BaraInfoCod *baraInfoTinta);

extern void
os_obtine_nume_fis_temporar(gchar *buff, gint buffLen);

extern gchar *
os_obtine_cod_mcu_prezent(gchar *comanda);

#endif
