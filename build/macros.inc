##################################################
#
# FILENAME:
#    macros.inc
# 
# PROJECT:
#    wxcairo
#
# FILE DESCRIPTION:
#    Macro definitions for the master makefile
#    for the wxcairo demos
#
# CREATED BY:
#    Brad Elliott (Jan 14, 2008)
##################################################

platform :=$(shell ./getos.pl)
arch     :=$(shell ./getarchitecture.pl)



all:

.SUFFIXES:


# The prefixes controlling the build environment
#     buildPrefix       = the output directory where build files will go
#     targetBuildPrefix = the release or debug subdirectory depending on what is being built
#     srcPrefix         = the base directory where source files are located
#     objPrefix         = the directory where object files will be stored as they are generated
#     binPrefix         = the output subdirectory where all generated executables/scripts will be stored
#     eventPrefix       = a directory used to generate events used for synchronization of the makefile
#     projectsPrefix    = the location of the individual project makefiles for each of the Xtensa sub-directories
buildPrefix ?= ../build-output

ifeq ($(RELEASE),yes)
  targetBuildPrefix ?= $(buildPrefix)/production/
  override TARGET =production
else
  ifneq ($(TARGET),)
    targetBuildPrefix ?= $(buildPrefix)/$(TARGET)/
    
    ifeq ($(TARGET),production)
       override RELEASE =yes
    endif
  else
    targetBuildPrefix ?= $(buildPrefix)/production/
    override TARGET=production
  endif
endif

CC            := g++
LD            := g++

override DEFINES += $(SWITCHES)

curdir         ?= $(CURDIR:%=%/)
curdirPrefix   ?= $(CURDIR:%=%/)
srcPrefix      ?= ../
objPrefix      ?= $(targetBuildPrefix)obj/
binPrefix      ?= $(targetBuildPrefix)bin/
eventPrefix    ?= $(targetBuildPrefix)events/
projectsPrefix ?= projects/

ifeq ($(arch),64)
    stdlibPrefix       ?= lib64
else
    stdlibPrefix       ?= lib
endif


# Cairo paths
cairo_path          = ../../../3rdparty/cairo
cairo_include_paths = -I/usr/include/cairo


# WxWidgets paths
wxwidgets_path          = ../../../3rdparty/wxwidgets
wxwidgets_lib_paths     = -L$(wxwidgets_path)/compiles/$(platform)/lib


ifeq ($(platform),linux)
    wxwidgets-defines = \
            -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 \
            -I/usr/lib/i386-linux-gnu/glib-2.0/include \
            -I/usr/lib/i386-linux-gnu/gtk-2.0/include \
            -I/usr/lib/gtk-2.0/include \
            -I/usr/include/atk-1.0 \
            -I/usr/include/pango-1.0 \
            -I/usr/include/gdk-pixbuf-2.0 \
            -Wall -Wundef -Wno-ctor-dtor-privacy -O2 -fno-strict-aliasing -fno-common
            
else
    wxwidgets_include_paths = -I$(wxwidgets_path)/compiles/macos/lib/wx/include/mac-ansi-release-static-2.8 \
                              -I$(wxwidgets_path)/include \
                              -I$(wxwidgets_path)/src/mac/carbon/morefilex
    wxwidgets-defines = -D__WXMAC__ -DWX_PRECOMP -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -I/Developer/Headers/FlatCarbon
endif



# wxcairo library include paths
wxcairo-inc := -I $(srcPrefix) \
               -I . \
               `wx-config --cppflags` \
               $(cairo_include_paths)

ifeq ($(RELEASE),yes)
    wxcairo-defines := $(wxwidgets-defines) -DWXCAIRO -g -W -Werror -Wall -Wundef -Wno-ctor-dtor-privacy -fno-strict-aliasing -fno-common
else
    wxcairo-defines := $(wxwidgets-defines) -DWXCAIRO -DDEBUG -DWXCAIRO -g -W -Wall -Wundef -Wno-ctor-dtor-privacy -fno-strict-aliasing -fno-common
endif

ifeq ($(platform),linux)
    wxcairo-linker := `wx-config --libs` 
else
    wxcairo-linker := $(wxwidgets_lib_paths) \
                -lz -lpthread -liconv -lwx_mac-2.8 -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8 -lwxexpat-2.8 \
                -framework QuickTime -framework IOKit -framework Carbon -framework Cocoa -framework System \
                -lz -lpthread -liconv -framework WebKit \
                $(cairo_path)/src/.libs/libcairo.a
endif

# The list of object files used to generate the dependencies of individual
# source files.
objects :=


ifeq ($(SILENT),yes)
   .SILENT:
   silent-echo :=echo
else
   silent-echo :=true
endif

make-parent-dir =@-mkdir -p "$(@D)"
display-target =@$(silent-echo) "$@"

# XCC is dumb and can't construct the dependencies in the proper location
# Instead of outputting it in the same location as the object file it
# outputs it in the local directory. These two lines are called to correct
# the dependency file and move it to the right location
gen_dependencies = @-cat $(notdir $(basename $(@))).d | sed -e 's|$(notdir $(@))|$(@)|g' > $(dir $(@))$(notdir $(basename $(@))).d; \
                   rm -rf $(notdir $(basename $(@))).d

# Generate the list of object files from the list of input source files
gen_objs     = $(subst $(srcPrefix),$(objPrefix)$(1)/,$(addsuffix .o, $(basename $($(1)-src))))
gen_asm_objs = $(subst $(srcPrefix),$(objPrefix)$(1)/,$(addsuffix .o, $(basename $($(1)-asm-src))))

# The statement
#   $(subst $(objPrefix)sandbox/,$(srcPrefix),$(addsuffix .cpp, $(basename $(@))))
# is used to re-construct the appropriate .cpp file from the
# object file. When expanded this might create a target that looks like this:
#   one/one.o two/two.o: one/one.cpp
#      compile instructions for one.cpp
# for one.cpp and
#   one/one.o two/two.o: two/two.cpp
#      compile instructions for two.cpp
# for two.c
gen_cpp_file = $(subst $(objPrefix)$(1)/,$(srcPrefix),$(addsuffix .cpp, $(basename $(2))))

link_display = @echo "";echo "$1: Linking $(2)";echo "-------------------------------";echo "";mkdir -p "$(@D)"


ifeq ($(platform),linux)
    gen_app_file =
else
    gen_app_file = /Developer/Tools/Rez -d __DARWIN__ -t APPL -d __WXMAC__ -i $(wxwidgets_path)/include -o $(1) Carbon.r graphics/main.r; \
	/Developer/Tools/SetFile -a C $(1); \
	mkdir -p $(1).app/Contents; \
	mkdir -p $(1).app/Contents/MacOS; \
	mkdir -p $(1).app/Contents/Resources; \
	sed -e "s/IDENTIFIER/`echo . | sed -e 's,\.\./,,g' | sed -e 's,/,.,g'`/" \
   -e "s/EXECUTABLE/$(2)/" \
   -e "s/VERSION/2.8.4/" \
   $(wxwidgets_path)/src/mac/carbon/Info.plist.in >$(1).app/Contents/Info.plist; \
	echo -n "APPL????" >$(1).app/Contents/PkgInfo; \
	ln -f $(1) $(1).app/Contents/MacOS/$(2); \
	cp -f $(wxwidgets_path)/src/mac/carbon/wxmac.icns $(1).app/Contents/Resources/wxmac.icns
endif   
