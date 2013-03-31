/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "bic.h"

#include <string.h>

#define MAX_SHOWN_IBLINES_CNT          5
#define LINE_PART_REFORMULATION_FORMAT "\n<b><a href=\"%s\" title=\"Sari la respectiva linie\">Linia %s</a></b>: "

static struct {
    const gchar *tiparGcc;
    const gchar *tiparInlocuire;
    gint nrParsariInlocuire;
} InlocuiriMesajeGcc[] = {
    {"\\): undefined reference to `([\\w]+)", "\nNu cunosc adresa '%s'.", 1}, /* [0] = referința lipsă */
    {"Assembler messages", NULL, -1}, /* --- */
    {"ld returned 1 exit status", NULL, -1}, /* --- */
    {"\\.o: In function `", NULL, -1}, /* --- */
    {"Warning: end of file not at end of a line", "\nVă rugăm adăugați o linie goală la sfârșitul codului dumneavoastră.", 0}, /* --- */
    {"Warning: end of file in comment", "\nVă rugăm adăugați o linie goală la sfârșitul codului dumneavoastră.", 0}, /* --- */
    {"([\\d]+): Error: junk at end of line, first unrecognized character is `(\\w)", LINE_PART_REFORMULATION_FORMAT "Sfârșitul liniei conține caractere necunoscute! Primul este '%s'.", 2}, /* [0] = nr. linie, [1] = primul caracter problematic */
    {"([\\d]+): Error: number must be positive and less than ([\\d]+)", LINE_PART_REFORMULATION_FORMAT "Numărul trebuie să fie pozitiv și mai mic decât %s.", 2}, /* [0] = nr. linie, [1] = limita superioară a valorii */
    {"([\\d]+): Error: unknown opcode `([\\w]+)", LINE_PART_REFORMULATION_FORMAT "Nu recunosc instrucțiunea '%s'.", 2}, /* [0] = nr. linie, [1] = cuvântul neînțeles */
    {"([\\d]+): Error: unknown pseudo-op: `(.[\\w]+)", LINE_PART_REFORMULATION_FORMAT "Nu recunosc directiva '%s'.", 2}, /* [0] = nr. linie, [1] = directiva necunoscută */
    {"([\\d]+): Error: garbage at end of line", LINE_PART_REFORMULATION_FORMAT "Linia se termină prin caractere invalide.", 1}, /* [0] = nr. linie */
    {"([\\d]+): Error: register name or number from 0 to 31 required", LINE_PART_REFORMULATION_FORMAT "Registrul general nu este cunoscut! Alegeți unul din domeniul {0-31}.", 1}, /* [0] = nr. linie */
    {"([\\d]+): Error: `(.)' required", LINE_PART_REFORMULATION_FORMAT "Necesită caracterul '%s'", 2}, /* [0] = nr. linie, [1] = caracterul ce lipsește */
    {"([\\d]+): Error: constant value required", LINE_PART_REFORMULATION_FORMAT "Necesită o valoare constantă.", 1}, /* [0] = nr. linie */
    {"([\\d]+): Error: pointer register (.) required", LINE_PART_REFORMULATION_FORMAT "Necesită registrul '%s'.", 2}, /* [0] = nr. linie, [1] = registrul ce lipsește */
    {"([\\d]+): Error: missing operand", LINE_PART_REFORMULATION_FORMAT "Nu are operand.", 1}, /* [0] = nr. linie */
    {"([\\d]+): Error: (.+)", LINE_PART_REFORMULATION_FORMAT "%s", 2}, /* nu am recunoscut mesajul curent, încearcă o procesare generică */
    {NULL, NULL, 0}
};

static gchar *transformaMesajulCompilatorului(const gchar *textOriginal);
static GMatchInfo *aplicaTiparPrezent(const gchar *txtTipar, const gchar *txtPropriu);
static gboolean raspundeLaClickPeLinie(GtkLabel *lbl, gchar *uri, BaraInfoCod *bics);

void bic_initializeaza(BaraInfoCod *bicStructure) {
    if (bicStructure == NULL) return;

    GtkWidget *baraInfoContent = NULL;

    /* inițializează câmpurile structurii */
    bicStructure->wid = gtk_info_bar_new_with_buttons("Ascunde înștiințarea", GTK_RESPONSE_CLOSE, NULL);
    bicStructure->text = gtk_label_new(NULL);
    bicStructure->cereRepozitionareaCursorului = NULL;
    bicStructure->nrLiniiUtileAfisate = 0;

    baraInfoContent = gtk_info_bar_get_content_area(GTK_INFO_BAR(bicStructure->wid));
    gtk_container_add(GTK_CONTAINER(baraInfoContent), bicStructure->text);

    /* leagă evenimentele */
    g_signal_connect(bicStructure->wid, "response", G_CALLBACK(gtk_widget_hide), NULL);
    g_signal_connect(bicStructure->text, "activate-link", G_CALLBACK(raspundeLaClickPeLinie), bicStructure);
}

