/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */
 
#include <assert.h>
#include <string.h>

#include "curl/curl.h"
#include <glib/gprintf.h>
#include <zip.h>
#include <stdlib.h>

#ifdef G_OS_WIN32
  #include <direct.h>
#elif defined G_OS_UNIX
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

#include "db.h"
#include "dl.h"

static gboolean dl_despacheteaza_pachet_in_situ(const char *numeArhiva);
static FILE *fopen_mkdir(const char *caleFis, const char *mode);
static size_t citeste_date_actualizare_recurent(void *ptr, size_t size, size_t nmemb, void *userdata);
static gboolean dl_descarca_fisier(const char *adresa, FILE *fisTinta);

static FILE *pFisVersActuala;
static IntrareActualizare *intrareaCeaMaiRecenta = NULL;
static char adrListaActualizare[256];

gboolean 
dl_initializeaza(const char *adresa) {
  strcpy(adrListaActualizare, adresa);
  pFisVersActuala = tmpfile();

  intrareaCeaMaiRecenta = (IntrareActualizare *)malloc(sizeof(IntrareActualizare));
  intrareaCeaMaiRecenta->adrPachetNou = (char *)malloc(1024 * sizeof(char));
  intrareaCeaMaiRecenta->vers = 0.0;
  intrareaCeaMaiRecenta->mesajModificari = (char *)malloc(4096 * sizeof(char));
  
  strcpy(intrareaCeaMaiRecenta->adrPachetNou, "\0");
  strcpy(intrareaCeaMaiRecenta->mesajModificari, "\0");

  return pFisVersActuala != NULL;
}

void 
dl_curata() {
  if(NULL != pFisVersActuala)  {
    fclose(pFisVersActuala);
    remove(DL_TEMP_ARHIVA);
  
    if(NULL != intrareaCeaMaiRecenta) {
	  free(intrareaCeaMaiRecenta->adrPachetNou);
	  free(intrareaCeaMaiRecenta->mesajModificari);
	  free(intrareaCeaMaiRecenta);
	}
  }
}

gboolean 
dl_exista_versiune_mai_buna_decat(double versCurentaLocal) {
  g_assert(NULL != pFisVersActuala);
  g_assert(NULL != intrareaCeaMaiRecenta);
  
  gchar buffLinie[4096];
  GRegex *tipar = NULL;
  GMatchInfo *containerPotriviri = NULL;
  gboolean existaActualizare = FALSE;
  
  dl_descarca_fisier(adrListaActualizare, pFisVersActuala);
  g_debug("Am descărcat fișierul-listă de actualizări de la adresa '%s'.", adrListaActualizare);
  rewind(pFisVersActuala);
  tipar = g_regex_new("([0-9\\.]+):\"([http|www]\\S+)\":\"(.+)\"\\\\0", 0, 0, NULL);
  while(fgets(buffLinie, sizeof(buffLinie), pFisVersActuala) != NULL) {
	/* extragem informația utilă pentru verificarea versiunii */
	g_debug("Linia curentă interpretată : %s", buffLinie);
	g_regex_match(tipar, buffLinie, 0, &containerPotriviri);
	if(g_match_info_matches(containerPotriviri)) {
	  /* [1] = versiune, [2] = adresă de descărcare, [3] = mesaj modificări */
      intrareaCeaMaiRecenta->vers = g_ascii_strtod(g_match_info_fetch(containerPotriviri, 1), NULL);
      if(intrareaCeaMaiRecenta->vers > versCurentaLocal) {
        /* păstrăm entitățile necesare */
        g_sprintf(intrareaCeaMaiRecenta->adrPachetNou, "%s", g_match_info_fetch(containerPotriviri, 2));
        g_sprintf(intrareaCeaMaiRecenta->mesajModificari, "%s", g_match_info_fetch(containerPotriviri, 3));
        
        existaActualizare = TRUE;
        break;
      }
    }
  }

  g_regex_unref(tipar);
  g_match_info_free(containerPotriviri);

  return existaActualizare;
}

extern double
dl_obtine_vers_curenta_server() {
  return intrareaCeaMaiRecenta->vers;
}

gboolean 
dl_actualizeaza_aplicatia() {
  g_assert(NULL != pFisVersActuala);
  g_assert(strlen(intrareaCeaMaiRecenta->adrPachetNou) != 0);
  g_assert(strlen(intrareaCeaMaiRecenta->mesajModificari) != 0);
 
  /* ajungând aici avem certitudinea că există o versiunea de aplicație mai bună decât cea locală,
   * iar datele necesare sunt încărcate în variabilele de modul. */
   
  FILE *tempFisArhiva = NULL;
  gboolean rezultatOp = FALSE;

  if((tempFisArhiva = fopen(DL_TEMP_ARHIVA, "wb")) != NULL) {
    if(dl_descarca_fisier(intrareaCeaMaiRecenta->adrPachetNou, tempFisArhiva)) {
	  fclose(tempFisArhiva);
      /* am obținut arhiva dorită. mergi mai departe și despacheteaz-o */
      if(dl_despacheteaza_pachet_in_situ(DL_TEMP_ARHIVA)) {
	    rezultatOp = TRUE;
	  }
      remove(DL_TEMP_ARHIVA);
    }
  }
  
  return rezultatOp;
}

