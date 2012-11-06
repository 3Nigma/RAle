# psAle - If Ale's board ever needed an app, then this would be IT!
# Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
# 
# This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
# COPYING file that resides in the same directory as this file does or send a letter to 
# Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

include ./Makefile.defs

PRINCIPAL_DIR    := ./principal
REINOITOR_DIR    := ./reinoitor
DIR_CONSTRUIT    := ./construit
DIR_CARTULIE_ODT := ../../doc

NUME_CERT_PEM    := cacert.pem

DEPENDENTE_ALL := $(DIR_CONSTRUIT)/$(SQLITEDB_NAME) $(DIR_CONSTRUIT)/$(NUME_CERT_PEM)
TIPCONSTRUCTIE :=

all : asigura_existenta_directoare $(DIR_CONSTRUIT)/$(CART_PDF_NAME) $(DEPENDENTE_ALL)
	@cd $(PRINCIPAL_DIR); make $(TIPCONSTRUCTIE)
	@cd $(REINOITOR_DIR); make $(TIPCONSTRUCTIE)
	cp $(PRINCIPAL_DIR)/$(PRIN_BIN_NAME) $(DIR_CONSTRUIT)/$(PRIN_BIN_NAME)
	cp $(REINOITOR_DIR)/$(REIN_BIN_NAME) $(DIR_CONSTRUIT)/$(REIN_BIN_NAME)
	
win : TIPCONSTRUCTIE := win
win : all

$(DIR_CONSTRUIT)/$(CART_PDF_NAME) : $(DIR_CARTULIE_ODT)/$(CART_ODT_NAME)
	@echo "Construiesc cărțulia și o aduc lângă aplicație ..."
	@libreoffice --invisible --convert-to pdf --outdir $(DIR_CONSTRUIT) $(DIR_CARTULIE_ODT)/$(CART_ODT_NAME)
	@mv $(DIR_CONSTRUIT)/$(subst .odt,.pdf,$(CART_ODT_NAME)) $(DIR_CONSTRUIT)/$(CART_PDF_NAME)

.PHONY : asigura_existenta_directoare
asigura_existenta_directoare :
	mkdir -p $(DIR_CONSTRUIT)

$(DIR_CONSTRUIT)/$(NUME_CERT_PEM) : $(REINOITOR_DIR)/$(NUME_CERT_PEM)
$(DIR_CONSTRUIT)/$(SQLITEDB_NAME) : $(PRINCIPAL_DIR)/$(SQLITEDB_NAME)

$(DEPENDENTE_ALL) :
	cp $< $@

.PHONY : clean
clean :
	@cd $(PRINCIPAL_DIR); make clean
	@cd $(REINOITOR_DIR); make clean
