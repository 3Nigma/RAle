/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef FPRIMARULARE_H
#define	FPRIMARULARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

    enum FPRRaspunsActualizare {
        FPR_RASPUNS_MAI_TARZIU, FPR_RASPUNS_MANUAL, FPR_RASPUNS_AUTOMAT
    };

    typedef struct {
        GtkWidget *frm;
        enum FPRRaspunsActualizare tipActDorit;
    } FPRInstanta;

    extern FPRInstanta *fpr_instantiaza();
    extern void fpr_elibereaza(FPRInstanta **fpri);
    
    extern enum FPRRaspunsActualizare fpr_lanseaza_si_asteapta_raspuns(FPRInstanta *fpri);
    
#ifdef	__cplusplus
}
#endif

#endif	/* FPRIMARULARE_H */

