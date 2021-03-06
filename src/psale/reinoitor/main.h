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

#ifdef G_OS_WIN32
#if       _WIN32_WINNT < 0x0500
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT   0x0500
#endif  /* necesar pentru funcția 'GetConsoleWindow' */
#include <windows.h>
#endif

#include "sda.h"
#include "db.h"
#include "par_aplicatie.h"
#include "fprimarulare.h"
#include "factualizare.h"

#include <errno.h>

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

