To Recompile the EvanToolkit source code:

To build the program from source you will first need to build the EW library and the GMM library.
The source code for these libraries should be available with the source for the EvanToolkit.
Instructions on how to build them are available with the source code for each library.

You will also need to download the binary files for the following libraries:
	Lapack
	Qwt
	QwtPlot3d
	OpenSceneGraph
	DCMTK

The EvanToolkit can also be linked with the R library
	
These libraries can be found on:
   svn://svn.secretlair.co.uk/evan/VPN/lib_win32
Rename this folder "lib" and put it inside the Evan folder.

1- Open a command prompt and navigate to the source directory.
2- Run: qmake -project
3- Edit the Evan.pro file

	For building a Debug version,add the following lines:
	INCLUDEPATH += . "inc/OpenSceneGraph"  "inc/Qwt" "inc/GMM" "inc/" "inc/Qwt3D" "inc/DCMTK"
	LIBS += -L"lib/OpenSceneGraph" -L"lib/Qwt" -L"lib/GMM" -L"lib/ew" -L"lib/Qwt3D" -L"lib/DCMTK" -llibosg -llibosgManipulator -llibosgViewer -llibosgDB -llibosgGA -llibosgUtil -llibosgVolume -lGMM -lew -lopengl32 -lgdtoa -llibxml2 -llibdcmdata -llibofstd -llibdcmimgle -lwsock32 -llapack
	QT += xml opengl svg
	RC_FILE = Evan.rc
	LIBS += -lqwtd -lqwtplot3dd

	For building a Release version,add the following lines:
	INCLUDEPATH += . "inc/OpenSceneGraph"  "inc/Qwt" "inc/GMM" "inc/" "inc/Qwt3D" "inc/DCMTK"
	LIBS += -L"lib/OpenSceneGraph" -L"lib/Qwt" -L"lib/GMM" -L"lib/ew" -L"lib/Qwt3D" -L"lib/DCMTK" -llibosg -llibosgManipulator -llibosgViewer -llibosgDB -llibosgGA -llibosgUtil -llibosgVolume -lGMM -lew -lopengl32 -lgdtoa -llibxml2 -llibdcmdata -llibofstd -llibdcmimgle -lwsock32 -llapack
	QT += xml opengl svg
	RC_FILE = Evan.rc
	LIBS += -lqwt -lqwtplot3d

	If you want to link against the R library:, add the following lines:
	INCLUDEPATH += "path/to/R/include" 
	LIBS +=  -L"path/to/R/lib" -lR
	DEFINES += USE_R_

4- Run: qmake Evan.pro
5- Run: make [Debug/Release]

Requires: Qt 4.x, MingW GCC 10.x(on Windows)
