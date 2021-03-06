/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#ifndef SDA_H
#define	SDA_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#define PSALE_NUME_AUTOR "Victor ADĂSCĂLIȚEI"
#define PSALE_FORMAT_VERSIUNE_PRINTF "%u.%u"
#define PSALE_FORMAT_VERSIUNE_REGEX "([0-9]+)\\.([0-9]+)"
#define PSALE_TEXT_DESPRE_TITLU "Bună,\n"
#define PSALE_TEXT_DESPRE_CORP \
  "Aplicația de față a fost construită pentru a fi utilizată împreună cu 'Pachetul educațional Ale' a comunității tuscale.ro.\n"\
  "Scopul ei este acela de a facilita interacțiunea, într-o manieră ușoară și intuitivă, cu plăcuța proiectului.\n\n"\
  "Pentru orice nedumeriri/sugestii/reclamații ne găsiți și vă răspundem cu drag în comunitatea proiectului.\n\n"

#define PSALE_BD_NUME_FIS        "date.sqlite"
#define DB_NUME_TABEL_EXEMPLE    "exemple"
#define DB_NUME_TABEL_MEDIA      "media"
#define DB_NUME_TABEL_META       "meta"
#define DB_NUME_TABEL_MODIFICARI "modificari"

#ifdef G_OS_WIN32
#define DB_NUME_CAMP_LINK_ACTUALIZARE "LinkReinoireWin"
#elif defined G_OS_UNIX
#define DB_NUME_CAMP_LINK_ACTUALIZARE "LinkReinoireUnix"
#else
#error "Nu cunosc adresa de actualizare pentru sistemul de operare gazda!"
#endif
    
#define OS_NUME_RPSALE "rpsale"
#ifdef G_OS_WIN32
#define ACTUALIZATOR_NUME_AP OS_NUME_RPSALE ".exe"
#elif defined G_OS_UNIX
#define ACTUALIZATOR_NUME_AP OS_NUME_RPSALE
#endif
#define OS_CALE_RPSALE "./" ACTUALIZATOR_NUME_AP

#define RPS_NUME_PSALE "psale"
#ifdef G_OS_WIN32
#define RPS_CALE_PSALE "./" RPS_NUME_PSALE".exe"
#elif defined G_OS_UNIX
#define RPS_CALE_PSALE "./" RPS_NUME_PSALE
#endif

#define ACTUALIZATOR_NUME_ACTUALIZAT "rpsale.nou"

#define DL_LUNGIME_MAX_URL            1024
#define DL_LUNGIME_MAX_MESAJ_VERSIUNE 3000
#define DL_CURL_TIMEOUT               5

#define FPRIN_PACTUALIZAT_SEC_VIATA_ETICH  8
#define FPRIN_PACTUALIZAT_CULOARE_FUNDAL   "#01DF3A"

    typedef struct {
        guint major;
        guint minor;
    } Versiune;

    typedef struct {
        Versiune vers;
        char *adresaDeDescarcare;
        char *mesajModificari;
    } IntrareActualizare;

    typedef struct {
        Versiune vers;
        gchar *detalii;
    } BDIntrareTabelModificare;

    extern Versiune *sda_obtineVersiuneDinSir(gchar *sir);
    extern gint sda_comparaVersiuni(Versiune *v1, Versiune *v2);

    extern IntrareActualizare *sda_intrare_actualizare_initializeaza();
    extern void sda_intrare_actualizare_curata(IntrareActualizare **iai);
    extern void sda_intrare_actualizare_copiaza(IntrareActualizare *sursa, IntrareActualizare *destinatie);
    extern gchar *sda_versiune_printf(Versiune *vers);
    extern FILE *sda_fopen_mkdir(const char *caleFis, const char *mode);
    extern gboolean sda_este_cale_fisier_valida(const char *caleFis);
    
    extern gchar *sda_conv_sir_din_hex_in_binar(const gchar *valHex, gchar *valRezultata);
    extern gchar *sda_conv_sir_din_hex_in_zecimal(const gchar *valHex, gchar *valRezultata);
#ifdef	__cplusplus
}
#endif

#endif	/* SDA_H */
