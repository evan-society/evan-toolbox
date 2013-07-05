# wdkg 2008-2010

# This makefile is designed for GNU make and g++.

# This doesn't automatically update makedepends.
# This won't automatically rebuild if the makefile or if any requirements are
# modified.
# This won't automatically rebuild if source files are deleted.

# The default target "all" will immediately fail if there are missing
# requirements.

system := $(subst `uname`,Windows,$(shell echo `uname`))

outputLibName := libew.a

# Qt requires 32 bit on Mac OSX Carbon.

MAKEFLAGS += -R
CXX := g++

CXXFLAGS_BASIC := -O3
CXXFLAGS_WARN := -Wall -Wno-uninitialized -Wno-return-type -Wno-parentheses
AR := ar -r -s
ifndef NODEBUG
  CXXFLAGS_BASIC += -g -ansi -pedantic-errors
  CXXFLAGS_WARN += -Werror -Wold-style-cast
  outputLibName := libewd.a
endif

ifeq ($(system), Windows)
  CPPFLAGS += -DUNICODE
  CXXFLAGS_BASIC += -mthreads
  RM := -del /q
#else ifeq ($(system), Darwin)
#  CXXFLAGS_BASIC += -arch i386
#  ifndef NODEBUG
#    CXXFLAGS_BASIC += -gdwarf-2
#  endif
#  RM := rm -f
else
  CPPFLAGS += -D_REENTRANT
  RM := rm -f
endif
CXXFLAGS := $(CXXFLAGS_BASIC) $(CXXFLAGS_WARN)

# Need header files for OpenGL, libxml2, and, indirectly, iconv.
# Assume the files are in standard locations, or their packages are installed
# in siblings of the source directory.
# Redefine or override these variables if this is not the case.

# The documentation requires LIBICONV_STATIC and LIBXML_STATIC to be defined
# if using static versions of these libraries on windows.
# However, iconv.h is only included by a libxml2 header file for iconv_t, which
# doesn't depend on LIBICONV_STATIC.
# LIBXML_STATIC effects the definition of XMLPUBVAR.
# There is one variable used: xmlFree.
# It is possible that defining LIBXML_STATIC will work with a dll if the linker
# is configured to auto-import.

# Delete the *_STATIC definitions if dynamic libxml2 or iconv libraries are
# going to be used.

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),veryclean)
ifneq ($(MAKECMDGOALS),depends)
  BASE := $(abspath ../..)
ifeq ($(system), Windows)
#-I"../external/include" -I"C:/mingw/include"
	GL_I := "C:/mingw/include"
	ICONV_I := "../external/include"
	XML_I := "../external/include"
	MINGW_I := "C:/mingw/include"
	CPPFLAGS += -DLIBICONV_STATIC -DLIBXML_STATIC -U__STRICT_ANSI__
else  
  GL_I := $(patsubst %/GL/gl.h,%,$(firstword $(wildcard \
   /usr/include/GL/gl.h /usr/X11/include/GL/gl.h $(BASE)/*/include/GL/gl.h)))
	
  ICONV_I := $(patsubst %/iconv.h,%,$(firstword $(wildcard \
   /usr/include/iconv.h $(BASE)/*/include/iconv.h)))
   
 # XML_I := $(patsubst %/libxml/tree.h,%,$(firstword $(wildcard \
 #  /usr/include/libxml2/libxml/tree.h $(BASE)/*/include/libxml/tree.h)))
 XML_I := C:\svn\libxml2-2.6.30.win32\include
  MINGW_I := $(patsubst %/stdio.h,%,$(firstword $(wildcard \
   $(BASE)/*/include/stdio.h)))
endif  
  REQUIRE := $(GL_I)/GL/gl.h $(ICONV_I)/iconv.h $(XML_I)/libxml/tree.h
  IDIRS := ../include $(sort $(filter-out /usr/include $(MINGW_I), \
   $(GL_I) $(ICONV_I) $(XML_I)))
  CPPFLAGS += $(IDIRS:%=-I%)
endif
endif
endif

SRCS := $(wildcard *.cpp)
HDRS := $(wildcard *.h) $(wildcard ../include/ew/*.h)
OBJS := $(SRCS:.cpp=.o)

#all: $(REQUIRE) $(outputLibName)
all: $(outputLibName)
$(outputLibName): $(OBJS)
Makefile makedepends $(SRCS) $(HDRS): ;
.SUFFIXES:
.SECONDARY:
.PHONY: all clean depends
-include makedepends

%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

%.a:
	$(RM) $@
	$(AR) $@ $(OBJS)

clean:
	$(RM) *.o *.a
	
depends:
	g++ -I../include -MM *.cpp > makedepends
