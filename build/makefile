##################################################
#
# FILENAME:
#    Makefile
# 
# FILE DESCRIPTION:
#    wxCairo build environment
#
##################################################
include macros.inc

# Include the projects in the right build order or there may
# be make dependency issues
include projects/wxcairo.inc
include projects/wxcairoclock.inc
include projects/wxcairospeedometer.inc
include projects/wxcairotrend.inc

# Add the targets that you want to build. This will
# restrict the $(MAKE)file to build only the targets that
# you specify
targets := wxcairo wxcairoclock wxcairospeedometer wxcairotrend

all: $(targets)
	@echo ""
	@echo "BUILD COMPLETE"
	@echo ""

# Clean the entire build output (but don't remove directories)
clean:
	@echo ""
	@echo "CLEANING BUILD OUTPUT"
	@echo "---------------------"
	@echo ""
	@rm -rf $(buildPrefix)/*
	@echo ""
	@echo "BUILD CLEANED"
	@echo ""

full-clean: fullclean

fullclean:
	@rm -rf $(buildPrefix)

.PHONY: targets
.PHONY: objects

# Setup the dependencies for each project
deps := $(patsubst %.o,%.d,$(sort $(objects)))
-include $(deps)
