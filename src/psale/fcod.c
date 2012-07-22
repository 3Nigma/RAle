#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcestyleschememanager.h>

#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include "al.h"
#include "db.h"
#include "fcod.h"

#define PSALE_CODS_IMPLICIT ".section text\n" \
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

static GtkSourceLanguageManager *txtSrcLimbAsignor = NULL;
static GtkSourceStyleSchemeManager *txtSrcStilAsignor = NULL;

static gchar *
obtine_codul_sursa_curent(GtkTextView *txtView) {
  GtkTextIter csStart;
  GtkTextIter csFinal;
  GtkTextBuffer *txtBuf = gtk_text_view_get_buffer(txtView);

  gtk_text_buffer_get_start_iter(txtBuf, &csStart);
  gtk_text_buffer_get_end_iter(txtBuf, &csFinal);

  return gtk_text_buffer_get_text(txtBuf, &csStart, &csFinal, FALSE);
}

static gboolean 
frmCod_delev(GtkWidget *widget, GdkEvent *event, gpointer data) {
  return FALSE;
}

static void 
btIncarcaPeAle_click(GtkWidget *bt, FormularCod *fc) {
  /* Acțiunile ce trebuie realizate pentru trimiterea de cod spre plăcuță sunt următoarele :
     avr-gcc -Os -Wall -mmcu=attiny25 main.s -o main.o
     avr-objcopy -j .text -O ihex main.o main.hex
     sudo avrdude -c usbtiny -p t25 -U flash:w:main.hex */
  char denFisSursa[L_tmpnam + 5];
  FILE *fs = NULL;
  gchar *codPrezent = NULL;

  sprintf(denFisSursa, "%s%s", tmpnam(NULL), fc->lmFolosit == C ? ".c" : ".s");
  fs = fopen(denFisSursa, "w");
  codPrezent = obtine_codul_sursa_curent(GTK_TEXT_VIEW(fc->txtVizCod));
  fwrite(codPrezent, sizeof(gchar), strlen(codPrezent), fs);
  fclose(fs);

  gchar textComandaGcc[256];
  gchar textComandaObjcopy[256];
  gchar textComandaAvrdude[256];
  char denObiectRezultat[L_tmpnam];
  char denHexRezultat[L_tmpnam];

  tmpnam(denObiectRezultat);
  tmpnam(denHexRezultat);

#ifdef G_OS_WIN32
  g_sprintf(textComandaGcc, "-Os -Wall -mmcu=attiny25 %s -o %s", denFisSursa, denObiectRezultat);
  g_sprintf(textComandaObjcopy, "-j .text -O ihex %s %s", denObiectRezultat, denHexRezultat);
  g_sprintf(textComandaAvrdude, "-c usbtiny -p t25 -U flash:w:%s", denHexRezultat);
 
  ShellExecute(NULL, "open", "winavr/bin/avr-gcc.exe", textComandaGcc, NULL, SW_SHOWNORMAL);
  ShellExecute(NULL, "open", "winavr/bin/avr-objcopy.exe", textComandaObjcopy, NULL, SW_SHOWNORMAL);
  if(actualizeaza_stare_placuta(fc->lblStareConex))
    ShellExecute(NULL, "open", "avrdude.exe", textComandaAvrdude, NULL, SW_SHOWNORMAL);
  else
    placutaConectata = FALSE;
#elif defined G_OS_UNIX
  g_sprintf(textComandaGcc, "avr-gcc -Os -Wall -mmcu=attiny25 %s -o %s", denFisSursa, denObiectRezultat);
  g_sprintf(textComandaObjcopy, "avr-objcopy -j .text -O ihex %s %s", denObiectRezultat, denHexRezultat);
  g_sprintf(textComandaAvrdude, "sudo avrdude -c usbtiny -p t25 -U flash:w:%s 2> /dev/null", denHexRezultat);
  
  system(textComandaGcc);
  system(textComandaObjcopy);
  system(textComandaAvrdude);
  if(al_este_placuta_conectata())
    system(textComandaAvrdude);
  else
    if(fc->laDepistare_neprezentaPlacuta_recurenta != NULL) fc->laDepistare_neprezentaPlacuta_recurenta();
#endif

  remove(denFisSursa);
  remove(denObiectRezultat);
  remove(denHexRezultat);
}

