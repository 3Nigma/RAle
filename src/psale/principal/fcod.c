/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcestyleschememanager.h>

#ifdef G_OS_WIN32
  #include <windows.h>
#endif

#include "os.h"
#include "fl.h"
#include "al.h"
#include "db.h"
#include "feeprom.h"
#include "fcod.h"

#define PSALE_SURSELE_MELE_DIR                  "sursele mele"
#define PSALE_PREFIX_LBLSTARENUME_SURSA_PROPRIE "SM"
#define PSALE_PREFIX_LBLSTARENUME_SURSA_EXEMPLU "Ex"
#define PSALE_PREFIX_MODIFICARE_COD_TEXT        "*"
#define PSALE_PREFIX_MODIFICARE_COD_FORMATAT    "<b>"PSALE_PREFIX_MODIFICARE_COD_TEXT"</b>"

#define PSALE_CODS_IMPLICIT ".section .text\n" \
  ".include \"ale.inc\"\n" \
  ".global main\n\n" \
  "main:\n" \
  "  ; introduceți codul dumneavoastră aici\n" \
  "ciclu_infinit:\n" \
  "  rjmp ciclu_infinit\n"
#define PSALE_CODC_IMPLICIT "#include \"ale.h\"\n\n" \
  "int main() {\n" \
  "  /* Scrieți codul dumneavoastră aici */\n" \
  "  return 0;\n" \
  "}\n"

static FormularCod *fc_initializeaza(Limbaj lmDorit, const char *codInitial, gchar *denumireSursa, gboolean esteExemplu);
static void laModificareCod(GtkTextBuffer *textbuffer, FormularCod *fc);
static gboolean laDezapasareTaste(GtkWidget *widget, GdkEventKey *ke, FormularCod *fc);

static void btExpandatorActiuni_click(GtkWidget *bt, FormularCod *fc);
static void btIncarcaPeAle_click(GtkWidget *bt, FormularCod *fc);
static void btReiaLucrul_click(GtkWidget *bt, FormularCod *fc);
static void btSalveazaLucrul_click(GtkWidget *bt, FormularCod *fc);
static void btEEPROM_click(GtkWidget *bt, FormularCod *fc);
static void btParasesteFrm_click(GtkWidget *bt, FormularCod *fc);

static void actualizeaza_stare_nume_sursa(GtkWidget *lbl, const gchar *numeSursa, gboolean esteExemplu, gboolean esteModificat);
static void actualizeaza_stare_nume_indicatii(GtkWidget *lbl, gboolean esteExemplu, gboolean esteModificata);
static gboolean este_sursa_modificata(FormularCod *fc);

static GtkSourceLanguageManager *txtSrcLimbAsignor = NULL;
static GtkSourceStyleSchemeManager *txtSrcStilAsignor = NULL;

static GtkWidget *
initializeaza_dialog_reluare_salvare(FormularCod *fc, TipDialogFisier df) {
  GtkWidget *dlgDeschideSursa = NULL;
  gchar titluDialog[64];
  GtkFileFilter *filtruFisSursa = NULL;
  
  filtruFisSursa = gtk_file_filter_new();
  switch(fc->lmFolosit) {
  case C:
    g_sprintf(titluDialog, "[C] Alegeți %s dorită ...", df == DESCHIDE ? "sursa" : "destinația");
    gtk_file_filter_set_name(filtruFisSursa, "Fișier C(*.c)");
    gtk_file_filter_add_pattern(filtruFisSursa, "*.c");
    break;
  case ASM:
    g_sprintf(titluDialog, "[ASM] Alegeți %s dorită ...", df == DESCHIDE ? "sursa" : "destinația");
    gtk_file_filter_set_name(filtruFisSursa, "Fișier ASM(*.s)");
    gtk_file_filter_add_pattern(filtruFisSursa, "*.s");
    break;
  }
  switch(df) {
  case DESCHIDE:
    dlgDeschideSursa = gtk_file_chooser_dialog_new (NULL,
				     GTK_WINDOW(fc->frm),
				     GTK_FILE_CHOOSER_ACTION_OPEN,
				     "Renunță", GTK_RESPONSE_CANCEL,
				     "Deschide", GTK_RESPONSE_ACCEPT,
				     NULL);	
    break;
  case SALVEAZA:
    dlgDeschideSursa = gtk_file_chooser_dialog_new (NULL,
				     GTK_WINDOW(fc->frm),
				     GTK_FILE_CHOOSER_ACTION_SAVE,
				     "Renunță", GTK_RESPONSE_CANCEL,
				     "Salvează", GTK_RESPONSE_ACCEPT,
				     NULL);	
    break;
  }
  gtk_window_set_title(GTK_WINDOW(dlgDeschideSursa), titluDialog);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dlgDeschideSursa), FALSE);
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dlgDeschideSursa), filtruFisSursa);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlgDeschideSursa), PSALE_SURSELE_MELE_DIR);
  
  return dlgDeschideSursa;
}