void bic_arata(BaraInfoCod *bicStructure) {
    if (NULL == bicStructure) return;

    gtk_info_bar_set_message_type(GTK_INFO_BAR(bicStructure->wid), GTK_MESSAGE_WARNING);
    gtk_widget_show(bicStructure->wid);
}

void bic_ascunde(BaraInfoCod *bicStructure) {
    if (bicStructure == NULL) return;

    gtk_widget_hide(bicStructure->wid);
}

void bic_text_initializeaza(BaraInfoCod *bicStructure) {
    if (NULL == bicStructure) return;

    bicStructure->areLiniiUtile = FALSE;
    bicStructure->nrLiniiUtileAfisate = 0;
    gtk_label_set_markup(GTK_LABEL(bicStructure->text), "<i>Am următoarele de raportat :</i>");
}

void bic_text_adauga_linie(BaraInfoCod *bicStructure, const gchar *msg) {
    if (NULL == bicStructure || NULL == msg) return;

    gchar *currentIBLblText = NULL;
    gchar *reformulatedMessage = NULL;

    if (bicStructure->nrLiniiUtileAfisate > MAX_SHOWN_IBLINES_CNT) return;

    if (bicStructure->nrLiniiUtileAfisate < MAX_SHOWN_IBLINES_CNT) {
        reformulatedMessage = transformaMesajulCompilatorului(msg);
    } else /* bicStructure->nrLiniiUtileAfisate == MAX_SHOWN_IBLINES_CNT */ {
        reformulatedMessage = g_new(gchar, 64);
        g_snprintf(reformulatedMessage, 64, "\n<i>... și alte informații</i>");
    }
    currentIBLblText = g_strconcat(gtk_label_get_label(GTK_LABEL(bicStructure->text)), reformulatedMessage, NULL);

    gtk_label_set_markup(GTK_LABEL(bicStructure->text), currentIBLblText);
    bicStructure->areLiniiUtile = TRUE;
    bicStructure->nrLiniiUtileAfisate++;

    g_free(currentIBLblText);
    g_free(reformulatedMessage);
}

gboolean bic_text_contine_informatii_utile(BaraInfoCod *bicStructure) {
    if (NULL == bicStructure) return FALSE;

    return bicStructure->areLiniiUtile;
}

static gchar *transformaMesajulCompilatorului(const gchar *textOriginal) {
    gchar *textRezultat = NULL;
    GMatchInfo *infoTipar = NULL;
    gint idStructuraTiparMesaj;

    textRezultat = g_new0(gchar, 256);

    for (idStructuraTiparMesaj = 0; InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparGcc != NULL; ++idStructuraTiparMesaj) {
        if ((infoTipar = aplicaTiparPrezent(InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparGcc, textOriginal)) != NULL) {
            switch (InlocuiriMesajeGcc[idStructuraTiparMesaj].nrParsariInlocuire) {
                case -1:
                    break;
                case 0:
                    g_stpcpy(textRezultat, InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparInlocuire);
                    break;
                case 1:
                    g_snprintf(textRezultat, 256, InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparInlocuire,
                            g_match_info_fetch(infoTipar, 1), g_match_info_fetch(infoTipar, 1));
                    break;
                case 2:
                    g_snprintf(textRezultat, 256, InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparInlocuire,
                            g_match_info_fetch(infoTipar, 1), g_match_info_fetch(infoTipar, 1),
                            g_match_info_fetch(infoTipar, 2));
                    break;
                default:
                    g_warning("Mesajul gcc are un număr negestionat de parametrii!");
                    break;
            }
            g_match_info_free(infoTipar);
            break;
        }
    }

    if (InlocuiriMesajeGcc[idStructuraTiparMesaj].tiparGcc == NULL) {
        /* nu s-a potrivit niciun tipar în nici un fel. Întoarce textul inițial */
        g_snprintf(textRezultat, 256, "%s", textOriginal);
    }

    return textRezultat;
}

static GMatchInfo *aplicaTiparPrezent(const gchar *txtTipar, const gchar *txtPropriu) {
    GRegex *tipar = NULL;
    GMatchInfo *containerPotriviri = NULL;

    tipar = g_regex_new(txtTipar,
            0, 0, NULL);
    g_regex_match(tipar, txtPropriu, 0, &containerPotriviri);
    if (!g_match_info_matches(containerPotriviri)) {
        containerPotriviri = NULL;
    }

    g_regex_unref(tipar);

    return containerPotriviri;
}

static gboolean raspundeLaClickPeLinie(GtkLabel *lbl, gchar *uri, BaraInfoCod *bics) {
    /* funcția este apelată atunci când utilizatorul dă click pe un mesaj ('Linie %d') din InfoBar 
     * 'uri' este numărul liniei la care face referire observația din InfoBar */
    gint64 nrLinie = 0;
    GtkTextIter iterLinie;

    nrLinie = g_ascii_strtoll(uri, NULL, 10);
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(bics->parentBuff), &iterLinie, (gint) (nrLinie - 1));

    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(bics->parentView), &iterLinie, 0.0, TRUE, 0.0, 0.5);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(bics->parentBuff), &iterLinie);

    return TRUE;
}
