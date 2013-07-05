# wdkg 2008-2010
# XXX call make -f *qmake to make specific components

# This makefile is designed for GNU make and g++.

# This doesn't automatically update makedepends.
# This won't automatically rebuild if the Makefile or any requirements are
# modified.
# This treats ../include/ew/*.h, ../src/*.h and ../src/libew.a as requirements.
# The default target "all" will immediately fail if there are missing
# requirements.

system := $(subst `uname`,Windows,$(shell echo `uname`))

# Qt headers use long long.
# XXX Don't use := for QMAKEFLAGS because later we need late evaluation.
# Use QTYPE so that out -O options come after the default ones.
# The default Qt (carbon version) on Max OS X requires 32 bit.

MAKEFLAGS += -R
CXX := g++
CPPFLAGS :=
CXXFLAGS_BASIC := -O3
CXXFLAGS_WARN := -Wall -Wno-uninitialized -Wno-return-type -Wno-parentheses
CXXFLAGS_WARN = -Wno-long-long
CXXFLAGS_LINK :=
QMAKE := qmake
#XXX
QMAKEFLAGS = -Wall -nocache "CONFIG += thread"
QMAKEFLAGS += "PRE_TARGETDEPS += testlowercase.o" "INCLUDES += testlowercase.h"
ifndef NODEBUG
  CXXFLAGS_BASIC += -g -ansi -pedantic-errors
  CXXFLAGS_WARN += -Werror -Wold-style-cast
  QMAKEFLAGS += "CONFIG -= debug_and_release" "CONFIG += debug"
  QTYPE=DEBUG
else
  QMAKEFLAGS += "CONFIG -= debug_and_release" "CONFIG += release"
  CXXFLAGS_LINK += -s
  QTYPE=RELEASE
endif
ifeq ($(system), Windows)
  CPPFLAGS += -DUNICODE
  CXXFLAGS_BASIC += -mthreads
  QMAKEFLAGS += -spec win32-g++
  RM := -del /q
  EXE := .exe
  SLASH := \\
else ifeq ($(system), Darwin)
  QMAKEFLAGS += -spec macx-g++ "CONFIG -= app_bundle"
  CXXFLAGS_BASIC += -arch i386
  ifndef NODEBUG
    CXXFLAGS_BASIC += -gdwarf-2
  endif
  RM := rm -f
  EXE :=
  SLASH := /
else
  QMAKEFLAGS += -spec $(shell echo $(system) | tr A-Z a-z)-g++
  CPPFLAGS += -D_REENTRANT
  RM := rm -f
  EXE :=
  SLASH := /
endif
CPPFLAGS += -DTESTDIR="\"$(abspath .)\""
CXXFLAGS := $(CXXFLAGS_BASIC) $(CXXFLAGS_WARN)
LDFLAGS := $(CXXFLAGS_BASIC) $(CXXFLAGS_WARN) $(CXXFLAGS_LINK)
QMAKEFLAGS += "QMAKE_CXXFLAGS_$(QTYPE) += $(CPPFLAGS) $(CXXFLAGS_BASIC)"
QMAKEFLAGS += "QMAKE_CXXFLAGS_WARN_ON += $(CXXFLAGS_WARN) -Wno-long-long"
QMAKEFLAGS += "QMAKE_LFLAGS_$(QTYPE) += $(LDFLAGS)"

