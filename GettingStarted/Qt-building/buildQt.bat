REM if not building in qt-src directory, copy this script to the
REM new directory and call 'configure.exe' with its absolute path, e.g.,
REM instead of calling 
REM 'configure.exe'
REM call 
REM 'G:\qt\qt-everywhere-opensource-src-4.8.5\configure.exe'
REM in the first line of the configure command below

configure.exe  ^
-platform win32-g++ -opensource -confirm-license ^
-debug -debug-and-release ^
-no-phonon  ^
-no-phonon-backend  ^
-no-webkit  ^
-fast  ^
-no-dsp  ^
-no-vcproj  ^
-no-cetest  ^
-no-s60  ^
-qt-sql-odbc -qt-sql-sqlite -plugin-sql-odbc -plugin-sql-sqlite  ^
-no-incredibuild-xge  ^
-no-script -no-scripttools -no-qt3support