static void 
btExpandatorActiuni_click(GtkWidget *bt, FormularCod *fc) {
  GdkPixbuf *imgExpandatorPixBuf = NULL;
  GtkWidget *imgExpandatorActiuni = NULL;

  switch(fc->vActiuni) {
  case ASCUNSE:
    gtk_widget_show(fc->cadruActiuni);
    imgExpandatorPixBuf = db_obtine_imagine_media_scalata(DB_IMG_COLAPSEAZA, 20, 20);
    fc->vActiuni = VIZIBILE;
    break;
  case VIZIBILE:
    gtk_widget_hide(fc->cadruActiuni);
    imgExpandatorPixBuf = db_obtine_imagine_media_scalata(DB_IMG_EXPANDEAZA, 20, 20);
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

  switch(fc->vActiuni) {
  case ASCUNSE:
    gtk_widget_hide(fc->cadruActiuni);
    break;
  case VIZIBILE:
    gtk_widget_show(fc->cadruActiuni);
    break;
  }
  }
}

FormularCod *
fc_initializeaza(Limbaj lmDorit, const char *codInitial, gchar *denumireSursa, gboolean esteExemplu) {
  FormularCod *deRet = NULL;
  GtkWidget *frm = NULL;
  GtkWidget *cadruFrm = NULL;
  GtkWidget *txtSrc = NULL;
  GtkWidget *btGestioneazaActiuni = NULL;
  GtkWidget *btIncarcaPeAle = NULL;
  GtkWidget *btSalveazaLucrul = NULL;
  GtkWidget *btReiaLucrul = NULL;
  GtkWidget *btCitesteEEPROM = NULL;
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
  cadruFrm = gtk_table_new(2, 3, FALSE);
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
  if(esteExemplu)
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), codInitial, -1);
  else switch(lmDorit) {
    case C:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODC_IMPLICIT, -1);
      break;
    case ASM:
      gtk_text_buffer_set_text(GTK_TEXT_BUFFER(txtSrcBuf), PSALE_CODS_IMPLICIT, -1);
      break;
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
  gtk_table_attach_defaults(GTK_TABLE(cadruFrm), txtSrcScroll, 0, 1, 0, 1);

  /* inițializăm butonul de afișare/ascundere acțiuni */
  btGestioneazaActiuni = gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(btGestioneazaActiuni), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btGestioneazaActiuni), FALSE);
  GdkPixbuf *imgExpandatorPixBuf = db_obtine_imagine_media_scalata(esteExemplu ? DB_IMG_COLAPSEAZA : DB_IMG_EXPANDEAZA, 20, 20);
  GtkWidget *imgExpandatorActiuni = gtk_image_new_from_pixbuf(imgExpandatorPixBuf);
  gtk_button_set_image(GTK_BUTTON(btGestioneazaActiuni), imgExpandatorActiuni);
  gtk_table_attach(GTK_TABLE(cadruFrm), btGestioneazaActiuni, 1, 2, 0, 2, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializează acțiunile speciale ale formularului de cod */  
  btIncarcaPeAle = gtk_button_new_with_label("Încarcă pe Ale");
  gtk_button_set_focus_on_click(GTK_BUTTON(btIncarcaPeAle), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(btIncarcaPeAle), -1, 60);
  GdkPixbuf *imgIncarcaPeAlePixBuf = db_obtine_imagine_media_scalata(DB_IMG_TRIMITE_LA_ALE, 16, 16);
  GtkWidget *imgIncarcaPeAle = gtk_image_new_from_pixbuf(imgIncarcaPeAlePixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btIncarcaPeAle), GTK_POS_RIGHT);
  gtk_button_set_image(GTK_BUTTON(btIncarcaPeAle), imgIncarcaPeAle);

  btSalveazaLucrul = gtk_button_new_with_label("Salvează lucrul");
  gtk_button_set_relief(GTK_BUTTON(btSalveazaLucrul), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btSalveazaLucrul), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(btSalveazaLucrul), -1, 50);
  GdkPixbuf *imgSalveazaLucrulPixBuf = db_obtine_imagine_media_scalata(DB_IMG_SALVEAZA_COD, 16, 16);
  GtkWidget *imgSalveazaLucrul = gtk_image_new_from_pixbuf(imgSalveazaLucrulPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btSalveazaLucrul), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btSalveazaLucrul), imgSalveazaLucrul);

  btReiaLucrul = gtk_button_new_with_label("Reia cod");
  gtk_button_set_relief(GTK_BUTTON(btReiaLucrul), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btReiaLucrul), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(btReiaLucrul), -1, 40);
  GdkPixbuf *imgReiaLucrulPixBuf = db_obtine_imagine_media_scalata(DB_IMG_REIA_COD, 16, 16);
  GtkWidget *imgReiaLucrul = gtk_image_new_from_pixbuf(imgReiaLucrulPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btReiaLucrul), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btReiaLucrul), imgReiaLucrul);

  btCitesteEEPROM = gtk_button_new_with_label("Citește EEPROM");
  gtk_button_set_relief(GTK_BUTTON(btCitesteEEPROM), GTK_RELIEF_NONE);
  gtk_button_set_focus_on_click(GTK_BUTTON(btCitesteEEPROM), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(btCitesteEEPROM), -1, 35);
  GdkPixbuf *imgCitesteEEPROMPixBuf = db_obtine_imagine_media_scalata(DB_IMG_EEPROM, 16, 16);
  GtkWidget *imgCitesteEEPROM = gtk_image_new_from_pixbuf(imgCitesteEEPROMPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btCitesteEEPROM), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btCitesteEEPROM), imgCitesteEEPROM);

  btParasesteFrm = gtk_button_new_with_label("Părăsește formular");
  gtk_button_set_relief(GTK_BUTTON(btParasesteFrm), GTK_RELIEF_HALF);
  gtk_button_set_focus_on_click(GTK_BUTTON(btParasesteFrm), FALSE);
  gtk_widget_set_size_request(GTK_WIDGET(btParasesteFrm), -1, 30);
  GdkPixbuf *imgParasesteFrmPixBuf = db_obtine_imagine_media_scalata(DB_IMG_PARASESTE, 16, 16);
  GtkWidget *imgParasesteFrm = gtk_image_new_from_pixbuf(imgParasesteFrmPixBuf);
  gtk_button_set_image_position(GTK_BUTTON(btParasesteFrm), GTK_POS_LEFT);
  gtk_button_set_image(GTK_BUTTON(btParasesteFrm), imgParasesteFrm);

  /* inițializăm cadrul regiunii de acțiuni */
  cadruBxActiuni = gtk_vbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(cadruBxActiuni), GTK_BUTTONBOX_DEFAULT_STYLE);
  gtk_box_set_homogeneous(GTK_BOX(cadruBxActiuni), FALSE);

  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btIncarcaPeAle, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btSalveazaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btReiaLucrul, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btCitesteEEPROM, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBxActiuni), btParasesteFrm, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBxActiuni, 2, 3, 0, 2, GTK_SHRINK, GTK_FILL, 0, 0);

  /* inițializăm bara de stare a formularului */
  lblStareLegatura = gtk_label_new("[Conexiune]");
  lblStareCod = gtk_label_new(NULL);
  lblStareNumeSursa = gtk_label_new(denumireSursa);

  cadruBaraStare = gtk_hbox_new(TRUE, 5);
  gtk_container_set_border_width(GTK_CONTAINER(cadruBaraStare), 3);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareLegatura, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareCod, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(cadruBaraStare), lblStareNumeSursa, FALSE, FALSE, 0);
  gtk_table_attach(GTK_TABLE(cadruFrm), cadruBaraStare, 0, 1, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  /* împachetăm elementele esențiale pentru a le returna */
  deRet = g_slice_new(FormularCod);
  deRet->frm = frm;
  deRet->lmFolosit = lmDorit;
  deRet->txtVizCod = txtSrc;
  deRet->btExpandator = btGestioneazaActiuni;
  deRet->cadruActiuni = cadruBxActiuni;
  deRet->lblStareConex = lblStareLegatura;
  deRet->lblStareDCod = lblStareCod;
  deRet->lblStareNSursa = lblStareNumeSursa;
  if(esteExemplu) deRet->vActiuni = VIZIBILE;
  else deRet->vActiuni = ASCUNSE;
  deRet->laDepistare_neprezentaPlacuta_recurenta = NULL;
  
  /* legăm semnalele de funcțiile recurente */
  g_signal_connect(frm, "delete-event", G_CALLBACK(frmCod_delev), NULL);
  g_signal_connect(btGestioneazaActiuni, "clicked", G_CALLBACK(btExpandatorActiuni_click), (gpointer)deRet);
  g_signal_connect(btIncarcaPeAle, "clicked", G_CALLBACK(btIncarcaPeAle_click), (gpointer)deRet);
  g_signal_connect_swapped(btParasesteFrm, "clicked", G_CALLBACK(gtk_widget_destroy), frm);
  
  return deRet;
}

FormularCod *
fc_initializeaza_fara_cod(Limbaj lmDorit) {
  return fc_initializeaza(lmDorit, "", "[Cod Nou]", FALSE);
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
