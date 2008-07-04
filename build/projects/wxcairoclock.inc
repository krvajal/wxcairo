################################################################################
#
# FILENAME:
#    wxcairoclock.inc
# 
# FILE DESCRIPTION:
#    Makefile for the wxcairoclock demo program
#
################################################################################


# Output target for the wxcairoclock demo program
wxcairoclock-out :=$(binPrefix)wxcairoclock

wxcairoclock-src := $(srcPrefix)examples/wxcairoclock/main.cpp \
                    $(srcPrefix)wx/wxcairoclock.cpp

wxcairoclock-inc := $(wxcairo-inc) \
             -I $(srcPrefix)/examples/wxcairowxcairoclock

wxcairoclock-obj := $(call gen_objs,wxcairoclock)

objects +=$(wxcairoclock-obj)

# Compile instructions for the wxcairoclock source files
#
$(wxcairoclock-obj):$(call gen_cpp_file,wxcairoclock,$(@))
	@echo ""
	@echo "Clock: Compiling '$(call gen_cpp_file,wxcairoclock,$(@))' to '$@'"
	@echo "-----------------------------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ -MD $(wxcairo-defines) \
		$(wxcairoclock-inc) -c -o $@ $(call gen_cpp_file,wxcairoclock,$@)
	@echo ""

# Linker instructions for the wxcairoclock target
$(wxcairoclock-out): $(wxcairoclock-obj) $(wxcairo-out)
	@echo ""
	@echo "Clock: Linking $(wxcairoclock-src) $(wxcairoclock-out), $@"
	@echo "-------------------------------"
	@echo ""
	@$(make-parent-dir)
	g++ $(wxcairoclock-obj) $(wxcairo-out) -o $@ $(wxcairo-linker)
	$(call gen_app_file,$@,wxcairoclock)
	@echo ""


# Build the wxcairoclock target
wxcairoclock: $(wxcairoclock-out)
	@echo "$(wxcairoclock-out)"
	@echo "wxcairoclock example BUILT"
	@echo ""


# Clean the wxcairoclock target
clean-wxcairoclock:
	@rm -rf $(objPrefix)wxcairoclock $(wxcairoclock-out)

.PHONY: wxcairoclock clean-wxcairoclock
