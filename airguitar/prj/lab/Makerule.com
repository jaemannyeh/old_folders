
COM_C_FLG_INC_DEF  = -I../../src/com/inc

COM_SRC_PATH       = ../../src/com/src

COM_OBJ_FILES      =

#######################################################################

COM_OBJ_FILES     += com_params.o
com_params.o : $(COM_SRC_PATH)/com_params.c
	$(CC) $(C_FLG_INC_DEF) $(COM_C_FLG_INC_DEF) -o $@ -c $<

#######################################################################

COM_DEP_FILES = $(patsubst %.o,%.d,$(COM_OBJ_FILES))

OBJ_FILES += \
	$(COM_OBJ_FILES)

DEP_FILES += \
	$(COM_DEP_FILES)