static gboolean
dl_despacheteaza_pachet_in_situ(const char *numeArhiva) {
  struct zip *zFile = NULL;
  gboolean stareOp = FALSE;
  
  /* deschidem arhiva */
  if((zFile = zip_open(numeArhiva, 0, NULL)) != NULL) {
    zip_uint64_t nrFisInArhiva = zip_get_num_entries(zFile, 0);
    zip_uint64_t fIndex = 0;
    struct zip_file *fisInternArhiva = NULL;
    struct zip_stat fZStat;
    zip_uint64_t restOctetiDeDecompresat = 0;
    FILE *fisDecompresat = NULL;
    char buffTransfer[2048];
    
    for(fIndex = 0; fIndex < nrFisInArhiva; ++fIndex) {
	  /* obține informații despre fișierul cu indexul curent */
	  zip_stat_init(&fZStat);
	  zip_stat_index(zFile, fIndex, 0, &fZStat);
	  
	  /* despachetează fișierul */
	  fisInternArhiva = zip_fopen_index(zFile, fIndex, 0);
	  fisDecompresat = fopen_mkdir(fZStat.name, "wb");
	  restOctetiDeDecompresat = fZStat.size;
	  do {
        /* preluăm din arhivă și punem în fișierul despachetat */
		if(restOctetiDeDecompresat > sizeof(buffTransfer)) {
		  zip_fread(fisInternArhiva, buffTransfer, sizeof(buffTransfer));
		  fwrite(buffTransfer, sizeof(char), sizeof(buffTransfer), fisDecompresat);
		  restOctetiDeDecompresat -= sizeof(buffTransfer);
		} else {
		   zip_fread(fisInternArhiva, buffTransfer, restOctetiDeDecompresat);
		   fwrite(buffTransfer, sizeof(char), restOctetiDeDecompresat, fisDecompresat);
		   restOctetiDeDecompresat = 0;
		}
		
	  } while(restOctetiDeDecompresat != 0);
	  
	  /* am încheiat operația. Putem închide fișierele */
	  zip_fclose(fisInternArhiva);
	  fclose(fisDecompresat);
	}
	
    /* curățăm memoria */
    zip_close(zFile);

    stareOp = TRUE;
  } else {
    g_warning("Nu am putut deschide arhiva pentru despachetare!");
  }

  return stareOp;
}

static FILE *
fopen_mkdir(const char *caleFis, const char *mode) {
  char *pCaleOriginala = strdup(caleFis);
  char delimitatorDirector;
  char *pCale = pCaleOriginala;
  
  while((*pCale) != '\0') {
	if((*pCale) == '\\' || (*pCale) == '/') {
	  delimitatorDirector = (*pCale);
	  (*pCale) = '\0';
	  #ifdef G_OS_WIN32
        _mkdir(pCaleOriginala);
      #elif defined G_OS_UNIX
        mkdir(pCaleOriginala, S_IRWXG | S_IRWXO | S_IRWXU);
      #endif
      (*pCale) = delimitatorDirector;
    }
    pCale += sizeof(char);
  }
  free(pCaleOriginala);

  return fopen(caleFis, mode);
}

static size_t 
citeste_date_actualizare_recurent(void *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t octetiScrisi = fwrite(ptr, size, nmemb, (FILE *)userdata);

  return octetiScrisi;
}

static int 
instiintare_progres_retea(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
  g_debug("Progresul actual: %f/%f", dlnow, dltotal);
  return 0;
}

static gboolean
dl_descarca_fisier(const char *adresa, FILE *fisTinta) {
  CURL *legatura = curl_easy_init();
  gboolean opIncheiataCuSucces = FALSE;
  char errBuff[CURL_ERROR_SIZE];
  
  if(NULL != legatura) {
    curl_easy_setopt(legatura, CURLOPT_URL, adresa);
    curl_easy_setopt(legatura, CURLOPT_ERRORBUFFER, errBuff);
    curl_easy_setopt(legatura, CURLOPT_TIMEOUT, DL_CURL_TIMEOUT);
    
    curl_easy_setopt(legatura, CURLOPT_SSL_VERIFYPEER, TRUE); 
    curl_easy_setopt(legatura, CURLOPT_CAINFO, "cacert.pem");
    
    curl_easy_setopt(legatura, CURLOPT_WRITEFUNCTION, citeste_date_actualizare_recurent);
    curl_easy_setopt(legatura, CURLOPT_WRITEDATA, fisTinta);
    
    curl_easy_setopt(legatura, CURLOPT_PROGRESSFUNCTION, instiintare_progres_retea);
    curl_easy_setopt(legatura, CURLOPT_PROGRESSDATA, NULL);
    curl_easy_setopt(legatura, CURLOPT_NOPROGRESS, 0);
    
    if(curl_easy_perform(legatura) == CURLE_OK) {
      opIncheiataCuSucces = TRUE;
    } else {
	  g_warning("S-a ivit o eroare la descărcarea fișierului : %s!", errBuff);
	}
    curl_easy_cleanup(legatura);
  } else {
    g_warning("Nu am putut inițializa librăria de descărcări!");
  }

  return opIncheiataCuSucces;
}
