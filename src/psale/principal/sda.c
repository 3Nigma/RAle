/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "sda.h"

Versiune *sda_obtineVersiuneDinSir(gchar *sir) {
    GRegex *tipar = NULL;
    GMatchInfo *containerPotriviri = NULL;
    Versiune *vers = NULL;

    tipar = g_regex_new(PSALE_FORMAT_VERSIUNE_REGEX, 0, 0, NULL);
    g_regex_match(tipar, sir, 0, &containerPotriviri);
    if (g_match_info_matches(containerPotriviri)) {
        vers = g_new0(Versiune, 1);
        vers->major = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 1), NULL, 10);
        vers->major = g_ascii_strtoull(g_match_info_fetch(containerPotriviri, 2), NULL, 10);
    }

    g_regex_unref(tipar);
    g_match_info_free(containerPotriviri);
    return vers;
}
