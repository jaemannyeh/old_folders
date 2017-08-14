
C_FLG_DEF_INC	+= -c
C_FLG_DEF_INC	+= -Wall
C_FLG_DEF_INC	+= -g

C_FLG_DEF_INC	+= -I$(PRJ_DIR)/oe_common/inc

OBJS	:= $(addprefix $(PRJ_OBJ_DIR)/,$(OBJECTS))

default: $(OBJS)

$(PRJ_OBJ_DIR)/%.o: %.c
	gcc $(C_FLG_DEF_INC) $(ADDED_C_FLG_DEF_INC) -o $@ $<

$(PRJ_OBJ_DIR)/%.o: %.cpp
	gcc $(C_FLG_DEF_INC) $(ADDED_C_FLG_DEF_INC) -o $@ $<
