Mac OS X 10.6 x86-64

1. Prerequisites

XCode (tested with 3.2.1)
It is on the system install disk, or can be downloaded from
 http://developer.apple.com/technology/xcode.html

QT (tested with qt-mac-opensource-4.6.2)
It can be downloaded from ftp://ftp.troll.no/qt/source/
Use qt-mac-opensource-*.dmg
(this is for the carbon API, and is the default)

CMake (tested with 2.8.0)
It can be downloaded from http://www.cmake.org/cmake/resources/software.html
Use cmake-2.8.0-Darwin-universal.dmg
It can be installed by double clicking.

2. Dependencies

1.1 Build the EW library

Extract the source in a directory and open a console. Navigate to the directory.
Use:

cd lib
make

It creates  "libew.a" in the lib folder.

2.2 Build the Qwt library (tested with 5.2.0)

It can be downloaded from http://sourceforge.net/projects/qwt/files/
Extract the archive.
Edit the file 'qwtconfig.pri'.
Add the line "CXXFLAGS= -arch i386" and comment this line (add a # in front of the line)

CONFIG 		+= QwtDll 

Save the changes.
Extract and open a console.  Navigate to the directory.

qmake -spec macx-g++
make

It creates the "libqwt.a" in the lib folder.

2.3 Build the QwtPlot3D library (tested with 0.2.7)

It can be dowmloaded from http://qwtplot3d.sourceforge.net/web/navigation/download_frame.html
Extract the archive.
Edit the file 'qwtplo3d.pro'.
Add the line "CXXFLAGS= -arch i386" in it.
Save the changes.
Extract and open a console.  Navigate to the directory.

qmake -spec macx-g++
make

It creates the "libqwtplot3d.a" in the lib folder.

2.4 Install R package (tested with  R 2.10.1)

It can be downloaded from http://cran.r-project.org/bin/macosx/ (R-2.10.1.pkg)
It can be installed by double clicking.

2.5 OpenSceneGraph (tested with 2.9.7)

It can be downloaded from http://blog.openscenegraph.org/?p=205
Use the source archive and extract it.
Open the console and navigate to the directory.
Use:

./configure
make
sudo make install

2.6 Build the DCMTK library (tested with 3.5.4)

It can be downloaded from http://dicom.offis.de/dcmtk.php.en
Download the source code and extract the archive.
Open the console and navigate to the directory.
Use 

./configure

Go to the config directory edit "Makefile.def"
Add the line:
ARCH = -arch i386
Change this line:
GLOBALDEFS = -DHAVE_CONFIG_H
to 
GLOBALDEFS = -DHAVE_CONFIG_H -D_DARWIN_C_SOURCE

In the console, type:

sudo make install-libs

2.7 Building the GMM library.

Open the console and navigate to the directory.
Use (given that EW directory is present at ../EW path):

cd src

qmake -project -t lib -o GMM.pro

qmake GMM.pro \
	-spec macx-g++ \
	"QT -= gui core" \
	"INCLUDEPATH += ../inc ../../EW/include" \
	"CONFIG += staticlib static" \
	"CXXFLAGS += -arch i386" \
	"LIBS += -L../../EW/lib -lew -llapack -xml2 -lblas"
	
It creates a libGMM.a in the lib directory.

3. Building the software

Checkout the latest version of the software 
In command line, use:

svn checkout svn://svn.secretlair.co.uk/evan/VPN/Evan
mkdir Dependencies

copy the libqwt.a, libGMM.a, libqwtplot3d.a, libew.a in the Dependencies folder.
Go to Evan/inc directory and remove DCMTK directory.

On the console use:

cd Evan
qmake -project

Edit Evan.project
Remove all the INCLUDEPATH references
copy/paste inside Evan.pro:

QMAKE_CXXFLAGS += -arch i386 -D_MACOS 
DEFINES += ENCRYPT_EVAN_LICENSE_
INCLUDEPATH += . "/usr/local/include" "/usr/local/qwt-5.2.0/include" "inc" "inc/GMM" "/opt/local/lib/R/include" "inc/Qwt3D" "/usr/local/dicom/include" "/usr/local/dicom/include/dcmtk" "inc/include"
INCLUDEPATH += "/opt/local/libexec/qt4-mac/include/QtCore" "/opt/local/libexec/qt4-mac/include/QtGui" 
LIBS += -mmacosx-version-min=10.5 -L"/usr/local/lib" -L"../Dependecies" -L"usr/lib" -L"/Library/Frameworks/R.framework/Versions/2.10/Resources/lib/" -L"/usr/local/dicom/lib" -lOpenThreads -losg -losgManipulator -losgViewer \
-losgDB -losgGA -losgUtil -losgVolume -lqwt -lGMM -lR -framework vecLib -framework Carbon -framework Cocoa -framework IOkit -framework CoreFoundation -llapack -lew -lxml2 -lqwtplot3d -ldcmdata -lofstd -ldcmimgle -lz
QT += svg xml opengl


In command line use:

qmake -spec macx-g++
make

It creates a Evan.app. Open it to run the Evan toolkit





	