static gchar *
obtine_codul_sursa_curent(GtkTextView *txtView) {
  GtkTextIter csStart;
  GtkTextIter csFinal;
  GtkTextBuffer *txtBuf = gtk_text_view_get_buffer(txtView);

  gtk_text_buffer_get_start_iter(txtBuf, &csStart);
  gtk_text_buffer_get_end_iter(txtBuf, &csFinal);

  return gtk_text_buffer_get_text(txtBuf, &csStart, &csFinal, FALSE);
}

static void 
seteaza_codul_sursa_curent(GtkTextView *txtView, const gchar *txt) {
  GtkTextBuffer *codBuff = NULL;
  
  codBuff = gtk_text_view_get_buffer(txtView);
  gtk_text_buffer_set_text(codBuff, txt, strlen(txt));
}

static gchar *
obtine_doar_nume_fisier(const gchar *numeFis) {
  GRegex *tiparNumeFis = NULL;
  GMatchInfo *rezultatTipar = NULL;
  gchar *nFisier = NULL;
  
  tiparNumeFis = g_regex_new("([\\w]+)[\\.]([\\w]+)", 0, 0, NULL);
  nFisier = g_new(gchar, 64);
    
  g_regex_match(tiparNumeFis, numeFis, 0, &rezultatTipar);
  if(g_match_info_matches(rezultatTipar)) {
    // [1] = nume mare, [2] = extensia
    g_sprintf(nFisier, "%s.%s", g_match_info_fetch(rezultatTipar, 1), g_match_info_fetch(rezultatTipar, 2));
  } else {
    /* numeFis nu a avut formatul necesar */
    g_sprintf(nFisier, "<b>Nume invalid!</b>");
  }
    
  g_regex_unref(tiparNumeFis);
  return nFisier;
}

static gchar *
incarca_continut_din_fisier(const gchar *numeFis) {
  gchar *continut = NULL;
  FILE *pFile = NULL;
  int fileSize = 0;

  /* deschide fișierul și calculează dimensiunea sa */
  pFile = fopen(numeFis, "r");
  fseek(pFile, 0, SEEK_END);
  fileSize = ftell(pFile);
  rewind(pFile);
    
  /* încarcă conținutul fișierului într-o variabilă alocată dinamic */
  continut = g_new0(gchar, fileSize + 20);
  fread(continut, sizeof(gchar), fileSize, pFile);
  fclose(pFile);
    
  return continut;
}

static void 
salveaza_continut_in_fisier(const gchar *continut, const gchar *numeFisier) {
  FILE *pFile = NULL;
  
  pFile = fopen(numeFisier, "w");
  fprintf(pFile, "%s", continut);
  fclose(pFile);
}

static void 
realizeaza_salvare_la_incheiere(FormularCod *fc) {
  if(este_sursa_modificata(fc)) {
    /* există modificări curente.
     * Întreabă utilizatorul ce să facă cu ele */
     GtkWidget *dlgIntrebareSalvare = NULL;
     
     dlgIntrebareSalvare = gtk_message_dialog_new(GTK_WINDOW(fc->frm), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                                  GTK_BUTTONS_NONE, "Codul curent a suferit modifcări.\nDoriți să le salvați ?");
     gtk_window_set_title(GTK_WINDOW(dlgIntrebareSalvare), "Întrebare");
     gtk_dialog_add_buttons(GTK_DIALOG(dlgIntrebareSalvare), 
                            "Nu", GTK_RESPONSE_NO,
                            "Da", GTK_RESPONSE_YES,
                            NULL);
     gtk_dialog_set_default_response(GTK_DIALOG(dlgIntrebareSalvare), GTK_RESPONSE_YES);
     if(gtk_dialog_run(GTK_DIALOG(dlgIntrebareSalvare)) == GTK_RESPONSE_YES) {
       btSalveazaLucrul_click(NULL, fc);
	 }
	 gtk_widget_destroy(dlgIntrebareSalvare);
  }
}

static gboolean 
frmCod_delev(GtkWidget *widget, GdkEvent *event, FormularCod *fc) {
  realizeaza_salvare_la_incheiere(fc);
  
  return FALSE;
}

