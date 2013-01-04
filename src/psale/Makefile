# psAle - If Ale's board ever needed an app, then this would be IT!
# Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
# 
# This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
# COPYING file that resides in the same directory as this file does or send a letter to 
# Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

# --- Zonă de variabile ---
include ./Makefile.defs

PRINCIPAL_DIR    := ./principal
REINOITOR_DIR    := ./reinoitor
DIR_CONSTRUIT    := ./construit
DIR_CARTULIE_ODT := ../../doc

PSALE_DIR_MEDIA_NUME   := media
PSALE_DIR_LIMBAJE_NUME := limbaje
PSALE_DIR_STILURI_NUME := stiluri

FIS_ANTETELE_MELE:= ale.inc ale.h
LIBREOFF_NUME_BIN:= libreoffice

DEPENDENTE_DIR_PRINCIPAL := $(PRINCIPAL_DIR)/$(DIR_SURSE_MELE) \
                            $(PRINCIPAL_DIR)/$(PSALE_DIR_MEDIA_NUME) \
                            $(PRINCIPAL_DIR)/$(PSALE_DIR_LIMBAJE_NUME) \
                            $(PRINCIPAL_DIR)/$(PSALE_DIR_STILURI_NUME)
DEPENDENTE_ADITIONALE := $(DIR_CONSTRUIT)/$(SQLITEDB_NAME) $(DIR_CONSTRUIT)/$(NUME_CERT_PEM)
TIPCONSTRUCTIE :=

# --- Sfârșit zonă de variabile și început de zonă de rețete ---

all : asigura_existenta_directoare \
      $(DEPENDENTE_DIR_PRINCIPAL) $(DEPENDENTE_ADITIONALE)
	@cd $(PRINCIPAL_DIR); make $(TIPCONSTRUCTIE)
	@cd $(REINOITOR_DIR); make
	cp $(PRINCIPAL_DIR)/$(PRIN_BIN_NAME) $(DIR_CONSTRUIT)/$(PRIN_BIN_NAME)
	cp $(REINOITOR_DIR)/$(REIN_BIN_NAME) $(DIR_CONSTRUIT)/$(REIN_BIN_NAME)
	@cd $(DIR_CONSTRUIT); make $(TIPCONSTRUCTIE)
	
win : TIPCONSTRUCTIE := win
win : LIBREOFF_NUME_BIN := soffice
win : all

linux : TIPCONSTRUCTIE :=
linux : $(DIR_CONSTRUIT)/$(CART_PDF_NAME)
linux : all

# Rețetă de construire a cărțuliei
$(DIR_CONSTRUIT)/$(CART_PDF_NAME) : $(DIR_CARTULIE_ODT)/$(CART_ODT_NAME)
	@echo "Construiesc cartulia si o aduc langa aplicatie ..."
	@$(LIBREOFF_NUME_BIN) --invisible --convert-to pdf --outdir $(DIR_CONSTRUIT) $(DIR_CARTULIE_ODT)/$(CART_ODT_NAME)
	@mv $(DIR_CONSTRUIT)/$(subst .odt,.pdf,$(CART_ODT_NAME)) $(DIR_CONSTRUIT)/$(CART_PDF_NAME)

# Rețetă de construire a directoarelor necesare construcției
.PHONY : asigura_existenta_directoare
asigura_existenta_directoare :
	@mkdir -p $(DIR_CONSTRUIT)

# Transportă în directorul construit alte fișiere necesare
$(DIR_CONSTRUIT)/$(NUME_CERT_PEM) : $(REINOITOR_DIR)/$(NUME_CERT_PEM)
$(DIR_CONSTRUIT)/$(SQLITEDB_NAME) : $(PRINCIPAL_DIR)/$(SQLITEDB_NAME)

$(DEPENDENTE_ADITIONALE) :
	cp "$<" "$@"

# Sincronizează subdirectoarele din principal cu cele din construit
.PHONY : $(DEPENDENTE_DIR_PRINCIPAL)
$(DEPENDENTE_DIR_PRINCIPAL):
	@echo "Sincronizez directorul '$@' ..."
	@rsync -rupE "$@" "$(DIR_CONSTRUIT)/"

# Rețeta de curățare
.PHONY : clean
clean :
	@cd $(PRINCIPAL_DIR); make clean
	@cd $(REINOITOR_DIR); make clean
	@cd $(DIR_CONSTRUIT); make clean
	@rm -f $(DEPENDENTE_ADITIONALE) $(DIR_CONSTRUIT)/$(CART_PDF_NAME)
	@rm -fr $(subst $(PRINCIPAL_DIR), $(DIR_CONSTRUIT), $(DEPENDENTE_DIR_PRINCIPAL))