# Use the distribution ew header files and ew library.
# Qmake takes care of finding qt's required headers and libraries including the
# opengl libraries.
# The ew library depends on opengl, lapack, and libxml2.
# The lapack library depends on blas and the equivalent of g2c or F77.
# The libxml2 library depends on iconv, zlib1, and, on Windows, wsocks32.
# By default, use dynamic libraries where standard, otherwise static ones.
# Assume the files are in standard locations, or their packages are installed
# in siblings of this source directory.
# Redefine LAPACK_L or XML_L, here, or in the make command, if this is not the
# case.

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),depends)
  BASE := $(abspath ../..)
  EW_H := ../include/ew/DataflowNetwork.h
  EW_L := ../src/libew.a
  ifneq ($(wildcard /usr/lib/liblapack.*),)
    ifeq ($(system), Darwin)
      LAPACK_L := -framework vecLib -llapack
    else
      LAPACK_L := -llapack
    endif
  else
    dir1 := $(dir $(firstword $(wildcard $(BASE)/*/liblapack.a)))
    dir2 := $(dir $(firstword $(wildcard $(BASE)/*/libblas.a)))
    dir3 := $(dir $(firstword $(wildcard $(BASE)/*/libF77.a)))
    LAPACK_L := $(dir1)/liblapack.a $(dir2)/libblas.a $(dir3)/libF77.a
  endif
  ifneq ($(wildcard /usr/lib/libxml2.*),)
    XML_L := -lxml2
  else
    dir1 := $(dir $(firstword $(wildcard $(BASE)/*/lib/libxml2.a)))/libxml2.a
    dir2 := $(dir $(firstword $(wildcard $(BASE)/*/lib/libiconv.a)))/libiconv.a
    dir3 := $(dir $(firstword $(wildcard $(BASE)/*/lib/libz.a)))/libz.a
    XML_L := $(dir1)/libxml2.a $(dir2)/libiconv.a $(dir3)/libz.a
    ifeq ($(system), Windows)
      XML_L += \
       $(dir $(firstword $(wildcard $(BASE)/*/lib/libwsock32.a)))/libwsock32.a
    endif
  endif
  REQUIRE :=  $(filter %.a,$(EW_L) $(LAPACK_L) $(XML_L)) $(EW_H)
  IDIRS := ../include ..
  CPPFLAGS += $(IDIRS:%=-I%)
  test_form3$(EXE): LDLIBS := $(EW_L) $(XML_L) -lm
  test_tps2$(EXE): LDLIBS := $(EW_L) $(LAPACK_L) -lm
  test_tps3$(EXE): LDLIBS := $(EW_L) $(LAPACK_L) -lm
  test_index3face$(EXE): LDLIBS := $(EW_L) -lm
  test_surface3$(EXE): LDLIBS := $(EW_L) -lm
  test_lmks3$(EXE): LDLIBS := $(EW_L) $(XML_L) -lm
  test_dig3$(EXE): LDLIBS := $(EW_L) $(LAPACK_L) $(XML_L) -lm
#XXX^
  filter_form3$(EXE): LDLIBS := $(EW_L) $(XML_L) -lm
  test_dataflow$(EXE): LDLIBS := $(EW_L) -lm
  test_geom3$(EXE): LDLIBS := $(EW_L) -lm
  test_string$(EXE): LDLIBS := $(EW_L) -lm
  test_transform2$(EXE): LDLIBS := $(EW_L) -lm
  test_transform3$(EXE): LDLIBS := $(EW_L) -lm
  filter_form3$(EXE): OBJS := filter_form3.o FilterForm3.o
  test_dataflow$(EXE): OBJS := test_dataflow.o TestDataflow.o DataflowTestNode.o
  test_dataflow$(EXE): OBJS += Test.o
  test_geom3$(EXE): OBJS := test_geom3.o TestGeom3.o Test.o
  test_string$(EXE): OBJS := test_string.o TestString.o Test.o
  test_transform2$(EXE): OBJS := test_transform2.o TestTransform2.o Test.o
  test_transform3$(EXE): OBJS := test_transform3.o TestTransform3.o Test.o
  QMAKEFLAGS += "INCLUDEPATH += $(IDIRS)" "LIBS += $(LDLIBS)"
endif
endif

HDRS := $(wildcard *.h)
PROS := $(wildcard *.pro)
SRCS := $(wildcard test_*.cpp filter_form.cpp [A-Z]*cpp)
OKS_T := test_dataflow.ok test_geom3.ok test_string.ok test_transform2.ok
OKS_T += test_transform3.ok
TESTS_T := $(OKS_T:.ok=$(EXE))
OKS_G :=
TESTS_G := $(OKS_G:.ok=$(EXE))
TESTS := $(TESTS_T) $(TESTS_G)
OTHERS := filter_form3
#XXX GUI .o's are handled by the qmake makefile.

# Order text tests before gui tests in targets "all" and "test".
# The target dependent variable $(OBJS) does not work for dependencies.

all: $(REQUIRE) $(TESTS) $(OTHERS)
test: all test_t test_g
test_t: $(OKS_T)
test_g: $(OKS_G)
filter_form3$(EXE): filter_form3.o FilterForm3.o
test_dataflow$(EXE): test_dataflow.o TestDataflow.o DataflowTestNode.o Test.o
test_geom3$(EXE): test_geom3.o TestGeom3.o Test.o
test_string$(EXE): test_string.o TestString.o Test.o
test_transform2$(EXE): test_transform2.o TestTransform2.o Test.o
test_transform3$(EXE): test_transform3.o TestTransform3.o Test.o
Makefile makedepends $(SRCS) $(HDRS) $(PROS) $(REQUIRE): ;
.SUFFIXES:
.SECONDARY:
.PHONY: all test_t test_g test clean depends
-include makedepends

%.qmake : %.pro
	$(QMAKE) $(QMAKEFLAGS) -o $@ $<

#XXXX
%$(EXE) : %.qmake %.cpp %.h
	$(MAKE) -f $<

%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TESTS_T) $(OTHERS):
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.ok : %$(EXE)
	.$(SLASH)$<

clean:
	$(RM) $(TESTS) $(OTHERS) *.o core *.out *.ok moc_* qrc_* *.qmake *.app

depends:
	g++ -I../include -I.. -MM -DTESTDIR $(SRCS) > makedepends
