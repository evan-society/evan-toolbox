# wdkg 2008-2010

# This makefile is designed for GNU make on systems other than Windows.

MAKEFLAGS += -R
NAME := "EW Library"
REFMAN := EWReferenceManual.pdf
SRCS := $(wildcard *.doxygen) $(wildcard ../examples/*) \
 $(wildcard ../include/ew/*) \
 $(wildcard ../src/*.cpp)
VERSION := $(shell cat ../version.txt)

all: html pdf
html: html.stamp
pdf: pdf.stamp
html.stamp: $(SRCS)
pdf.stamp: html.stamp
Makefile $(SRCS): ;
.SUFFIXES:
.SECONDARY:
.PHONY: all html pdf clean

html.stamp:
	rm -rf html man man3 latex pdf
	(cat Doxyfile ; \
	 echo 'PROJECT_NAME = $(NAME)' ; \
	 echo 'PROJECT_NUMBER = $(VERSION)' ) | \
	 doxygen - 2>&1 | egrep -v '^(format:|./usr/share/texmf-texlive)'
	mv man3 man
	rm man/[!e]* man/e[!w]*
	./fix_doxygen_man.sh $(NAME) $(VERSION) man/*
	touch html.stamp

pdf.stamp:
	$(MAKE) -C latex
	mkdir pdf
	mv latex/refman.pdf pdf/$(REFMAN)
	$(MAKE) -C latex clean
	touch pdf.stamp

clean:
	rm -rf html man man3 latex pdf html.stamp pdf.stamp
