# psAle - If Ale's board ever needed an app, then this would be IT!
# Copyright (c) 2012 by Victor ADĂSCĂLIȚEI at admin@tuscale.ro
# 
# This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. 
# To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ , read it from the 
# COPYING file that resides in the same directory as this file does or send a letter to 
# Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

include ./Makefile.defs

PRINCIPAL_DIR := ./principal
REINOITOR_DIR := ./reinoitor
DIR_CONSTRUIT := ./construit

TIPCONSTRUCTIE :=

all :
	cd $(PRINCIPAL_DIR); make $(TIPCONSTRUCTIE)
	cd $(REINOITOR_DIR); make $(TIPCONSTRUCTIE)
# copiem binarele în directorul rezultat
	mkdir -p $(DIR_CONSTRUIT)
	cp $(PRINCIPAL_DIR)/$(PRIN_BIN_NAME) $(DIR_CONSTRUIT)/$(PRIN_BIN_NAME)
	cp $(PRINCIPAL_DIR)/$(SQLITEDB_NAME) $(DIR_CONSTRUIT)/$(SQLITEDB_NAME)
	cp $(REINOITOR_DIR)/$(REIN_BIN_NAME) $(DIR_CONSTRUIT)/$(REIN_BIN_NAME)
	
win : TIPCONSTRUCTIE := win
win : all
	
clean :
	cd $(PRINCIPAL_DIR); make clean
	cd $(REINOITOR_DIR); make clean