static void 
btIncarcaPeAle_click(GtkWidget *bt, FormularCod *fc) {
  /* Acțiunile ce trebuie realizate pentru trimiterea de cod spre plăcuță sunt următoarele :
     avr-gcc -Os -Wall -mmcu=attiny25 main.s -o main.o
     avr-objcopy -j .text -O ihex main.o main.hex 
	 + scrierea prin avrdude */
  /* depozitează sursa curentă într-un fișier temporar pentru a putea fi procesat mai departe */
  char denFisSursa[1024];
  FILE *fs = NULL;
  gchar *codPrezent = NULL;
  gchar locatieTempFisSursa[1024];
  
  os_obtine_nume_fis_temporar(locatieTempFisSursa, sizeof(locatieTempFisSursa)/(sizeof(gchar)));
  sprintf(denFisSursa, "%s%s", locatieTempFisSursa, fc->lmFolosit == C ? ".c" : ".s");
  fs = fopen(denFisSursa, "w");
  codPrezent = obtine_codul_sursa_curent(GTK_TEXT_VIEW(fc->txtVizCod));
  fwrite(codPrezent, sizeof(gchar), strlen(codPrezent), fs);
  fclose(fs);

  /* alocăm variabile necesare pentru execuția celorlalte tool-uri */
  gchar textComandaGcc[1024];
  gchar textComandaObjcopy[1024];
  BaraInfoCod *bInfoFormularCurent;
  char denObiectRezultat[1024];
  char denHexRezultat[1024];
  gchar *cDir = NULL;
  
  /* generăm fișierele temporale necesare */
  os_obtine_nume_fis_temporar(denObiectRezultat, sizeof(denObiectRezultat)/(sizeof(gchar)));
  os_obtine_nume_fis_temporar(denHexRezultat, sizeof(denHexRezultat)/(sizeof(gchar)));
  bInfoFormularCurent = &fc->bInfo;
  cDir = g_get_current_dir();
  
#ifdef G_OS_WIN32
  g_sprintf(textComandaGcc, "%s\\winavr\\bin\\avr-gcc.exe -Os -Wall %s-I\"%s\\%s\" -mmcu=attiny25 \"%s\" -o \"%s\"", 
                            cDir, 
                            fc->lmFolosit == C ? "" : "-Wa,"
                            cDir, PSALE_SURSELE_MELE_DIR, 
                            denFisSursa, denObiectRezultat);
  g_sprintf(textComandaObjcopy, "%s\\winavr\\bin\\avr-objcopy.exe -j .text -O ihex \"%s\" \"%s\"", cDir, denObiectRezultat, denHexRezultat);
#elif defined G_OS_UNIX
  /* Construiește instrucțiunile ce vor fi executate pe cod, prin sistemul de operare.
   * Redirecționarea de tip stderr > stdout ('2>&1') se realizează pentru a ajuta funcția popen care nu știe 
   * să lucreze decât cu stdout!*/
  g_sprintf(textComandaGcc, "avr-gcc -Os -Wall %s-I\"%s/%s\" -mmcu=attiny25 \"%s\" -o \"%s\" 2>&1", 
                            fc->lmFolosit == C ? "" : "-Wa,",
                            cDir, PSALE_SURSELE_MELE_DIR, 
                            denFisSursa, denObiectRezultat);
  g_sprintf(textComandaObjcopy, "avr-objcopy -j .text -O ihex \"%s\" \"%s\"", denObiectRezultat, denHexRezultat);
#endif

  g_free(cDir);

  /* încearcă să compilezi sursa curentă trecând-o prin 'avr-gcc' și interpretând eventualele erori în bara de stare a formularului curent */
  if(os_executa_si_completeaza_bic_fc(textComandaGcc, bInfoFormularCurent)) {
    /* Compilarea a reușit. Încearcă o extragere a secțiunilor necesare pentru programator și construiește HEX-ul */
    os_system(textComandaObjcopy);
    bic_ascunde(bInfoFormularCurent);
  
    /* verifică conexiunea cu plăcuța și trimite codul rezultat doar dacă aceasta este vizibilă */
    if(al_este_placuta_conectata()) {
      if(!al_scrie_aplicatie(denHexRezultat)) {
	    g_debug("Nu am putut trimite aplicația spre plăcuță!");
	  }
    } else {
      /* asigură-te că, întradevăr, formularele de cod deschise știu că plăcuța nu este conectată */
      if(fc->laDepistare_neprezentaPlacuta_recurenta != NULL) fc->laDepistare_neprezentaPlacuta_recurenta();
    }
  
    /* realizează o curățare generală a fișierelor implicate în proces */
    remove(denFisSursa);
    remove(denObiectRezultat);
    remove(denHexRezultat);
  } else {
    /* există erori în codul utilizatorului. Afișează mesajele culese prin bara de stare (dacă acestea există) */
	if(bic_text_contine_informatii_utile(bInfoFormularCurent)) {
	  bic_arata(bInfoFormularCurent);
	}
  }
}

static void 
actualizeaza_stare_nume_sursa(GtkWidget *lbl, const gchar *numeSursa, gboolean esteExemplu, gboolean esteModificat) {
  gchar formatNumeEticheta[256];
  
  /* actualizează stare nume */
  g_sprintf(formatNumeEticheta, "%s%s%s", 
            esteModificat ? PSALE_PREFIX_MODIFICARE_COD_FORMATAT : "",
            esteExemplu ? "<i>"PSALE_PREFIX_LBLSTARENUME_SURSA_EXEMPLU"</i> : " : "<i>"PSALE_PREFIX_LBLSTARENUME_SURSA_PROPRIE"</i> : ",
            numeSursa);
            
  gtk_label_set_markup(GTK_LABEL(lbl), formatNumeEticheta);
  
  /* actualizează indicațiile sale */
  actualizeaza_stare_nume_indicatii(lbl, esteExemplu, esteModificat);
}

