#include "os.h"

#include <glib/gprintf.h>

DWORD 
os_win_executa_com(HANDLE *hConsoleOutput, HANDLE *hConsoleInput, HANDLE *hConsoleError, gchar *command) {
  STARTUPINFO startupInfo;
  PROCESS_INFORMATION processInformation;

  ZeroMemory(&startupInfo, sizeof(startupInfo));
  ZeroMemory(&processInformation, sizeof(processInformation));

  startupInfo.cb = sizeof(startupInfo);	
  startupInfo.dwFlags = STARTF_USESTDHANDLES;
  startupInfo.hStdOutput = hConsoleOutput;
  startupInfo.hStdError = hConsoleError;
  startupInfo.hStdInput = hConsoleInput;

  if(CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation)) {
    WaitForSingleObject(processInformation.hProcess, INFINITE);
    
	DWORD returnCode;
    GetExitCodeProcess(processInformation.hProcess, &returnCode);
	CloseHandle(hConsoleOutput);
	CloseHandle(hConsoleError);
	CloseHandle(hConsoleInput);
    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);
	
    return returnCode;
  } else {
    DWORD errorCode = GetLastError();

    return errorCode;
  }

  return -1;
}

DWORD 
os_win_executa_com_fara_redirectionari(gchar *command) {
  return os_win_executa_com(NULL, NULL, NULL, command);
}

unsigned long 
os_system(gchar *command) {
#ifdef G_OS_WIN32
  return os_win_executa_com_fara_redirectionari(command);
#elif defined G_OS_UNIX
  int retVal = system(command);
  
  if(WIFEXITED(retVal)) {
    return WEXITSTATUS(retVal);
  } else {
    return -1;
  }
#endif
}

gboolean
os_executa_si_completeaza_bic_fc(gchar *comanda, BaraInfoCod *baraInfoTinta) {
  g_assert(comanda != NULL);
  g_assert(baraInfoTinta != NULL);

  char lineBuff[4096];
  gboolean gccCompilatCuSucces = TRUE;
  
#ifdef G_OS_WIN32
  gchar *linePt = lineBuff;
  HANDLE avrGccOuput[2];  /* [0] = Read, [1] = Write */
  DWORD nrOctetiCititi; 
  SECURITY_ATTRIBUTES securityAttributes;
  
  /* construim structura de siguran?ã a pipe-ului */
  securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES); 
  securityAttributes.bInheritHandle = TRUE; 
  securityAttributes.lpSecurityDescriptor = NULL;
   
  /* construim efectiv pipe-ul */
  CreatePipe(&avrGccOuput[0], &avrGccOuput[1], &securityAttributes, 0); 
  
  if(os_win_executa_com(avrGccOuput[1], NULL, avrGccOuput[1], comanda) != 0) {
    gccCompilatCuSucces = FALSE;
    bic_text_initializeaza(baraInfoTinta);
  
    while(ReadFile(avrGccOuput[0], linePt, 1, &nrOctetiCititi, NULL)) {
      if(nrOctetiCititi == 0) {
	    break;
	  }
	  if(*linePt == '\r') {
	    (*linePt) = '\0';
	    bic_text_adauga_linie(baraInfoTinta, lineBuff);
	    linePt = lineBuff;
      } else {
	    linePt ++;
      }
    }
    CloseHandle(avrGccOuput[0]);
    CloseHandle(avrGccOuput[1]);
  }
  
#elif defined G_OS_UNIX
  FILE *fGCCOut = NULL;
  
  /* încearcã sã compilezi sursa curentã */
  if((fGCCOut = popen(comanda, "r")) == NULL) {
	g_warning("S-a întâmplat ceva cu execu?ia compilatorului!");
  } else {
	bic_text_initializeaza(baraInfoTinta);
	while(fgets(lineBuff, 4096, fGCCOut) != NULL) {
      /* s-a ob?inut un rând din fluxul stãrii returnate. 
	     Este clar cã a apãrut o eroare în cod altfel nu s-ar fi ajuns aici! */
      gccCompilatCuSucces = FALSE;
      bic_text_adauga_linie(baraInfoTinta, lineBuff);
	}
	pclose(fGCCOut);
  }
