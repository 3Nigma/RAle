#ifndef _DL_H_
#define _DL_H_

#include <assert.h>
#include <string.h>

#include "curl/curl.h"

#include "db.h"

enum {DL_NICI_O_ACTUALIZARE = 0, DL_ACTUALIZAT, DL_PROBLEME_LA_ACTUALIZARE};

extern int 
dl_initializeaza(const char *);

extern int 
dl_acutalizeaza_bd();

extern void
dl_curata();

#endif