static gboolean 
este_sursa_modificata(FormularCod *fc) {
  const gchar *strStareCurentaText = gtk_label_get_text(GTK_LABEL(fc->lblStareNSursa));
  gboolean rezultatFct = FALSE;
  
  if(g_str_has_prefix(strStareCurentaText, PSALE_PREFIX_MODIFICARE_COD_TEXT) == TRUE) {
    rezultatFct = TRUE;
  }
  
  return rezultatFct;
}

static void 
actualizeaza_stare_nume_indicatii(GtkWidget *lbl, gboolean esteExemplu, gboolean esteModificata) {
  gchar textIndicatie[512];
  
  g_sprintf(textIndicatie, "Arată numele sursei curente.\n%s%s", 
                           (esteExemplu ? 
                             "Prefixul '"PSALE_PREFIX_LBLSTARENUME_SURSA_EXEMPLU"' indică faptul că sursa curentă face parte din exemple." :
                             "Prefixul '"PSALE_PREFIX_LBLSTARENUME_SURSA_PROPRIE"' sugerează faptul că sursa este personală."),
                           (esteModificata ? 
                             "\n'"PSALE_PREFIX_MODIFICARE_COD_FORMATAT"' spune faptul că sursa a suferit modificări între timp." :
                             ""
                             )
                           
                           );
  
  gtk_widget_set_tooltip_markup(lbl, textIndicatie);
}

static void 
laModificareCod(GtkTextBuffer *textbuffer, FormularCod *fc) {
  if(este_sursa_modificata(fc) == FALSE) {
    actualizeaza_stare_nume_sursa(fc->lblStareNSursa, fc->numeSimpluAfisat, fc->esteExempluIncarcat, TRUE);
  }
}

static gboolean 
laDezapasareTaste(GtkWidget *widget, GdkEventKey *ke, FormularCod *fc) {
  gboolean keyHandled = FALSE;
  
  if((ke->state & GDK_CONTROL_MASK) != 0) {
    /* tasta 'Ctrl' a fost apăsată */
    switch(ke->keyval) {
    case GDK_KEY_S:
    case GDK_KEY_s:
     /* ... și tasta 's' a fost apăsată.
      * Realizează o salvarea rapidă o salvare rapidă */
      if(strlen(fc->caleCurentaSursa) != 0) {
	    /* sursa curentă nu este un exemplu și are o cale de slavare deja asociată. Continuă cu salvarea */
	    salveaza_continut_in_fisier(obtine_codul_sursa_curent(GTK_TEXT_VIEW(fc->txtVizCod)) , fc->caleCurentaSursa);
	    actualizeaza_stare_nume_sursa(fc->lblStareNSursa, fc->numeSimpluAfisat, fc->esteExempluIncarcat, FALSE);
      }
      keyHandled = TRUE;
      break;
    case GDK_KEY_T:
    case GDK_KEY_t:
     /* ... și tasta 't' a fost apăsată
	  * Simulează o trimitere spre plăcuță. */
	  btIncarcaPeAle_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    case GDK_KEY_D:
    case GDK_KEY_d:
     /* ... și tasta 'd' a fost apăsată
	  * Simulează o depozitare în memoria internă. */
	  btSalveazaLucrul_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    case GDK_KEY_R:
    case GDK_KEY_r:
     /* ... și tasta 'r' a fost apăsată
	  * Simulează o reluare de cod propriu. */
	  btReiaLucrul_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    case GDK_KEY_E:
    case GDK_KEY_e:
     /* ... și tasta 'e' a fost apăsată
	  * Declanșează formularul memoriei speciale. */
	  btEEPROM_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    case GDK_KEY_P:
    case GDK_KEY_p:
     /* ... și tasta 'p' a fost apăsată
	  * Dorim părăsirea formularului. O realizăm! */
	  btParasesteFrm_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    case GDK_KEY_B:
    case GDK_KEY_b:
     /* ... și tasta 'b' a fost apăsată
	  * Declanșează o tranziție a regiunii butoanelor */
	  btExpandatorActiuni_click(NULL, fc);
	  keyHandled = TRUE;
      break;
    }
  }
  
  return keyHandled;
}
                                                      
