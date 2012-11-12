!include WordFunc.nsh
!insertmacro VersionCompare
 
!include LogicLib.nsh
!include Sections.nsh
!include EnvVarUpdate.nsh
!include WinVer.nsh

Name "psAle"
OutFile "CD/instalator.exe"
Icon "psale_icoana_cd.ico"
UninstallIcon "psale_icoana_dezinstalare.ico"
InstallDir "$PROGRAMFILES\tuScale\psAle"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Romanian.nlf"
LicenseData "psale_licenta.rtf"
AutoCloseWindow true

XPStyle on
CRCCheck on
RequestExecutionLevel admin

Page license
Page components
Page directory directorAles
Page instfiles

Section /o "-Driverele necesare aplicației" drvInstall_ID
  Push $R0
  
  ExecWait '"$EXEDIR\drivere\DPInst.exe" /sw /c /lm /sh /path "$EXEDIR\drivere\usbtiny 1.12"' $R0
  DetailPrint "Rezultatul instalării driverelor: $R0"
 
  Pop $R0
SectionEnd

Section "Aplicația principală" psAleInstall_ID
  DetailPrint "Instalez psAle ..."
  nsisunz::UnzipToLog "$EXEDIR\pachete\psAle.zip" "$INSTDIR"
 
  Pop $0
  DetailPrint "Rezultatul operațiunii : $0"
  DetailPrint "S-a terminat de instalat aplicația principală psAle."
SectionEnd

Section "WinAvr și aplicații aferente ..." wAvrAppInstall_ID
  DetailPrint "Instalez WinAvr și aplicații aferente ..."
  nsisunz::UnzipToLog "$EXEDIR\pachete\compilator.zip" "$INSTDIR"
 
  Pop $0
  DetailPrint "Rezultatul operațiunii : $0"
  DetailPrint "Am terminat de instalat suita compilatorului."

  WriteRegStr HKLM "Software\WinAVR" "20100110" "$INSTDIR\winavr"
  ${EnvVarUpdate} $0 "PATH" "P" "HKLM" "$INSTDIR\winavr\bin"
  ${EnvVarUpdate} $0 "PATH" "P" "HKLM" "$INSTDIR\winavr\utils\bin"
SectionEnd

Section "Documentație" docsInstall_ID
  DetailPrint "Instalez bibliografia ..."
  
  CreateDirectory "$INSTDIR\documente"
  CopyFiles "$EXEDIR\documentatie\*" "$INSTDIR\documente"
  
  DetailPrint "S-a terminat de construit bibliografia."
SectionEnd

/* Creează o secțiune dummy, ascunsă, pentru a face alte operațiuni la instalare */
Section "-sect. ascunsa"
  WriteUninstaller "$INSTDIR\dezinstaleaza.exe"
SectionEnd

Section "Creează legături"
  /* Creează scurtături la regiunea de Start */
  CreateDirectory "$SMPROGRAMS\psAle"
  ${If} ${SectionIsSelected} ${docsInstall_ID}
    CreateShortCut "$SMPROGRAMS\psAle\Bibliografie.lnk" "$INSTDIR\documente\"
  ${EndIf}
  	
  CreateShortCut "$SMPROGRAMS\psAle\Editor.lnk" "$INSTDIR\rpsale.exe" "" "$INSTDIR\psale_icoana_ap.ico"
  CreateShortCut "$SMPROGRAMS\psAle\Elimină aplicația.lnk" "$INSTDIR\dezinstaleaza.exe" "" "$INSTDIR\dezinstaleaza.exe" 0
	
  /* Creează scurtături pe 'Desktop' */
  CreateShortCut "$DESKTOP\psAle.lnk" "$INSTDIR\rpsale.exe" "" "$INSTDIR\psale_icoana_ap.ico"
	
  /* Acordă privilegii de administrator pentru următoarele legături */
  ${If} ${AtLeastWinVista}
	; ShellLink::SetRunAsAdministrator "$SMPROGRAMS\Aplicatii RAle\PN.lnk"
	; ShellLink::SetRunAsAdministrator "$DESKTOP\Programmer's Notepad.lnk"
  ${Endif}
SectionEnd

Section "Uninstall"
  /* Șterge  legăturile din regiștri a WinAvr-ului doar dacă noi l-am instalat */
  IfFileExists $INSTDIR\winavr\*.* 0 dupa_curatarea_registrilor
    DeleteRegKey HKLM "Software\WinAVR"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\winavr\bin"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\winavr\utils\bin"
  
  dupa_curatarea_registrilor:
    /* Șterge legăturile de pe Desktop, directorul de instalare + scurtăturile de la Start */
    Delete "$DESKTOP\psAle.lnk"

    RMDir /r "$SMPROGRAMS\psAle"
    RMDir /r /REBOOTOK $INSTDIR
SectionEnd

Function .onInit
  /* Arată splash-screen-ul aplicației */
  InitPluginsDir
  File /oname=$PLUGINSDIR\baner_instalator.bmp "baner_instalator.bmp"
  advsplash::show 2200 750 500 -1 $PLUGINSDIR\baner_instalator
  
  /* Definim dimensiunile fiecărei secțiuni în parte */
  SectionSetSize ${drvInstall_ID} 70
  SectionSetSize ${wAvrAppInstall_ID} 273440
  SectionSetSize ${docsInstall_ID} 9134
  SectionSetSize ${psAleInstall_ID} 38115
  
  /* Adaugă detalii la secțiuni */
  
  Call esteRuntimeGtkInstalat
  Pop $0
  ${If} $0 == "nu"
    goto gtk_neinstalat
  ${EndIf}
  
  SectionSetFlags ${drvInstall_ID} ${SF_SELECTED}
  
  /* marchează forțat opțiunea instalării aplicației principale psAle */
  ;IntOp $0 ${SF_SELECTED} | ${SF_RO}
  ;SectionSetFlags ${psAleInstall_ID} $0
  
  gtk_neinstalat:
  
  Call obtineCaleWinAvr_dinReg
  ${If} $1 != ""
    !insertmacro UnselectSection ${wAvrAppInstall_ID}
  ${Endif}
  
FunctionEnd

Function un.onInit
  MessageBox MB_YESNO|MB_ICONQUESTION "Sunteți sigur că doriți să ștergeti pachetul de programe psAle ?$\n$\nDacă da, asigurați-vă că ați închis toate aplicațiile psAle înainte de a continua." IDYES Confirmat
    Abort
  Confirmat:
FunctionEnd

Function directorAles
  SetOutPath $INSTDIR
FunctionEnd

Function obtineCaleWinAvr_dinReg
  /* $1 va conține calea WinAvr-ului instalat, culeasă din registry */
  Push $0
  
  EnumRegValue $0 HKLM "Software\WinAVR" 0
  ReadRegStr $1 HKLM "Software\WinAVR" $0
  
  Pop $0
FunctionEnd

Function esteRuntimeGtkInstalat
  /* TODO: se va completa ulterior, dacă este posibil */
FunctionEnd
