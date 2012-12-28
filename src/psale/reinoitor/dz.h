/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef DZ_H
#define	DZ_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>
#include <errno.h>
#include "sda.h"

#define DZ_CALE_DESPACHETARE "."

    extern gboolean dz_despacheteaza_pachet(const char *numeArhiva, const char *cale);

#ifdef	__cplusplus
}
#endif

#endif	/* DZ_H */

