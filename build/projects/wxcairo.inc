################################################################################
#
# FILENAME:
#    wxcairo.inc
# 
# FILE DESCRIPTION:
#    Makefile for the wxcairo project
#
################################################################################

# Output target for wxcairo
wxcairo-out :=$(binPrefix)wxcairo.a

wxcairo-src := $(srcPrefix)wx/speedometer.cpp \
               $(srcPrefix)wx/trend_plot.cpp 

ifeq ($(platform),linux)
else
endif

# Build the list of wxcairo object files
wxcairo-obj := $(call gen_objs,wxcairo)

# Update the dependency list to include any wxcairo related object files
objects +=$(wxcairo-obj)

$(wxcairo-obj):$(call gen_cpp_file,wxcairo,$@)
	@echo ""
	@echo "wxCairo: Compiling '$(call gen_cpp_file,wxcairo,$(@))' to '$@'"
	@echo "-----------------------------------------------------"
	@echo ""
	@$(make-parent-dir)
	$(CC) -fPIC -MD \
				  $(wxcairo-inc) \
				  $(wxcairo-defines) \
				  -c -o $@ \
				  $(call gen_cpp_file,wxcairo,$@)
	@echo ""

# Linker instructions for the wxcairo library target
$(wxcairo-out): $(wxcairo-obj)
	@echo ""
	@echo "wxCairo: Linking $(wxcairo-out), $@"
	@echo "-------------------------------"
	@echo ""
	@$(make-parent-dir)
#	$(LD) -r $(wxcairo-obj) -o $@ $(wxcairo-linker)
	@echo ""


# Build the wxcairo library target
wxcairo: $(wxcairo-out)
	@echo "$(wxcairo-out)"
	@echo "wxCairo library BUILT"
	@echo ""


# Clean the wxcairo library target
clean-wxcairo:
	@rm -rf $(objPrefix)wxcairo $(wxcairo-out)

.PHONY: wxcairo clean-wxcairo
