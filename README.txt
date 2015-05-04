Getting Started
===============
The source code for the EVAN Toolkit (ET) is located in '.\VPN\Evan'. The 'Templand' source code is found in '.\VPN\Evan\EW' Open the file '.\VPN\Evan\Evan.cbp' to bring up the Code::Blocks IDE for the project. Note that the project file simply uses a Makefile for building, which is created using qmake. See the section 'Recreating the Makefile' for more information.
As a general advice: make sure to run any qmake-related shell commands from a Qt-shell, which, among others should define these environment variables (just type 'set' in the shell to list all env vars):
QMAKESPEC=win32-g++
QTDIR=C:\Qt\4.8.5
For your convenience, there is a batch script, which opens a Qt-shell in the source folder '.\VPN\Evan\qt-shell.bat'. You might have to adjust it, when your Qt version changes.


Installing/Building Qt
======================
Please consult '.\GettingStarted\Qt-building\Qt-building-info.txt'.


Build Environment
=================
Currently the project is meant to be built with TDM-GCC 4.8 32bit - future versions of the compiler suite may be compatible, too, though... 
http://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/tdm-gcc-4.8.1-3.exe/download
Make sure to install fortran support as well (needed for LAPACK). Alternatively, after installation, grab 
http://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%204.8%20series/4.8.1-tdm-2%20SJLJ/gcc-4.8.1-tdm-2-fortran.zip
and unpack it "over" the TDM-installation dir (just make sure that the folder hierarchy of the zip file matches the one from the TDM-GCC installation).

The document '.\GettingStarted\TDM-GCC32bit.odt' gives a guided tour through the installation process. 
Following the advice at 
http://tdm-gcc.tdragon.net/quirks
the SJLJ exception unwinder format was chosen for better compatibility. Make sure not to mix libraries generated with the same compiler, but the other DW2 exception unwinder format! Of course, you can recreate the project and all libraries with the DW2 setting any time, but this process takes its time ;-)

Code::Blocks 12.11 is currently used as the project's IDE, but any more recent release should work, too.


Recreating the ET Makefile
==========================
This is a modified excerpt from '.\VPN\Evan\README.txt':
---------------------
1a - Open a qt-command prompt (you may want to use the convenience script GettingStarted\Qt-building\qt-4.8.5-shell.bat)
1b - navigate to the source directory (the directory that contains Evan.cpp).
2  - Run: qmake -project
     // UPDATE 26.Apr.2015: it should not be necessary to run this command, 
     // since Evan.pro should already be correctly set up => skip step 3 as well
3  - Edit the Evan.pro file
     // the original README file offers some advice that should not be necessary...
     // UPDATE 26.Apr.2015: i had to add the lines (just below the already-present line INCLUDEPATH += .)
     //                     make sure that the environment variable QTDIR is set correctly
     //                     ( In the qt-command prompt, type set, so for example E:\EVAN-related\EVAN\VPN\Evan>set
     //                       which will print all currently set environment variables, and search for QTDIR.
     //                       On my machine i have "QTDIR=C:\Qt\4.8.5" )
     //                     for more info about using qmake and configuring .pro files:
     //                       http://doc.qt.io/qt-4.8/qmake-advanced-usage.html
     //
     // INCLUDEPATH += GMM/include
     // INCLUDEPATH += EW/include
     // INCLUDEPATH += ../../external/qwtplot3d-0.2.7/include
     // INCLUDEPATH += ../../external/qwt-5.2/include
     // INCLUDEPATH += ../../external/OSG-3.0.1/include
     // INCLUDEPATH += ../../external/libiconv-1.14/include
     // INCLUDEPATH += ../../external/libxml2-2.9.1/include/libxml2
     // INCLUDEPATH += $$(QTDIR)/include
     // INCLUDEPATH += $$(QTDIR)/include/QtOpenGL
     // otherwise, in step 5, i get the following error for example:
     //     In file included from DataTypes\DataTypeFactory.cpp:2:0:
     //     DataTypes\MatrixD.h:5:20: fatal error: Matrix.h: No such file or directory
     //     #include <Matrix.h>
     //
     // further, remove all lines starting with EW/tests (or put "# " in front of them)
     //
     // NOTE: Evan.cbp has some of the additionally needed include directories set separately, 
     //       so in Code::Blocks, the project should build fine without those further adjustments
4  - Run: qmake Evan.pro
5  - Run: make [Debug|Release] or mingw32-make [Debug|Release], respectively (Release is the default if 
     // UPDATE 26.Apr.2015: use VPN\Evan\buildDebug.bat and VPN\Evan\buildRelease.bat, respectively,
     //                     since they also copy the generated exectable to the expected folder
     // the original README file does not tell you that you may edit '.\VPN\Evan\Makefile.Debug' and add this line
     // LFLAGS        =        -mthreads -Wl,-subsystem,console
     // for easy printf-debugging in case you don't want to use the Code::Blocks mingw-gdb interface or ET's logger class
     // [ using -Wl,-subsystem,windows prevents the program from entering a cmd-shell ]
--------------------


Creating an ET Installer
========================
Navigate to '.\VPN\InnoSetupScript' and open 'Evan.iss'. For the ET 1.71 installer, Inno Script Studio 2.1.0.20 was used. Consult the 'readme.txt' in the corresponding directory for installation instructions. When Inno Script Studio is installed, double-clicking on Evan.iss starts an IDE. Creating a new ET installer simply involves pressing the button 'Compile the current Inno Setup Script'. The installer can be found in '.\VPN\InnoSetupScript\Output\EvanToolboxSetup.exe'.


External Libraries
==================
The 'external' folder contains prebuilt libraries - folder names reflect the exact version used. Note that further updating some libraries to more recent versions would lead to incompatibilities with the current system, so maybe it's best to more or less stick with 'outdated' versions of some libraries. Most notably, updating Qt 4 to Qt 5 would mean lots of rewriting.
In the folder '.\GettingStarted\3rdPartyLibs-building' is a collection of several text files with some instructions on how to build all the libraries that ET depends on, from scratch. This is not a definitive guide and some information might be inaccurate - view it merely as a loose collection of 'log' files that were put together while rebuilding all the libraries ;-)
Note that currently ET is built without R support.