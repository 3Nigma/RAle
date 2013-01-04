/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <string.h>

#include "fprin.h"

static gboolean incearca_actualizare_actualizator();
static void inregistreazaMesaj(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data);

int main(int argc, char *argv[]) {
    FILE *fMesajAplicatie = NULL;

    /* redirecționăm toate mesajele de stare ale aplicației într-un fișier fizic */
    if ((fMesajAplicatie = fopen("mesaje_sesiune_" RPS_NUME_PSALE ".txt", "w")) != NULL) {
        g_log_set_handler("",
                G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION,
                inregistreazaMesaj, fMesajAplicatie);
    }

    FormularPrincipal *fprin = NULL;
    gboolean aplicatiaSaActualizatRecent = FALSE;

    if (incearca_actualizare_actualizator()) {
        g_debug("Am realizat cu succes înlocuirea actualizatorului cu o versiune mai nouă.");
    }

    if (argc == 2) {
        if (g_strcmp0(argv[1], "--proaspat-actualizata") == 0) {
            aplicatiaSaActualizatRecent = TRUE;
        }
    }

#ifdef G_OS_WIN32
    /* versiunea glib de windows cere această apelare pe când în versiunea curentă de linux, această funcție nu mai există */
    g_thread_init(NULL);
#endif
    gdk_threads_init();
    gtk_init(&argc, &argv);

    fprin = fp_initializeaza_formular_principal(aplicatiaSaActualizatRecent);
    fp_arata(fprin);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
    fp_curata(fprin);
    if (fMesajAplicatie != NULL) fclose(fMesajAplicatie);
    return 0;
}

static gboolean incearca_actualizare_actualizator() {
    gboolean rezultatOp = FALSE;

    if (g_file_test(ACTUALIZATOR_NUME_ACTUALIZAT, G_FILE_TEST_EXISTS)) {
        /* există o versiune mai nouă a acutalizatorului descărcată.
         * Înlocuiește actualizatorul curent cu cel nou */
        if (g_remove(ACTUALIZATOR_NUME_AP) == 0) {
            if (g_rename(ACTUALIZATOR_NUME_ACTUALIZAT, ACTUALIZATOR_NUME_AP) == 0) {
                rezultatOp = TRUE;
            }
        }
    }

    return rezultatOp;
}

static void inregistreazaMesaj(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {
    g_assert(user_data != NULL);

    gchar *mesajDeAdaugat = NULL;
    gchar tipMesaj[20];
    FILE *fSesiuneMesaje = (FILE *) (user_data);
    time_t timpIntroducere;

    if ((mesajDeAdaugat = g_new0(gchar, strlen(message) + 5)) != NULL) {
        timpIntroducere = time(NULL);
        switch (log_level) {
            case G_LOG_FLAG_FATAL:
                g_sprintf(tipMesaj, "FATAL");
                break;
            case G_LOG_LEVEL_ERROR:
                g_sprintf(tipMesaj, "EROARE");
                break;
            case G_LOG_LEVEL_CRITICAL:
                g_sprintf(tipMesaj, "CRITIC");
                break;
            case G_LOG_LEVEL_WARNING:
                g_sprintf(tipMesaj, "ATENTIE");
                break;
            case G_LOG_LEVEL_MESSAGE:
                g_sprintf(tipMesaj, "MESAJ");
                break;
            case G_LOG_LEVEL_INFO:
                g_sprintf(tipMesaj, "INFO");
                break;
            case G_LOG_LEVEL_DEBUG:
                g_sprintf(tipMesaj, "ANALIZA");
                break;
            default:
                g_sprintf(tipMesaj, "TIP %d", log_level);
                break;
        }

        fprintf(fSesiuneMesaje, "[%ld, %s] %s\n", timpIntroducere, tipMesaj, message);

        g_free(mesajDeAdaugat);
    } else {
        fprintf(fSesiuneMesaje, "EROARE FATALĂ: Nu am putut aloca spațiul necesar pentru găzduirea mesajului de înregistrare!\n");
    }
}
