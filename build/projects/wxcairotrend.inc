################################################################################
#
# FILENAME:
#    wxcairotrend.inc
# 
# FILE DESCRIPTION:
#    Makefile for the wxcairotrend demo program
#
################################################################################


# Output target for the wxcairotrend demo program
wxcairotrend-out :=$(binPrefix)wxcairotrend

wxcairotrend-src := $(srcPrefix)examples/wxtrendplot/main.cpp

wxcairotrend-inc := $(wxcairo-inc) \
             -I $(srcPrefix)/examples/wxtrendplot

wxcairotrend-obj := $(call gen_objs,wxcairotrend)

objects +=$(wxcairotrend-obj)

# Compile instructions for the wxcairotrend source files
#
$(wxcairotrend-obj):$(call gen_cpp_file,wxcairotrend,$(@))
	@echo ""
	@echo "Clock: Compiling '$(call gen_cpp_file,wxcairotrend,$(@))' to '$@'"
	@echo "-----------------------------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ -MD $(wxcairo-defines) \
		$(wxcairotrend-inc) -c -o $@ $(call gen_cpp_file,wxcairotrend,$@)
	@echo ""

# Linker instructions for the wxcairotrend target
$(wxcairotrend-out): $(wxcairotrend-obj) $(wxcairo-out)
	@echo ""
	@echo "Clock: Linking $(wxcairotrend-src) $(wxcairotrend-out), $@"
	@echo "-------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ $(wxcairotrend-obj) $(wxcairo-obj) -o $@ $(wxcairo-linker)
	$(call gen_app_file,$@,wxcairotrend)
	@echo ""


# Build the wxcairotrend target
wxcairotrend: $(wxcairotrend-out)
	@echo "$(wxcairotrend-out)"
	@echo "wxcairotrend example BUILT"
	@echo ""


# Clean the wxcairotrend target
clean-wxcairotrend:
	@rm -rf $(objPrefix)wxcairotrend $(wxcairotrend-out)

.PHONY: wxcairotrend clean-wxcairotrend