static void 
btSalveazaLucrul_click(GtkWidget *bt, FormularCod *fc) {
  GtkWidget *dlgSalveazaSursa = NULL;
  
  dlgSalveazaSursa = initializeaza_dialog_reluare_salvare(fc, SALVEAZA);
  if(gtk_dialog_run(GTK_DIALOG(dlgSalveazaSursa)) == GTK_RESPONSE_ACCEPT) {
	gchar *caleSursa = NULL;
	gchar *numeSimpluSursa = NULL;
	
	caleSursa = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlgSalveazaSursa));
    salveaza_continut_in_fisier(obtine_codul_sursa_curent(GTK_TEXT_VIEW(fc->txtVizCod)) , caleSursa);
    numeSimpluSursa = obtine_doar_nume_fisier(caleSursa);
    
    fc->esteExempluIncarcat = FALSE;
    g_sprintf(fc->numeSimpluAfisat, "%s", numeSimpluSursa);
    actualizeaza_stare_nume_sursa(fc->lblStareNSursa, fc->numeSimpluAfisat, fc->esteExempluIncarcat, FALSE);
    g_sprintf(fc->caleCurentaSursa, "%s", caleSursa);
    
    g_free(caleSursa);
    g_free(numeSimpluSursa);
  }
  gtk_widget_destroy(dlgSalveazaSursa);
}

static void 
btEEPROM_click(GtkWidget *bt, FormularCod *fc) {
  FormularEEPROM *fe = NULL;
  
  fe = fme_initializeaza(GTK_WINDOW(fc->frm));
  fme_afiseaza(fe);
}

static void 
btReiaLucrul_click(GtkWidget *bt, FormularCod *fc) {
  GtkWidget *dlgDeschideSursa = NULL;
  
  dlgDeschideSursa = initializeaza_dialog_reluare_salvare(fc, DESCHIDE);
  if(gtk_dialog_run(GTK_DIALOG(dlgDeschideSursa)) == GTK_RESPONSE_ACCEPT) {
    gchar *caleSursa = NULL;
    gchar *continutSursa = NULL;
    gchar *numeSimpluSursa = NULL;
    
    caleSursa = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlgDeschideSursa));
    continutSursa = incarca_continut_din_fisier(caleSursa);
    seteaza_codul_sursa_curent(GTK_TEXT_VIEW(fc->txtVizCod), continutSursa);
    numeSimpluSursa = obtine_doar_nume_fisier(caleSursa);
    
    fc->esteExempluIncarcat = FALSE;
    g_sprintf(fc->numeSimpluAfisat, "%s", numeSimpluSursa);
    actualizeaza_stare_nume_sursa(fc->lblStareNSursa, fc->numeSimpluAfisat, fc->esteExempluIncarcat, FALSE);
    g_sprintf(fc->caleCurentaSursa, "%s", caleSursa);
    
    g_free(caleSursa);
    g_free(continutSursa);
    g_free(numeSimpluSursa);
  }
  gtk_widget_destroy(dlgDeschideSursa);
}

static void 
btParasesteFrm_click(GtkWidget *bt, FormularCod *fc) {
  realizeaza_salvare_la_incheiere(fc);
  
  gtk_widget_destroy(fc->frm);
}

static void 
btExpandatorActiuni_click(GtkWidget *bt, FormularCod *fc) {
  GdkPixbuf *imgExpandatorPixBuf = NULL;
  GtkWidget *imgExpandatorActiuni = NULL;

  switch(fc->vActiuni) {
  case ASCUNSE:
    gtk_widget_set_tooltip_markup(fc->btExpandator, "Ascunde activitățile formularului.");
    gtk_widget_show(fc->cadruActiuni);
    imgExpandatorPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_COLAPSEAZA, 20, 20);
    fc->vActiuni = VIZIBILE;
    break;
  case VIZIBILE:
    gtk_widget_set_tooltip_markup(fc->btExpandator, "Arată activitățile disponibile.");
    gtk_widget_hide(fc->cadruActiuni);
    imgExpandatorPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_EXPANDEAZA, 20, 20);
    fc->vActiuni = ASCUNSE;
    break;
  }

  imgExpandatorActiuni = gtk_image_new_from_pixbuf(imgExpandatorPixBuf);
  gtk_button_set_image(GTK_BUTTON(fc->btExpandator), imgExpandatorActiuni);
}

void 
fc_modifica_vizibilitate(FormularCod *fc, gboolean vizibil) {
  if(NULL != fc) {
  if(vizibil)  gtk_widget_show_all(fc->frm);
  else gtk_widget_hide_all(fc->frm);

  /* gestionează vizibilitatea activitătilor */
  if(fc->vActiuni == ASCUNSE) {
    fc->vActiuni = VIZIBILE;
  } else {
	fc->vActiuni = ASCUNSE;
  }
  btExpandatorActiuni_click(NULL, fc);
  
  bic_ascunde(&fc->bInfo);
  }
}

