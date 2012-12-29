/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "fl.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

GdkPixbuf *
fl_obtine_imagine_media_scalata(fl_media_type tp, gint w, gint h) {
    GdkPixbuf *bufImg = NULL;
    char caleMediaCompleta[256];
    GError *eroareRaportata = NULL;

    switch (tp) {
        case FL_IMG_TUSCALE_LOGO:
            strcpy(caleMediaCompleta, MEDIA_CALE_FPRINCIPAL "/tuscale_small_logo_ale.png");
            break;

        case FL_GENERAL_IMG_FORMULARE:
            strcpy(caleMediaCompleta, MEDIA_CALE_GENERAL "/psale_img_formulare.png");
            break;

        case FL_IMG_FINFO_LICENTA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/img_licenta.png");
            break;
        case FL_IMG_FINFO_INFO:
            srand(time(NULL));
            sprintf(caleMediaCompleta, MEDIA_CALE_FINFO "/img_info_%d.png", (rand() % FINFO_INFO_ID_MAX) + FINFO_INFO_ID_MIN);
            break;
        case FL_IMG_FINFO_ACTUALIZEAZA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/bt_icoana_actualizeaza_ap.png");
            break;
        case FL_IMG_FINFO_PARASESTE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/bt_icoana_paraseste_frm.png");
            break;
        case FL_IMG_FINFO_ACTUALIZARE_ESUATA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/img_actualizare_esuata.png");
            break;
        case FL_IMG_FINFO_ACTUALIZARE_SUCCES:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/img_actualizare_succes.png");
            break;
        case FL_IMG_FINFO_ACTUALIZARE_ANIM_PROGRES:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/img_actualizare_anim_in_curs.gif");
            break;
        case FL_IMG_FINFO_ACTUALIZARE_REPAUS:
            strcpy(caleMediaCompleta, MEDIA_CALE_FINFO "/img_actualizare_repaus.png");
            break;
        case FL_IMG_FCOD_TRIMITE_LA_ALE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_trimite_la_ale.png");
            break;
        case FL_IMG_FCOD_COLAPSEAZA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_colapseaza.png");
            break;
        case FL_IMG_FCOD_EXPANDEAZA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_expandeaza.png");
            break;
        case FL_IMG_FCOD_EEPROM:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_eeprom.png");
            break;
        case FL_IMG_FCOD_REIA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_reia.png");
            break;
        case FL_IMG_FCOD_SALVEAZA:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_salveaza.png");
            break;
        case FL_IMG_FCOD_PARASESTE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FCOD "/bt_icoana_paraseste.png");
            break;

        case FL_IMG_FEEPROM_SCRIE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FEEPROM "/bt_icoana_scrie_mem.png");
            break;
        case FL_IMG_FEEPROM_CITESTE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FEEPROM "/bt_icoana_citeste_mem.png");
            break;
        case FL_IMG_FEEPROM_PARASESTE:
            strcpy(caleMediaCompleta, MEDIA_CALE_FEEPROM "/bt_icoana_paraseste_frm.png");
            break;

        case FL_IMG_FVIZC_PAG_PREC:
            strcpy(caleMediaCompleta, MEDIA_CALE_FVIZCARTULIE "/bt_icoana_cartulie_pag_precedenta.png");
            break;
        case FL_IMG_FVIZC_PAG_URM:
            strcpy(caleMediaCompleta, MEDIA_CALE_FVIZCARTULIE "/bt_icoana_cartulie_pag_urmatoare.png");
            break;
        case FL_IMG_FVIZC_PAG_SALT_CUPRINS:
            strcpy(caleMediaCompleta, MEDIA_CALE_FVIZCARTULIE "/bt_icoana_cartulie_sari_la_cuprins.png");
            break;

        default:
            /* TODO: nu ar trebui să ajungă aici. Trimite o imagine implicită ? */
            break;
    }

    bufImg = gdk_pixbuf_new_from_file_at_scale(caleMediaCompleta, w, h, FALSE, &eroareRaportata);
    if (NULL != eroareRaportata) {
        g_warning("Nu am putut încărca imaginea '%s'! Motiv : %s", caleMediaCompleta, eroareRaportata->message);
        g_error_free(eroareRaportata);
        /* TODO: întoarce o imagine implcită ? */
    }

    return bufImg;
}
