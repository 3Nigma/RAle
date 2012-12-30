/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef _FL_H_ /* 'file link' */
#define _FL_H_

#include <gtk/gtk.h>

#define CALE_DIRECTOR_MEDIA        "./media"
#define MEDIA_CALE_GENERAL         CALE_DIRECTOR_MEDIA "/general"
#define MEDIA_CALE_FPRINCIPAL      CALE_DIRECTOR_MEDIA "/fprincipal"
#define MEDIA_CALE_FINFO           CALE_DIRECTOR_MEDIA "/finfo"
#define MEDIA_CALE_FCOD            CALE_DIRECTOR_MEDIA "/fcod"
#define MEDIA_CALE_FEEPROM         CALE_DIRECTOR_MEDIA "/feeprom"
#define MEDIA_CALE_FVIZCARTULIE    CALE_DIRECTOR_MEDIA "/fvizcartulie"

#define FINFO_INFO_ID_MIN          1
#define FINFO_INFO_ID_MAX          10


typedef enum {
  FL_IMG_TUSCALE_LOGO,
  
  FL_GENERAL_IMG_FORMULARE,
  
  FL_IMG_FINFO_LICENTA,
  FL_IMG_FINFO_INFO,
  FL_IMG_FINFO_PARASESTE,
  FL_IMG_FINFO_ACTUALIZARE_ESUATA,
  FL_IMG_FINFO_ACTUALIZARE_SUCCES_VNOUA,
          FL_IMG_FINFO_ACTUALIZARE_SUCCES_VNENOUA,
  FL_IMG_FINFO_ACTUALIZARE_REPAUS,
          
  FL_IMG_FCOD_TRIMITE_LA_ALE,
  FL_IMG_FCOD_COLAPSEAZA,
  FL_IMG_FCOD_EXPANDEAZA,
  FL_IMG_FCOD_EEPROM,
  FL_IMG_FCOD_REIA,
  FL_IMG_FCOD_SALVEAZA,
  FL_IMG_FCOD_PARASESTE,
  
  FL_IMG_FEEPROM_SCRIE,
  FL_IMG_FEEPROM_CITESTE,
  FL_IMG_FEEPROM_PARASESTE,
  
  FL_IMG_FVIZC_PAG_PREC,
  FL_IMG_FVIZC_PAG_URM,
  FL_IMG_FVIZC_PAG_SALT_CUPRINS
} FLImgTipMedia;

typedef enum {
    FL_ANIM_FINFO_ACTUALIZARE_PROGRES
} FLAnimTipMedia;

extern GdkPixbuf *fl_obtine_imagine_media_scalata(FLImgTipMedia tp, gint w, gint h);
extern GdkPixbufAnimation *fl_obtine_animatie_media(FLAnimTipMedia tp);

#endif
