This is the source distribution for the EW library.

Copyright information is contained in the file ./copyright.txt.
Build instructions are in the file ./readme.txt.
Notes about building and using on supported platforms are in the
files ./notes_linux.txt, ./notes_macosx.txt and ./notes_windows.txt.

The library public headers are in the directory ./include/ew.
The library archive is built at ./src/libew.a.

The library API documentation is in the directory ./doc/html.
Examples of various file formats are in the directory ./examples.

Build instructions:
    1. cd lib
    2. Edit, if necessary, ./makefile.
       No changed should be necessary in a standard environment.
       On Windows, if the necessary 3rd party libraries are not installed
       alongside the source distribution, their location will have to be
       provided in the makefile.
       See the comments in the makefile for details.
    3. make

The distribution contains a test suite.
Test instructions:
    1. Build libew.a.
    2. cd tests
    2. Edit, if necessary, the ./makefile.
    3. make
    4. make test
This will build and run all the tests.
