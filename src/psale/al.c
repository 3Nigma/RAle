/*
 * psAle - If Ale's board ever needed an app, then this would be IT!
 * Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
 * 
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
 * COPYING file that resides in the same directory as this file does or send a letter to 
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <stdlib.h>

#include "al.h"

gboolean 
al_este_placuta_conectata() {
  int avrdudeRet = 0;

#ifdef G_OS_WIN32
  /* DE FĂCUT : obține același efect pentru windows */
#elif defined G_OS_UNIX
  avrdudeRet = system("sudo avrdude -c usbtiny -p t25 -V 2> /dev/null");
#endif
  
  if(WIFEXITED(avrdudeRet))
    return WEXITSTATUS(avrdudeRet) == 0;

  return FALSE;
}
