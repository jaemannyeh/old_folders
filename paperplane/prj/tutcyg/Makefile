
PP_HOME		= ../..
PP_LIB_PATH	= $(PP_HOME)/lib

PP_CC		= gcc
PP_LD		= g++
PP_AR		= ar
PP_NM		= nm

PP_C_FLG	= -MD -MP -Wall
PP_C_INC	= -I. -I$(PP_HOME)/inc
PP_C_DEF	=

PP_LD_FLG	=

default : build_all

####

PRJ_NAME	= tutcyg

include ./Makerule.pp_base
include ./Makerule.pp_class
include ./Makerule.pp_draft

####

PRJ_LIB_FILE	= lib$(PRJ_NAME).a
PRJ_MAIN_NAME	= $(PRJ_NAME)
PRJ_EXE_FILE	= $(PRJ_MAIN_NAME).exe
PRJ_NM_FILE	= $(PRJ_MAIN_NAME).nm

DEPENDENCY_FILES := $(DEP_FILES)
-include $(DEPENDENCY_FILES)

$(PRJ_MAIN_NAME).o: $(PRJ_MAIN_NAME).c
	$(PP_CC) $(PP_C_FLG) $(PP_C_INC) $(PP_C_DEF) -o $@ -c $<

$(PRJ_LIB_FILE) : $(OBJ_FILES)
	$(PP_AR) crus $(PP_LIB_PATH)/$@ $(OBJ_FILES)

$(PRJ_EXE_FILE) : $(PRJ_MAIN_NAME).o $(PRJ_LIB_FILE)
	$(PP_LD) -o $@ $< $(PP_LIB_PATH)/$(PRJ_LIB_FILE) $(PP_LD_FLG)
	$(PP_NM) -g $@ > $(PRJ_NM_FILE)

build_all : $(PRJ_EXE_FILE)

clean :
	rm -rf *.d
	rm -rf *.o
	rm -rf *.nm
	rm -rf *.db
	rm -rf *~
	rm -rf $(PRJ_EXE_FILE)
	rm -rf $(PP_LIB_PATH)/$(PRJ_LIB_FILE)
	rm -rf *.yy.c *.tab.c *.tab.h
	
	
	