#endif

  return gccCompilatCuSucces;
}

void 
os_obtine_nume_fis_temporar(gchar *buff, gint buffLen) {
  g_assert(buffLen > L_tmpnam + 20);
  
#ifdef G_OS_WIN32
  gchar tempDir[MAX_PATH];
  gint dimSirTempDir = 0;
  
  if(GetTempPath(sizeof(tempDir)/sizeof(gchar), tempDir) == 0) {
    g_sprintf(tempDir, ".");
  } else {
    dimSirTempDir = strlen(tempDir);
    if(tempDir[dimSirTempDir - 1] == '\\') {
	  tempDir[dimSirTempDir - 1] = '\0';
	}
  }
  g_sprintf(buff, "%s%s", tempDir, tmpnam(NULL));
#elif defined G_OS_UNIX
  g_sprintf(buff, "%s", tmpnam(NULL));
#endif
  g_debug("Am creeat fi?ierul temporal '%s'", buff);
}

extern gchar * 
os_obtine_cod_mcu_prezent(gchar *comanda) {
  gchar *codMCUGasit = NULL;
  char lineBuff[4096];
  GRegex *tipar = NULL;
  GMatchInfo *containerPotriviri = NULL;
  
  tipar = g_regex_new("Device signature = ([\\w]+)", 0, 0, NULL);
  codMCUGasit = g_new0(gchar, 10); 
#ifdef G_OS_WIN32
  gchar *linePt = lineBuff;
  HANDLE avrGccOuput[2];  /* [0] = Read, [1] = Write */
  DWORD nrOctetiCititi; 
  SECURITY_ATTRIBUTES securityAttributes;
  
  /* construim structura de siguran?ã a pipe-ului */
  securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES); 
  securityAttributes.bInheritHandle = TRUE; 
  securityAttributes.lpSecurityDescriptor = NULL;
   
  /* construim efectiv pipe-ul */
  CreatePipe(&avrGccOuput[0], &avrGccOuput[1], &securityAttributes, 0); 
  
  if(os_win_executa_com(avrGccOuput[1], NULL, avrGccOuput[1], comanda) == 0) {
    while(ReadFile(avrGccOuput[0], linePt, 1, &nrOctetiCititi, NULL)) {
      if(nrOctetiCititi == 0) {
	    break;
	  }
	  if(*linePt == '\r') {
	    (*linePt) = '\0';
		
		/* s-a încãrcat o linie nouã. Aplicã tiparul ?i verificã dacã putem extrage informa?ia doritã (codul MCU-ului curent) */
		g_regex_match(tipar, lineBuff, 0, &containerPotriviri);
	    if(g_match_info_matches(containerPotriviri)) {
		  g_sprintf(codMCUGasit, "%s", g_match_info_fetch(containerPotriviri, 1));
          break;
        }
	    linePt = lineBuff;
      } else {
	    linePt ++;
      }
    }
    CloseHandle(avrGccOuput[0]);
    CloseHandle(avrGccOuput[1]);
  } else {
    g_warning("S-a ivit o problemã la invocarea lui 'avrdude' : '%s'! Ac?iunea a fost anulatã ...", comanda);
  }
#elif defined G_OS_UNIX
  FILE *fConsoleOut = NULL;

  if((fConsoleOut = popen(com, "r")) == NULL) {
	g_warning("S-a ivit o problemã la invocarea lui 'avrdude' : '%s'! Ac?iunea a fost anulatã ...", comanda);
  } else {
	while(fgets(lineBuff, 4096, fConsoleOut) != NULL) {
      g_regex_match(tipar, lineBuff, 0, &containerPotriviri);
      if(g_match_info_matches(containerPotriviri)) {
		g_sprintf(codMCUGasit, "%s", g_match_info_fetch(containerPotriviri, 1));
        break;
      }
    }
	pclose(fConsoleOut);
  }
#endif

  g_regex_unref(tipar);
  g_match_info_free(containerPotriviri);
  
  return codMCUGasit;
}