static FormularCod *
fc_initializeaza(Limbaj lmDorit, const char *codInitial, gchar *denumireSursa, gboolean esteExemplu) {
  FormularCod *deRet = NULL;
  GtkWidget *frm = NULL;
  GtkWidget *cadruFrm = NULL;
  GtkWidget *txtSrc = NULL;
  GtkWidget *btGestioneazaActiuni = NULL;
  GtkWidget *btIncarcaPeAle = NULL;
  GtkWidget *btSalveazaLucrul = NULL;
  GtkWidget *btReiaLucrul = NULL;
  GtkWidget *btEEPROM = NULL;
  GtkWidget *btParasesteFrm = NULL;
  GtkWidget *cadruBxActiuni = NULL;
  GtkWidget *cadruBaraStare = NULL;
  GtkWidget *lblStareLegatura = NULL;
  GtkWidget *lblStareCod = NULL;
  GtkWidget *lblStareNumeSursa = NULL;

  /* inițializăm formularul principal */
  frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  switch(lmDorit) {
  case C:
    gtk_window_set_title(GTK_WINDOW(frm), "[C] psAle ~ Fereastră de cod");
    break;
  case ASM:
    gtk_window_set_title(GTK_WINDOW(frm), "[ASM] psAle ~ Fereastră de cod");
    break;
  }
  gtk_window_set_position(GTK_WINDOW(frm), GTK_WIN_POS_CENTER);

  /* inițializăm cadrul formularului de cod */
  cadruFrm = gtk_table_new(3, 3, FALSE);
  gtk_container_add(GTK_CONTAINER(frm), cadruFrm);

  /* inițializăm entități ajutătoare pentru elementul de cod */
  if(NULL == txtSrcLimbAsignor) {
    gchar *dirs[] = {"./limbaje", NULL};
    txtSrcLimbAsignor = gtk_source_language_manager_get_default();
    gtk_source_language_manager_set_search_path(txtSrcLimbAsignor, dirs);
  }
  GtkSourceLanguage *txtSrcLimb = NULL;
  switch(lmDorit) {
  case C:
    txtSrcLimb = gtk_source_language_manager_get_language(txtSrcLimbAsignor, "c");
    break;
  case ASM:
    txtSrcLimb = gtk_source_language_manager_get_language(txtSrcLimbAsignor, "asm");
   break;
  }
  if(NULL == txtSrcStilAsignor) {
	gchar *dirs[] = {"./stiluri", NULL};
	txtSrcStilAsignor = gtk_source_style_scheme_manager_get_default();
	gtk_source_style_scheme_manager_set_search_path(txtSrcStilAsignor, dirs);
  }
  GtkSourceBuffer *txtSrcBuf = gtk_source_buffer_new_with_language(txtSrcLimb);
  gtk_source_buffer_set_style_scheme(txtSrcBuf, gtk_source_style_scheme_manager_get_scheme(txtSrcStilAsignor, "kate"));
  if(esteExemplu) {
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), codInitial, -1);
  } else {
    switch(lmDorit) {
    case C:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODC_IMPLICIT, -1);
      break;
    case ASM:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODS_IMPLICIT, -1);
      break;
    }
  }

  /* inițializăm elementul central care va găzdui codul utilizatorului */
  txtSrc = gtk_source_view_new_with_buffer(txtSrcBuf);
  gtk_widget_set_size_request(txtSrc, 480, 320);
  gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(txtSrc), 2);
  gtk_source_view_set_insert_spaces_instead_of_tabs(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(txtSrc), TRUE);
  gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(txtSrc), FALSE);

  GtkWidget *txtSrcScroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(txtSrcScroll), txtSrc);
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), txtSrcScroll, 0, 1, 1, 2);

  /* inițializăm butonul de afișare/ascundere acțiuni */
  btGestioneazaActiuni = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btGestioneazaActiuni), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btGestioneazaActiuni), FALSE);
  GdkPixbuf *imgExpandatorPixBuf = fl_obtine_imagine_media_scalata(esteExemplu ? FL_IMG_FCOD_COLAPSEAZA : FL_IMG_FCOD_EXPANDEAZA, 20, 20);
  GtkWidget *imgExpandatorActiuni = gtk_image_new_from_pixbuf(imgExpandatorPixBuf);
  g_object_unref(imgExpandatorPixBuf);
  gtk_button_set_image(GTK_BUTTON(btGestioneazaActiuni), imgExpandatorActiuni);
  gtk_table_attach(GTK_TABLE(cadruFrm), btGestioneazaActiuni, 1, 2, 0, 3, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializează acțiunile speciale ale formularului de cod */  
  btIncarcaPeAle = gtk_button_new_with_label("Încarcă pe Ale");
  gtk_button_set_focus_on_click(GTK_BUTTON(btIncarcaPeAle), FALSE);
  gtk_widget_set_tooltip_markup(btIncarcaPeAle, "Construiește aplicația și împinge-o pe plăcuță.\nTaste scurte: <i>Ctrl + T</i>");
  gtk_widget_set_size_request(GTK_WIDGET(btIncarcaPeAle), -1, 60);
  GdkPixbuf *imgIncarcaPeAlePixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_TRIMITE_LA_ALE, 16, 16);
  GtkWidget *imgIncarcaPeAle = gtk_image_new_from_pixbuf(imgIncarcaPeAlePixBuf);
  g_object_unref(imgIncarcaPeAlePixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btIncarcaPeAle), GTK_POS_RIGHT);
  gtk_button_set_image(GTK_BUTTON(btIncarcaPeAle), imgIncarcaPeAle);

  btSalveazaLucrul = gtk_button_new_with_label("Salvează lucrul");
  gtk_button_set_relief(GTK_BUTTON(btSalveazaLucrul), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btSalveazaLucrul), FALSE);
  gtk_widget_set_tooltip_markup(btSalveazaLucrul, "Depozitează codul într-un fișier intern.\n"
                                                  "Taste scurte: <i>Ctrl + D</i>\n"
                                                  "Comandă rapidă: <i>Ctrl + S</i>");
  gtk_widget_set_size_request(GTK_WIDGET(btSalveazaLucrul), -1, 50);
  GdkPixbuf *imgSalveazaLucrulPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_SALVEAZA, 16, 16);
  GtkWidget *imgSalveazaLucrul = gtk_image_new_from_pixbuf(imgSalveazaLucrulPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btSalveazaLucrul), GTK_POS_LEFT);
  g_object_unref(imgSalveazaLucrulPixBuf);
  gtk_button_set_image(GTK_BUTTON(btSalveazaLucrul), imgSalveazaLucrul);

  btReiaLucrul = gtk_button_new_with_label("Reia cod");
  gtk_button_set_relief(GTK_BUTTON(btReiaLucrul), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btReiaLucrul), FALSE);
  gtk_widget_set_tooltip_markup(btReiaLucrul, "Încarcă sursă din fișier extern.\nTaste scurte: <i>Ctrl + R</i>");
  gtk_widget_set_size_request(GTK_WIDGET(btReiaLucrul), -1, 40);
  GdkPixbuf *imgReiaLucrulPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_REIA, 16, 16);
  GtkWidget *imgReiaLucrul = gtk_image_new_from_pixbuf(imgReiaLucrulPixBuf);
  g_object_unref(imgReiaLucrulPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btReiaLucrul), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btReiaLucrul), imgReiaLucrul);

  btEEPROM = gtk_button_new_with_label("Acțiuni EEPROM");
  gtk_button_set_relief(GTK_BUTTON(btEEPROM), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btEEPROM), FALSE);
  gtk_widget_set_tooltip_markup(btEEPROM, "Deschide formularul destinat memoriei speciale.\nTaste scurte: <i>Ctrl + E</i>");
  gtk_widget_set_size_request(GTK_WIDGET(btEEPROM), -1, 35);
  GdkPixbuf *imgEEPROMPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_EEPROM, 16, 16);
  GtkWidget *imgEEPROM = gtk_image_new_from_pixbuf(imgEEPROMPixBuf);
  g_object_unref(imgEEPROMPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btEEPROM), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btEEPROM), imgEEPROM);

  btParasesteFrm = gtk_button_new_with_label("Părăsește formular");
  gtk_button_set_relief(GTK_BUTTON(btParasesteFrm), GTK_RELIEF_HALF);
  gtk_button_set_focus_on_click(GTK_BUTTON(btParasesteFrm), FALSE);
  gtk_widget_set_tooltip_markup(btParasesteFrm, "Părăsește codul curent.\nTaste scurte: <i>Ctrl + P</i>");
  gtk_widget_set_size_request(GTK_WIDGET(btParasesteFrm), -1, 30);
  GdkPixbuf *imgParasesteFrmPixBuf = fl_obtine_imagine_media_scalata(FL_IMG_FCOD_PARASESTE, 16, 16);
  GtkWidget *imgParasesteFrm = gtk_image_new_from_pixbuf(imgParasesteFrmPixBuf);
  g_object_unref(imgParasesteFrmPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btParasesteFrm), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btParasesteFrm), imgParasesteFrm);

  /* inițializăm cadrul regiunii de acțiuni */
  cadruBxActiuni = gtk_vbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cadruBxActiuni), GTK_BUTTONBOX_DEFAULT_STYLE);
  gtk_box_set_homogeneous(GTK_BOX(cadruBxActiuni), FALSE);

  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btIncarcaPeAle, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btSalveazaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btReiaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btEEPROM, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btParasesteFrm, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBxActiuni, 2, 3, 0, 3, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializăm bara de stare inferioară a formularului */
  lblStareLegatura = gtk_label_new("[Conexiune]");
  gtk_widget_set_tooltip_markup(lblStareLegatura, "Prezintă starea conexiunii cu plăcuța.");
  lblStareCod = gtk_label_new(NULL);
  lblStareNumeSursa = gtk_label_new(NULL);
  actualizeaza_stare_nume_sursa(lblStareNumeSursa, denumireSursa, esteExemplu, FALSE);
  
  cadruBaraStare = gtk_hbox_new(TRUE, 5);
  gtk_container_set_border_width(GTK_CONTAINER(cadruBaraStare), 3);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareLegatura, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareCod, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareNumeSursa, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBaraStare, 0, 1, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  /* împachetăm elementele esențiale pentru a le returna */
  deRet = g_slice_new(FormularCod);
  deRet->frm = frm;
  deRet->lmFolosit = lmDorit;
  deRet->txtVizCod = txtSrc;
  deRet->esteExempluIncarcat = esteExemplu;
  g_sprintf(deRet->numeSimpluAfisat, "%s", denumireSursa);
  deRet->caleCurentaSursa[0] = '\0';
  deRet->btExpandator = btGestioneazaActiuni;
  deRet->cadruActiuni = cadruBxActiuni;
  deRet->lblStareConex = lblStareLegatura;
  deRet->lblStareDCod = lblStareCod;
  deRet->lblStareNSursa = lblStareNumeSursa;
  if(esteExemplu) deRet->vActiuni = VIZIBILE;
  else deRet->vActiuni = ASCUNSE;
  deRet->laDepistare_neprezentaPlacuta_recurenta = NULL;
  
  /* inițializăm bara de stare superioară */
  bic_initializeaza(&deRet->bInfo);
  deRet->bInfo.parentBuff = txtSrcBuf;
  deRet->bInfo.parentView = GTK_SOURCE_VIEW(txtSrc);
  gtk_table_attach(GTK_TABLE(cadruFrm), deRet->bInfo.wid, 0, 1, 0, 1, GTK_FILL, GTK_SHRINK, 3, 3);
  bic_ascunde(&deRet->bInfo);
  
  /* legăm semnalele de funcțiile recurente */
  g_signal_connect(frm, "delete-event", G_CALLBACK(frmCod_delev), (gpointer)deRet);
  g_signal_connect(btGestioneazaActiuni, "clicked", G_CALLBACK(btExpandatorActiuni_click), (gpointer)deRet);
  g_signal_connect(btIncarcaPeAle, "clicked", G_CALLBACK(btIncarcaPeAle_click), (gpointer)deRet);
  g_signal_connect(btSalveazaLucrul, "clicked", G_CALLBACK(btSalveazaLucrul_click), (gpointer)deRet);
  g_signal_connect(btReiaLucrul, "clicked", G_CALLBACK(btReiaLucrul_click), (gpointer)deRet);
  g_signal_connect(btEEPROM, "clicked", G_CALLBACK(btEEPROM_click), (gpointer)deRet);
  g_signal_connect(btParasesteFrm, "clicked", G_CALLBACK(btParasesteFrm_click), (gpointer)deRet);
  g_signal_connect(txtSrcBuf, "changed", G_CALLBACK(laModificareCod), (gpointer)deRet);
  g_signal_connect(frm, "key-release-event", G_CALLBACK(laDezapasareTaste), (gpointer)deRet);
  
  return deRet;
}

