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
The document '.\GettingStarted\TDM-GCC32bit.odt' gives a guided tour through the installation process. 
Following the advice at 
http://tdm-gcc.tdragon.net/quirks
the SJLJ exception unwinder format was chosen for better compatibility. Make sure not to mix libraries generated with the same compiler, but the other DW2 exception unwinder format! Of course, you can recreate the project and all libraries with the DW2 setting any time, but this process takes its time ;-)

Code::Blocks 12.11 is currently used as the project's IDE, but any more recent release should work, too.


Recreating the ET Makefile
==========================
This is a modified excerpt from '.\VPN\Evan\README.txt':
---------------------
1- Open a command prompt and navigate to the source directory.
2- Run: qmake -project
3- Edit the Evan.pro file
 // the original README file offers some advice that should not be necessary...
4- Run: qmake Evan.pro
5- Run: make [Debug/Release]
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