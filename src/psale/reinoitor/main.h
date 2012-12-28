/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "sda.h"
#include "db.h"
#include "par_aplicatie.h"
#include "fprimarulare.h"
#include "factualizare.h"

#ifdef G_OS_WIN32
#include <windows.h>
#elif defined G_OS_UNIX
#include <unistd.h>
#include <errno.h>
#endif

#define RPS_NUME_PSALE "psale"
#define RPS_CALE_PSALE "./"RPS_NUME_PSALE


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

