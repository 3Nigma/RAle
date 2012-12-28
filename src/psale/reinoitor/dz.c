/*
 * rpsAle - psAle's updater application
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "dz.h"

#include <zip.h>

gboolean dz_despacheteaza_pachet(const char *numeArhiva, const char *caleDezarhivare) {
    struct zip *zFile = NULL;
    gboolean stareOp = FALSE;

    /* deschidem arhiva */
    if ((zFile = zip_open(numeArhiva, 0, NULL)) != NULL) {
        zip_uint64_t nrFisInArhiva = zip_get_num_entries(zFile, 0);
        zip_uint64_t fIndex = 0;
        struct zip_file *fisInternArhiva = NULL;
        struct zip_stat fZStat;
        zip_uint64_t restOctetiDeDecompresat = 0;
        FILE *fisDecompresat = NULL;
        char buffTransfer[2048];
        char caleCompletaFisTinta[1024];

        for (fIndex = 0; fIndex < nrFisInArhiva; ++fIndex) {
            /* obține informații despre fișierul cu indexul curent */
            zip_stat_init(&fZStat);
            zip_stat_index(zFile, fIndex, 0, &fZStat);

            /* despachetează fișierul */
            fisInternArhiva = zip_fopen_index(zFile, fIndex, 0);
            sprintf(caleCompletaFisTinta, "%s/%s", caleDezarhivare, fZStat.name);
            if (sda_este_cale_fisier_valida(caleCompletaFisTinta)) {
                if ((fisDecompresat = sda_fopen_mkdir(caleCompletaFisTinta, "wb")) != NULL) {
                    restOctetiDeDecompresat = fZStat.size;
                    do {
                        /* preluăm din arhivă și punem în fișierul despachetat */
                        if (restOctetiDeDecompresat > sizeof (buffTransfer)) {
                            zip_fread(fisInternArhiva, buffTransfer, sizeof (buffTransfer));
                            fwrite(buffTransfer, sizeof (char), sizeof (buffTransfer), fisDecompresat);
                            restOctetiDeDecompresat -= sizeof (buffTransfer);
                        } else {
                            zip_fread(fisInternArhiva, buffTransfer, restOctetiDeDecompresat);
                            fwrite(buffTransfer, sizeof (char), restOctetiDeDecompresat, fisDecompresat);
                            restOctetiDeDecompresat = 0;
                        }

                    } while (restOctetiDeDecompresat != 0);

                    /* am încheiat operația. Putem închide fișierele */
                    zip_fclose(fisInternArhiva);
                    fclose(fisDecompresat);
                } else {
                    g_warning("Nu am putut crea calea '%s' ce va ține fișierul din pachet!", caleCompletaFisTinta);
                }
            } else {
                g_debug("Sar peste entitatea '%s'. Se pare că nu este o cale de fișier valid (o fi director?).", caleCompletaFisTinta);
            }
        }
        /* curățăm memoria */
        zip_close(zFile);

        stareOp = TRUE;
    } else {
        g_warning("Nu am putut deschide arhiva pentru despachetare!");
    }

    return stareOp;
}