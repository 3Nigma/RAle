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

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

#include "os.h"
#include "al.h"

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

static gint al_obtine_index_mcu();
static gchar *pune_eeprom_in_fis_temporar(GtkListStore *lm);

gboolean 
al_este_placuta_conectata() {
  gchar *cDir = g_get_current_dir();
  gchar avrDudeCom[1024];
  
  if(NULL != cDir) {
#ifdef G_OS_WIN32
  g_sprintf(avrDudeCom, "\"%s\\avrdude.exe\" -c usbtiny -p t25 -V", cDir);
#elif defined G_OS_UNIX
  g_sprintf(avrDudeCom, "sudo \"%s/avrdude\" -c usbtiny -p t25 -V 2> /dev/null", cDir);
#endif
    g_free(cDir);
  }
  
  return os_system(avrDudeCom) == 0;
}

gboolean 
al_scrie_aplicatie(const gchar *caleFisHex) {
  gchar *cDir = g_get_current_dir();
  gchar avrDudeCom[1024];
  
  if(NULL != cDir) {
#ifdef G_OS_WIN32
    g_sprintf(avrDudeCom, "\"%s\\avrdude.exe\" -c usbtiny -p t25 -U flash:w:\"%s\":i", cDir, caleFisHex);
#elif defined G_OS_UNIX
    g_sprintf(avrDudeCom, "sudo \"%s/avrdude\" -c usbtiny -p t25 -U flash:w:\"%s\":i 2> /dev/null", cDir, caleFisHex);
#endif
    g_free(cDir);
  }
  
  return os_system(avrDudeCom) == 0;
}

void
al_citeste_eeprom(GtkListStore *lm) {
  gchar *cDir = g_get_current_dir();
  gchar com[255];
  char hexRezultat[L_tmpnam];
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
	   g_warning("Nu recunosc microcontrollerul atașat! Nu pot continua ...");
	   return;
	}
	 
    tmpnam(hexRezultat);
#ifdef G_OS_WIN32
    g_sprintf(com, "\"%s\\avrdude.exe\" -c usbtiny -p %s -U eeprom:r:\"%s\":h", cDir, mcus[indexMCUPrezent].avrdudePart, hexRezultat);
#elif defined G_OS_UNIX
    g_sprintf(com, "sudo \"%s/avrdude\" -c usbtiny -p %s -U eeprom:r:\"%s\":h 2>/dev/null", cDir, mcus[indexMCUPrezent].avrdudePart, hexRezultat);
#endif
    if(os_system(com) == 0) {
	  /* avem memoria în fișierul 'hexRezultat'.
       * O citim, procesăm și indexăm. */
       fMem = fopen(hexRezultat, "r");
	   nrRanduriInViz = mcus[indexMCUPrezent].eepromMemSize / 16;
	   gtk_list_store_clear(lm);
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
		   if((cnt != nrRanduriInViz - 1 || idCol != 15) && 
		      g_ascii_isxdigit(valCelula[1])) {
		     fgetc(fMem);                                /* , */
		   } else if(g_ascii_isxdigit(valCelula[1]) == FALSE) {
		     valCelula[1] = valCelula[0];
		     valCelula[0] = '0';
		   }
		   gtk_list_store_set(lm, &itrRand, idCol + 1, valCelula, -1);
		 }
	   }
	   fclose(fMem);
	   remove(hexRezultat);
    } else {
      g_warning("Nu am putut obține memoria EEPROM pentru că nu am putut executa aplicația 'avrdude'!");
    }
    g_free(cDir);
  }
}

void 
al_scrie_eeprom(GtkListStore *lm) {
  gchar *fisTempHex = NULL;
  gchar *cDir = g_get_current_dir();
  gchar com[255];
  gint indexMCUPrezent = -1;
  
  if(NULL != cDir) {
    indexMCUPrezent = al_obtine_index_mcu();
    if(-1 == indexMCUPrezent) {
	  g_warning("Nu recunosc microcontrollerul atașat! Nu pot continua ...");
	  return;
	}
	
    fisTempHex = pune_eeprom_in_fis_temporar(lm);
  
#ifdef G_OS_WIN32
    g_sprintf(com, "\"%s\\avrdude.exe\" -c usbtiny -p %s -U eeprom:w:\"%s\":r", cDir, mcus[indexMCUPrezent].avrdudePart, fisTempHex);
#elif defined G_OS_UNIX
    g_sprintf(com, "sudo \"%s/avrdude\" -c usbtiny -p %s -U eeprom:w:\"%s\":r 2>/dev/null", cDir, mcus[indexMCUPrezent].avrdudePart, fisTempHex);
#endif
    /* trimitem memoria rezultată spre plăcuță */
    os_system(com);
  }
  
  remove(fisTempHex);
  g_free(fisTempHex);
}

static gchar *
pune_eeprom_in_fis_temporar(GtkListStore *lm) {
  gchar *fisTemporar = NULL;
  gchar *memFormatata = NULL;
  GtkTreeModel *tm = NULL;
  GtkTreeIter trIter;
  gchar *infoCelula;
  gint indexCol = 0;
  FILE *fHexTemp = NULL;
  char continutMem[2048];
  int indexContinutMem = 0;
  
  /* creem fișierul temporal și deschidem canalul de salvare */
  fisTemporar = g_new(gchar, 512);
  tmpnam(fisTemporar);
  fHexTemp = fopen(fisTemporar, "wb");
  
  /* formatează memoria curentă în formatul dorit de avrdude */
  memFormatata = g_new(gchar, 4096);
  tm = GTK_TREE_MODEL(lm);
  gtk_tree_model_get_iter_first(tm, &trIter); 
  do {
    for(indexCol = 1; indexCol < 17; ++indexCol) {
	  gtk_tree_model_get(tm, &trIter, indexCol, &infoCelula, -1);
	  
	  /* avem informația din celula curentă.
	   * O adăugăm în tamponul nostru */
	  continutMem[indexContinutMem++] = (int)strtol(infoCelula, NULL, 16);
	  g_free(infoCelula);
	}
  } while(gtk_tree_model_iter_next(tm, &trIter));
  
  /* scriem tamponul în fișier */
  fwrite(continutMem, sizeof(char), indexContinutMem, fHexTemp);
  
  fclose(fHexTemp);
  g_free(memFormatata);
  
  return fisTemporar;
}

static gint 
al_obtine_index_mcu() {
  gint cnt = 0;
  gchar *cDir = g_get_current_dir();
  gchar comAvrDude[255];
  gint indexMcu = -1;
  gchar *semnCompCitita = NULL;
  
#ifdef G_OS_WIN32
  g_sprintf(comAvrDude, "\"%s\\avrdude.exe\" -c usbtiny -p t25 -V 2>&1", cDir);
#elif defined G_OS_UNIX
  g_sprintf(comAvrDude, "sudo \"%s/avrdude\" -c usbtiny -p t25 -V 2>&1", cDir);
#endif
  semnCompCitita = os_obtine_cod_mcu_prezent(comAvrDude);
  for(cnt = 0; cnt < sizeof(mcus)/sizeof(struct mcu); ++cnt) {
    if(g_strcmp0(mcus[cnt].devSignature, semnCompCitita) == 0) {
	    indexMcu = cnt;
	    break;
	}
  }
  
  g_free(cDir);
  g_free(semnCompCitita);
  
  return indexMcu;
}
