/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <stdlib.h>

#include <glib/gprintf.h>

#include "al.h"

static gint al_obtine_index_mcu();

struct mcu{
  const gchar *avrdudePart;
  const gchar *devSignature; 
  gint eepromMemSize;
} mcus[] = {
  {"t25", "0x1e9108", 128},
  {"t45", "0x1e9206", 256},
  {"t85", "0x1e930B", 512},
  {NULL ,      NULL,   0}
};

gboolean 
al_este_placuta_conectata() {
  int avrdudeRet = 0;
  gchar *cDir = g_get_current_dir();
  gchar com[255];
  
  if(NULL != cDir) {
#ifdef G_OS_WIN32
  g_sprintf(com, "%s\\avrdude.exe -c usbtiny -p t25 -V", cDir);
  g_print("%s", com);
  avrdudeRet = system(com);
  
  return avrdudeRet == 0;
#elif defined G_OS_UNIX
  g_sprintf(com, "sudo %s/avrdude -c usbtiny -p t25 -V 2> /dev/null", cDir);
  avrdudeRet = system(com);
  
  if(WIFEXITED(avrdudeRet))
    return WEXITSTATUS(avrdudeRet) == 0;
#endif
    g_free(cDir);
  }
  
  return FALSE;
}

void
al_citeste_eeprom(GtkListStore *lm) {
  gchar *cDir = g_get_current_dir();
  gchar com[255];
  char hexRezultat[L_tmpnam];
  int avrdudeRet = 0;
  gint indexMCUPrezent = -1;
  gint nrRanduriInViz = 0;
  gint cnt = 0;
  gint idCol = 0;
  GtkTreeIter itrRand;
  gchar valCelula[5];
  FILE *fMem = NULL;
  
  if(NULL != cDir) {
    indexMCUPrezent = al_obtine_index_mcu();
    if(-1 == indexMCUPrezent) {
	   /* TODO: nu este bun microcontrollerul. Stop! */
	   return;
	}
	 
    tmpnam(hexRezultat);
#ifdef G_OS_WIN32
    g_sprintf(com, "%s\\avrdude.exe -c usbtiny -p %s -U eeprom:r:%s:h", cDir, mcus[indexMCUPrezent].avrdudePart, hexRezultat);
    system(com);
    /* TODO: adaugă și la windows captura de consolă */
#elif defined G_OS_UNIX
    g_sprintf(com, "sudo %s/avrdude -c usbtiny -p %s -U eeprom:r:%s:h 2>/dev/null", cDir, mcus[indexMCUPrezent].avrdudePart, hexRezultat);
    avrdudeRet = system(com);
  
    if(WIFEXITED(avrdudeRet) && WEXITSTATUS(avrdudeRet) == 0) {
      /* avem memoria în fișierul 'hexRezultat'.
       * O citim, procesăm și indexăm. */
       fMem = fopen(hexRezultat, "r");
	   nrRanduriInViz = mcus[indexMCUPrezent].eepromMemSize / 16;
	   for(cnt = 0; cnt < nrRanduriInViz; ++cnt) {
	     /* adaugă un rând nou */
	     gtk_list_store_append(lm, &itrRand);
	     
	     /* adaugă adresa mare în prima coloană */
	     g_sprintf(valCelula, "%03X", cnt * 16);
	     gtk_list_store_set(lm, &itrRand, 0, valCelula, -1);
	     
	     /* populează rândul cu valori */
	     for(idCol = 0; idCol < 16; ++idCol) {
		   fgetc(fMem);                                /* 0 */
		   fgetc(fMem);                                /* x */
		   valCelula[0] = fgetc(fMem);                 /* valoarea superioară a octetului */
		   valCelula[1] = fgetc(fMem);                 /* valoarea inferioară a octetului */
		   valCelula[2] = '\0';
		   if((cnt != nrRanduriInViz - 1 || idCol != 15) && valCelula[1] != ',') {
		     fgetc(fMem);                                /* , */
		   } else if(valCelula[1] == ',') {
		     valCelula[1] = valCelula[0];
		     valCelula[0] = '0';
		   }
		   gtk_list_store_set(lm, &itrRand, idCol + 1, valCelula, -1);
		 }
	   }
	   fclose(fMem);
    } else {
      /* TODO: buba s-a întâmplat! */
    }
#endif
    g_free(cDir);
  }
}

static gint 
al_obtine_index_mcu() {
  gint cnt = 0;
  gchar *cDir = g_get_current_dir();
  gchar com[255];
  gint indexMcu = -1;
  
#ifdef G_OS_WIN32
  g_sprintf(com, "%s\\avrdude.exe -c usbtiny -p t25 -V 2>&1", cDir);
  system(com);
  /* TODO: adaugă și la windows captura de consolă */
#elif defined G_OS_UNIX
  g_sprintf(com, "sudo %s/avrdude -c usbtiny -p t25 -V 2>&1", cDir);
  
  FILE *fConsoleOut = NULL;
  char lineBuff[4096];
  GRegex *tipar = NULL;
  GMatchInfo *containerPotriviri = NULL;
  gchar semnCompCitita[10];
  
  if((fConsoleOut = popen(com, "r")) == NULL) {
	  /* TODO: s-a întâmplat ceva cu execuția aplicației */
  } else {
	tipar = g_regex_new("Device signature = ([\\w]+)", 0, 0, NULL);
	while(fgets(lineBuff, 4096, fConsoleOut) != NULL) {
      g_regex_match(tipar, lineBuff, 0, &containerPotriviri);
      if(g_match_info_matches(containerPotriviri)) {
		g_sprintf(semnCompCitita, "%s", g_match_info_fetch(containerPotriviri, 1));
        break;
      }
    }
    g_regex_unref(tipar);
	g_match_info_free(containerPotriviri);
	pclose(fConsoleOut);
  }
#endif

  for(cnt = 0; cnt < sizeof(mcus)/sizeof(struct mcu); ++cnt) {
    if(g_strcmp0(mcus[cnt].devSignature, semnCompCitita) == 0) {
	    indexMcu = cnt;
	    break;
	}
  }
  
  g_free(cDir);
  
  return indexMcu;
}
