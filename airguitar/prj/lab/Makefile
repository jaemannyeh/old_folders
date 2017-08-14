
PRJ_HOME        = ../..
PRJ_LIB_PATH    = $(PRJ_HOME)/lib

######## ######## ######## ######## ######## ######## ######## ########

CC          = ccpentium
LD          = ldpentium
NM          = nmpentium
AR          = arpentium
RM          = rm -f

######## ######## ######## ######## ######## ######## ######## ########

C_FLG       = -mtune=i486 -march=i486
C_FLG      += -ansi -O2 -nostdlib -Wall -MD -MP
C_FLG      += -fvolatile -fno-builtin -fno-defer-pop -fno-zero-initialized-in-bss
C_INC      += -I$(WIND_BASE)/target/config/all
C_INC      += -I$(WIND_BASE)/target/h
C_INC      += -I$(WIND_BASE)/target/h/wrn/coreip
C_INC      += -I.
C_INC      += -I../../..
C_DEF       = -DCPU=SIMLINUX
C_DEF      += -DTOOL_FAMILY=gnu -DTOOL=gnu -D_WRS_KERNEL -DIP_PORT_VXWORKS=68 -DINET
C_DEF      += -D_VSB_CONFIG_FILE=\"/opt/build_tools/WindRiver32ARM/vxworks-6.8/target/lib/h/config/vsbConfig.h\"  

C_FLG_INC_DEF = $(C_FLG) $(C_INC) $(C_DEF)

######## ######## ######## ######## ######## ######## ######## ########

default : build_all

######## ######## ######## ######## ######## ######## ######## ########

include ./Makerule.bse
include ./Makerule.com

######## ######## ######## ######## ######## ######## ######## ########

DEPENDENCY_FILES := $(DEP_FILES)
-include $(DEPENDENCY_FILES)

######## ######## ######## ######## ######## ######## ######## ########

PRJ_NAME        = lab
PRJ_LIB_FILE    = lib$(PRJ_NAME).a
PRJ_KOBJ_FILE   = $(PRJ_NAME).ko
PRJ_MAIN_NAME   = $(PRJ_NAME)

######## ######## ######## ######## ######## ######## ######## ########

$(PRJ_MAIN_NAME).o : $(PRJ_MAIN_NAME).c
	$(CC) $(C_FLG_INC_DEF) -o $@ -c $<

$(PRJ_LIB_FILE) : $(OBJ_FILES)
	@$(RM) $(PRJ_LIB_PATH)/$@
	@echo
	$(AR) crus $(PRJ_LIB_PATH)/$@ $(OBJ_FILES)
	@echo

$(PRJ_KOBJ_FILE) : $(OBJ_FILES)
	@$(RM) $(PRJ_LIB_PATH)/$@
	$(NM) $(OBJ_FILES) | wtxtcl /opt/build_tools/WindRiver32ARM/vxworks-6.8/host/resource/hutils/tcl/munch.tcl -c pentium > ctdt.c
	@echo
	$(CC) $(C_FLG_INC_DEF) -fdollars-in-identifiers -o ctdt.o -c ctdt.c
	@echo
	$(LD) -r -nostdlib -X -i -T /opt/build_tools/WindRiver32ARM/vxworks-6.8/target/h/tool/gnu/ldscripts/link.OUT -o $(PRJ_LIB_PATH)/$@ $(OBJ_FILES) ctdt.o
	@echo

build_all : $(PRJ_LIB_FILE) $(PRJ_KOBJ_FILE) $(PRJ_MAIN_NAME).o

clean :
	$(RM) *.d
	$(RM) *.o
	$(RM) ctdt.c

