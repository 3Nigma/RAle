/* psAle - If Ale's board ever needed an app, then this would be IT!
   Well, techinically this is the installer, but you get the ideea! ;-)
# Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
# 
# This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
# COPYING file that resides in the same directory as this file does or send a letter to 
# Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA. */

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
  
  ExecWait '"$EXEDIR\drivere\DPInst.exe" /sw /c /lm /sh /path "$EXEDIR\drivere"' $R0
  DetailPrint "Rezultatul instalării driverelor: $R0"
 
  Pop $R0
SectionEnd

Section "Aplicația principală" psAleInstall_ID
  DetailPrint "Instalez psAle ..."
  nsisunz::UnzipToLog "$EXEDIR\pachete\psAle.zip" "$INSTDIR"
 
  WriteRegStr HKLM "Software\tuScale\psAle" "CaleInstalare" "$INSTDIR"
 
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
  CopyFiles "$EXEDIR\documentatie\*" "$INSTDIR\documente" 37798
  
  DetailPrint "S-a terminat de construit bibliografia."
SectionEnd

Section "Creează legături" creeazaLegaturi_ID
  /* Creează scurtături la regiunea de Start */
  CreateDirectory "$SMPROGRAMS\psAle"
  ${If} ${SectionIsSelected} ${docsInstall_ID}
    CreateDirectory "$SMPROGRAMS\psAle\Bibliografie"
    CreateShortCut "$SMPROGRAMS\psAle\Bibliografie\Foi de Catalog.lnk" "$INSTDIR\documente\foi de catalog"
    CreateShortCut "$SMPROGRAMS\psAle\Bibliografie\Aplicații.lnk" "$INSTDIR\documente\soft"
  ${EndIf}
  	
  CreateShortCut "$SMPROGRAMS\psAle\Editor.lnk" "$INSTDIR\rpsale.exe" "" "$INSTDIR\psale_icoana_ap.ico"
  CreateShortCut "$SMPROGRAMS\psAle\Elimină aplicația.lnk" "$INSTDIR\dezinstaleaza.exe" "" "$INSTDIR\dezinstaleaza.exe" 0
	
  /* Creează scurtături pe 'Desktop' */
  CreateShortCut "$DESKTOP\psAle.lnk" "$INSTDIR\rpsale.exe" "" "$INSTDIR\psale_icoana_ap.ico"
SectionEnd

/* Creează o secțiune dummy, ascunsă, pentru a face alte operațiuni la instalare */
Section "-sect. ascunsa"
  push $0
  
  call existaCelPutinOSelectieDeComponente
  pop $0

  ${If} $0 == true
    WriteUninstaller "$INSTDIR\dezinstaleaza.exe"
  ${EndIf}
  
  pop $0
SectionEnd

Section "Uninstall"
  /* Șterge  legăturile din regiștri a aplicației principale precum și a WinAvr-ului doar dacă noi l-am instalat */
  DeleteRegKey HKLM "Software\tuScale\psAle"
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
  SectionSetSize ${wAvrAppInstall_ID} 241647
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
  
  Call obtineCalePsAle_dinReg
  ${If} $1 != ""
    !insertmacro UnselectSection ${psAleInstall_ID}
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

Function obtineCalePsAle_dinReg
  /* $1 va conține calea de instalare a lui psAle, culeasă din registry */
  Push $0
  
  EnumRegValue $0 HKLM "Software\tuScale\psAle" 0
  ReadRegStr $1 HKLM "Software\tuScale\psAle" $0
  
  Pop $0
FunctionEnd

Function existaCelPutinOSelectieDeComponente
  ${If} ${SectionIsSelected} ${wAvrAppInstall_ID}
    goto existaCelPutinOSelectie
  ${Else}
    ${If} ${SectionIsSelected} ${docsInstall_ID}
      goto existaCelPutinOSelectie
    ${Else}
       ${If} ${SectionIsSelected} ${psAleInstall_ID}
         goto existaCelPutinOSelectie
       ${Else} 
         ${If} ${SectionIsSelected} ${creeazaLegaturi_ID}
           goto existaCelPutinOSelectie
         ${EndIf}
       ${EndIf}
    ${EndIf}
  ${EndIf}
  
  Push false
  Return
  
  existaCelPutinOSelectie:
  Push true
FunctionEnd
