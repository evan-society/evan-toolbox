######################################################################
# Automatically generated by qmake (2.01a) ?ri. 30. mar. 14:38:31 2010
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

win32 {
        LIBS += -liphlpapi
      }

# Input
HEADERS += Generator.h LicenseKeyUtils.h sha1.h
FORMS += generator.ui
SOURCES += Generator.cpp main.cpp sha1.cpp