FormularCod *
fc_initializeaza_fara_cod(Limbaj lmDorit) {
  FormularCod *dlgCodRezultat = NULL;
  
  dlgCodRezultat = fc_initializeaza(lmDorit, "", "[Cod Nou]", FALSE);
  
  return dlgCodRezultat;
}

FormularCod *
fc_initializeaza_cu_exemplu(const gchar *titluScurt, const gchar *titluLung) {
  gchar numeExempluAfisat[256];
  char *codCompletExemplu = NULL;
  FormularCod *dlgCodRezultat = NULL;
  
  g_sprintf(numeExempluAfisat, "%s", titluLung);
  if(g_str_has_suffix(titluScurt, "s")) {
	codCompletExemplu = db_obtine_cod_complet(titluLung, "s");
    dlgCodRezultat = fc_initializeaza(ASM, codCompletExemplu, numeExempluAfisat, TRUE);
  } else {
	codCompletExemplu = db_obtine_cod_complet(titluLung, "c");
    dlgCodRezultat = fc_initializeaza(C, codCompletExemplu, numeExempluAfisat, TRUE);
  }
  
  if(NULL != codCompletExemplu) free(codCompletExemplu);
  return dlgCodRezultat;
}

void 
fc_actualizeaza_stare_placuta(GtkWidget *lblStare, gboolean online, gboolean primaRulare) {
  g_assert(GTK_IS_LABEL(lblStare));

  if(primaRulare) {
    if(online) {
      gtk_label_set_markup(GTK_LABEL(lblStare), "<span color=\"#003300\">Văd plăcuța!</span>");
    } else {
      gtk_label_set_markup(GTK_LABEL(lblStare), "<span color=\"#980000\"><b>NU</b> văd plăcuța!</span>");
    }
  } else {
    if(online) {
      gtk_label_set_markup(GTK_LABEL(lblStare), "<span color=\"#003300\">Am găsit <b>plăcuța</b>!</span>");
    } else {
      gtk_label_set_markup(GTK_LABEL(lblStare), "<span color=\"#980000\">Am pierdut plăcuța!</span>");
    }
  }
}
