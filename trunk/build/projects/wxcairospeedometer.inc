################################################################################
#
# FILENAME:
#    wxcairospeedometer.inc
# 
# FILE DESCRIPTION:
#    Makefile for the wxcairospeedometer demo program
#
################################################################################


# Output target for the wxcairospeedometer demo program
wxcairospeedometer-out :=$(binPrefix)wxcairospeedometer

wxcairospeedometer-src := $(srcPrefix)examples/wxspeedometer/main.cpp

wxcairospeedometer-inc := $(wxcairo-inc) \
             -I $(srcPrefix)/examples/wxspeedometer

wxcairospeedometer-obj := $(call gen_objs,wxcairospeedometer)

objects +=$(wxcairospeedometer-obj)

# Compile instructions for the wxcairospeedometer source files
#
$(wxcairospeedometer-obj):$(call gen_cpp_file,wxcairospeedometer,$(@))
	@echo ""
	@echo "Clock: Compiling '$(call gen_cpp_file,wxcairospeedometer,$(@))' to '$@'"
	@echo "-----------------------------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ -MD $(wxcairo-defines) \
		$(wxcairospeedometer-inc) -c -o $@ $(call gen_cpp_file,wxcairospeedometer,$@)
	@echo ""

# Linker instructions for the wxcairospeedometer target
$(wxcairospeedometer-out): $(wxcairospeedometer-obj) $(wxcairo-out)
	@echo ""
	@echo "Clock: Linking $(wxcairospeedometer-src) $(wxcairospeedometer-out), $@"
	@echo "-------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ $(wxcairospeedometer-obj) $(wxcairo-out) -o $@ $(wxcairo-linker)
	$(call gen_app_file,$@,wxcairospeedometer)
	@echo ""


# Build the wxcairospeedometer target
wxcairospeedometer: $(wxcairospeedometer-out)
	@echo "$(wxcairospeedometer-out)"
	@echo "wxcairospeedometer example BUILT"
	@echo ""


# Clean the wxcairospeedometer target
clean-wxcairospeedometer:
	@rm -rf $(objPrefix)wxcairospeedometer $(wxcairospeedometer-out)

.PHONY: wxcairospeedometer clean-wxcairospeedometer
