#include "fcod.h"

#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

GtkWidget *
initializeaza_formular_cod() {
  GtkWidget *frm = NULL;
  GtkWidget *txtSrc = NULL;
  GtkSourceBuffer *txtSrcBuf = NULL;
  GtkSourceLanguageManager *txtSrcLimbAsignor = NULL;
  GtkSourceLanguage *txtSrcLimb = NULL;

  frm = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gchar *dirs[] = {"./limbaje", g_get_current_dir(), NULL};
  txtSrcLimbAsignor = gtk_source_language_manager_get_default();
  gtk_source_language_manager_set_search_path(txtSrcLimbAsignor, dirs);
  txtSrcLimb = gtk_source_language_manager_get_language(txtSrcLimbAsignor, "c");
  txtSrcBuf = gtk_source_buffer_new_with_language(txtSrcLimb);
  txtSrc = gtk_source_view_new_with_buffer(txtSrcBuf);
  gtk_container_add(GTK_CONTAINER(frm), txtSrc);

  return frm;
}
