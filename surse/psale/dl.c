#include "dl.h"

static FILE *pFisVersActuala;
static char adrActualizare[128];

void 
dl_curata() {
  fclose(pFisVersActuala);
}

int 
dl_initializeaza(const unsigned char *adresa) {
  strcpy(adrActualizare, adresa);
  pFisVersActuala = tmpfile();

  return pFisVersActuala != NULL;
}

static size_t 
citeste_lista_recurent(void *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t octetiScrisi = fwrite(ptr, size, nmemb, (FILE *)userdata);

  return octetiScrisi;
}

static int
dl_descarca_fisier_actualizari(const unsigned char *adresa) {
  CURL *legatura = curl_easy_init();
  int deRet = -1;

  if(NULL != legatura) {
    curl_easy_setopt(legatura, CURLOPT_URL, adresa);
    curl_easy_setopt(legatura, CURLOPT_WRITEFUNCTION, citeste_lista_recurent);
    curl_easy_setopt(legatura, CURLOPT_WRITEDATA, pFisVersActuala);
    if(curl_easy_perform(legatura) == CURLE_OK) {
      deRet = 1;
    }
    curl_easy_cleanup(legatura);
  }

  return deRet;
}

static int 
dl_exista_versiune_mai_buna(int versCurenta) {
  int versActuala = 0;
  
  rewind(pFisVersActuala);
  do {
    fscanf(pFisVersActuala, "%d,", &versActuala);
    if(versActuala > versCurenta)
      break;
    else
      while(!feof(pFisVersActuala) && fgetc(pFisVersActuala) != '\n');
  } while(!feof(pFisVersActuala));

  return (versActuala != versCurenta);
}

int 
dl_actualizeaza_bd() {
  assert(pFisVersActuala != NULL);
  int nrVersCurenta = db_obtine_versiune_curenta();

  dl_descarca_fisier_actualizari(adrActualizare);
  if(dl_exista_versiune_mai_buna(nrVersCurenta)) {
    /* există o versiune de bază de date mai bună decât ce avem noi. Începe actualizarea */
    printf("EXISTA VERSIUNE MAI NOUA!\n");
  }

  return DL_NICI_O_ACTUALIZARE;
}
