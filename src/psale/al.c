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
