/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef PAR_APLICATIE_H
#define	PAR_APLICATIE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>
#include "sda.h"
    
enum OperatieInstanta {
    INVALID, ACTUALIZEAZA_DIRECT, ACTUALIZEAZA_NORMAL, AFISEAZA
};

typedef struct {
    enum OperatieInstanta tipOp;
    gboolean folsesteIG;
    Versiune *versServer;
} ParametriiRulareAplicatie;

extern ParametriiRulareAplicatie *pa_incarca_parametrii(int argc, char *argv[]);
extern void pa_curata(ParametriiRulareAplicatie **pr);

extern void pa_afiseaza_meniu();

#ifdef	__cplusplus
}
#endif

#endif	/* PAR_APLICATIE_H */

