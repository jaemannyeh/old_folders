
################################################
#
################################################

PRJ_DIR	?= $(CURDIR)

SUB_DIRS	+= $(PRJ_DIR)/oe_common

PRJ_OBJ_DIR	:= $(PRJ_DIR)/obj

################################################
# Export variables to submakes
################################################

export PRJ_DIR
export PRJ_OBJ_DIR

################################################
# Usage
################################################

.PHONY: usage sub_dirs all rclean

usage:
	@echo ""
	@echo "Check makecfg.mk for configuration options"
	@echo ""
	@echo "Options"
	@echo "  PRJ_OBJ_DIR = $(PRJ_OBJ_DIR)"
	@echo "Targets"
	@echo "  all         # build all"
	@echo "  rclean      # remove all"
	@echo ""

$(PRJ_OBJ_DIR):
	@mkdir $(PRJ_OBJ_DIR)

sub_dirs: $(PRJ_OBJ_DIR)
	@echo "building"
	@@for sub_dir in $(SUB_DIRS); do \
	if [ -d $$sub_dir ]; then \
		make -C $$sub_dir || exit 1; \
	fi; \
	done

all: sub_dirs

rclean:
	@echo "removing"
	rm -rf $(PRJ_OBJ_DIR)

################################################
# End of Makefile
################################################
