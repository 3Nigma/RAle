@setlocal enableextensions
@cd /d "%~dp0"
@echo off

REM ... un mic si sugestiv preambul
echo Programul defata este responsabil cu descarcarea unei versiuni specifice a
echo aplicatiei psAle.
echo Pe scurt, "psAle" este aplicatia "Pachetului educational Ale", ce permite,
echo printre altele, comunicarea cu placuta.
echo Pentru informatii suplimentare, vizitati pagina comunitatii: www.tuscale.ro.
echo.

set adrListaVersiuni="http://www.tuscale.ro?wp_ct=1"

REM Afisam versiunile disponibile
echo Descarc lista de versiuni disponibile ...
curl --progress-bar --insecure --location --referer "aducator_psale" %adrListaVersiuni% | grep -o -e "^[0-9]\{1,\}\.[0-9]\{1,\}" > fisTempVersiuni
if errorlevel 1 (
  echo !!! NU m-am putut conecta cu serverul de date !!!
  echo Problema poate fi temporara, caz in care puteti incerca un pic mai tarziu.
  echo Daca totusi nu reusiti sa prindeti legatura, vizitati comunitatea si poate
  echo o rezolvam acolo.
  echo Cu permisiunea dumneavoastra, eu voi incheia aici aplicatia.
  
  pause
  del fisTempVersiuni
  exit /B
)
set /p versDisponibile=<fisTempVersiuni

:introduceti_versiunea_dorita
echo Versiuni de nuclee stiute: 
type fisTempVersiuni
echo.

REM Regiunea variabilelor de configurare
set /p versCurenta=Versiunea dorita (lasati liber pentru a alege ultima): 
if /i {%versCurenta%}=={} (
  grep -o -e "^[0-9]\{1,\}\.[0-9]\{1,\}" fisTempVersiuni | head --lines=1 > fisTemp
  set /p versCurenta=<fisTemp
  del fisTemp
  goto :versiune_aleasa_cu_succes
)

grep -o "%versCurenta%" fisTempVersiuni > fisTempOp
set /p valoareControl=<fisTempOp
del fisTempOp  
if /i {%valoareControl%}=={} (
  echo Versiunea specificata nu exista! Alegeti alta din lista de mai jos...
  set versCurenta=
  goto :introduceti_versiunea_dorita
) else (
  REM Versiunea invocata este valida. Continuam cu achizitia ...
)

:versiune_aleasa_cu_succes
echo Am configurat descarcatorul pentru a obtine versiunea %versCurenta% ...
del fisTempVersiuni

REM Configuram o serie de variabile utile
set dirTinta=psAle_v%versCurenta%

set numeArhivaNucleu=nucleu_psAle_v%versCurenta%.zip
set numeArhivaDependente=dependente.zip
set numeArhivaCompilator=compilator.zip

set adresaNucleu=https://s3-eu-west-1.amazonaws.com/tuscale/Pachetul+educa%%C8%%9Bional+Ale/psAle/Nuclee/%numeArhivaNucleu%
set adresaDependente=https://s3-eu-west-1.amazonaws.com/tuscale/Pachetul+educa%%C8%%9Bional+Ale/psAle/Statice/%numeArhivaDependente%
set adresaCompilator=https://s3-eu-west-1.amazonaws.com/tuscale/Pachetul+educa%%C8%%9Bional+Ale/psAle/Statice/%numeArhivaCompilator%

REM == MAI JOS DE ACEASTA LINIE, NU MODIFICATI DECAT DACA STITI CE FACETI! ==
title Aducator de psAle v%versCurenta%...
mkdir %dirTinta%

REM aduc nucleul versiunii cerute
if not exist %dirTinta%\%numeArhivaNucleu% (
  echo Descarc nucleul aplicatiei psAle v%versCurenta% ...
  curl --progress-bar --insecure --remote-name %adresaNucleu%
  move %numeArhivaNucleu% %dirTinta%\%numeArhivaNucleu%
  echo Arhiva s-a descarcat cu succes ...
) else (
  echo Nu descarc nucleul pentru ca el se pare ca exista deja!
)

echo Despachetez nucleul ...
7za x -y -o%dirTinta% %dirTinta%\%numeArhivaNucleu%
echo Am terminat de despachetat arhiva ...
echo.

REM aduc conitionat dependentele
SET /P raspunsIntrebare=Doriti sa descarcati si dependentele aplicatiei odata cu aplicatia principala (D/N)? 
if /i {%raspunsIntrebare%}=={d} (goto :descarca_dependente)
if /i {%raspunsIntrebare%}=={da} (goto :descarca_dependente)
goto :nu_descarca_dependente

:descarca_dependente
if not exist %dirTinta%\%numeArhivaDependente% (
  echo Descarc dependentele aplicatiei ...
  curl --progress-bar --insecure --remote-name %adresaDependente%
  move %numeArhivaDependente% %dirTinta%\%numeArhivaDependente%
  echo Arhiva s-a descarcat cu succes ...
) else (
  echo Nu descarc dependentele pentru ca pachetul se pare ca exista deja!
)

echo Despachetez dependentele ...
7za x -y -o%dirTinta% %dirTinta%\%numeArhivaDependente%
echo Am terminat de despachetat pachetul ...
echo.

:nu_descarca_dependente
REM aduc conditionat compilatorul
SET /P raspunsIntrebare=Doriti sa descarcati si compilatorul WinAvr odata cu aplicatia principala (D/N)? 
if /i {%raspunsIntrebare%}=={d} (goto :descarca_compilator)
if /i {%raspunsIntrebare%}=={da} (goto :descarca_compilator)
goto :nu_descarca_compilatorul

:descarca_compilator
if not exist %dirTinta%\%numeArhivaCompilator% (
  echo Descarc compilatorul aplicatiei ...
  curl --progress-bar --insecure --remote-name %adresaCompilator%
  move %numeArhivaCompilator% %dirTinta%\%numeArhivaCompilator%
  echo Arhiva s-a descarcat cu succes ...
) else (
  echo Nu descarc compilatorul pentru ca acesta se pare ca exista deja!
)

echo Despachetez compilatorul ...
7za x -y -o%dirTinta% %dirTinta%\%numeArhivaCompilator%
echo Am terminat de despachetat ...

REM adaugam caile interne ale compilatorulu conditionat la variabila de mediu PATH
SET /P raspunsIntrebare=Doriti sa adaugati aplicatiile uzuale ale compilatorului la variabila de mediu PATH (D/N)? 
if /i {%raspunsIntrebare%}=={d} (goto :adauga_in_path)
if /i {%raspunsIntrebare%}=={da} (goto :adauga_in_path)
goto :nu_adauga_in_path

:adauga_in_path
reg add "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_SZ /d "%path%;%cd%\%dirTinta%\winavr\bin"

:nu_adauga_in_path
:nu_descarca_compilatorul
echo Curat arhivele folosite ...
if exist %dirTinta%\%numeArhivaNucleu%. del /F /Q %dirTinta%\%numeArhivaNucleu%.
if exist %dirTinta%\%numeArhivaDependente%. del /F /Q %dirTinta%\%numeArhivaDependente%.
if exist %dirTinta%\%numeArhivaCompilator%. del /F /Q %dirTinta%\%numeArhivaCompilator%.

echo.
echo Gata! Distractie, sper, placuta! ...

REM Apel prostesc :-D
curl --location --silent "http://www.tuscale.ro?wp_ct=2"

pause
