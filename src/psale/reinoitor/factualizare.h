/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef FACTUALIZARE_H
#define	FACTUALIZARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#include "par_aplicatie.h"
#include "dl.h"

#define FA_MS_INTRE_STARI 1000

    typedef enum {
        FA_DL_INITIALIZARE,
        FA_DL_DESC_LISTA_ACTUALIZARI,
        FA_DL_CANALIZEAZA_ACTIUNE,
        FA_DL_PREGATESTE_VERSIUNE_TINTA,
        FA_DL_VERIFICA_VERSIUNE,
        FA_DL_DESC_ACTUALIZARE,
        FA_DL_APLICARE_ACTUALIZARE,

        FA_DL_ATENTIE_MODUL_DL_NEINITIALIZAT,
        FA_DL_ATENTIE_LISTA_ACTUALIZARI_NEDESCARCATA,
        FA_DL_ATENTIE_LISTA_ACTUALIZARI_GOALA,
        FA_DL_ATENTIE_NU_RECUNOSC_ACTIUNEA,
        FA_DL_ATENTIE_NU_GASESC_VERSIUNEA_TINTA,
        FA_DL_ATENTIE_PROBLEME_LA_DESCARCARE,
        FA_DL_ATENTIE_PROBLEME_LA_APLICARE,

        FA_DL_AFISARE_CU_SUCCES,
        FA_DL_VERSIUNE_ESTE_ULTIMA,
        FA_DL_AM_ACTUALIZAT_LA_VERS_TINTA,

        FA_DL_FINALIZARE,
        FA_DL_ANULARE
    } StareActualizare;

    typedef struct {
        StareActualizare faza;
        DLInstanta *date;
    } AutomatonActualizare;

    typedef struct {
        GtkWidget *frm;
        GtkWidget *baraProgres;
        GtkWidget *btAnuleaza;
        gboolean esteBtAnuleazaApasat;
        gboolean actualizareAplicataCuSucces;
        AutomatonActualizare *masinaActualizanta;
        ParametriiRulareAplicatie *parametriInvocare;
    } FAInstanta;

    extern FAInstanta *fa_initializeaza(ParametriiRulareAplicatie *pari);
    extern void fa_curata(FAInstanta **fai);

    extern gboolean fa_aplica_sesiune_actualizare(FAInstanta *fai);

#ifdef	__cplusplus
}
#endif

#endif	/* FACTUALIZARE_H */

