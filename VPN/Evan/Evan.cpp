#include <QApplication>
#include <QString>
#include <QDir>
#include "MainWindow/MainWindow.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

// global variables
ofstream dbgfile; //RP 29/12/08 Debug file for use throughout toolbox
/*extern int IsRegistered;
extern bool IsRunning;
extern QString workingDirectory;*/
#include "Utilities/Settings.h" //YN 9Nov2015:extern globals are not clean, and are generating link errors with cmake

// File locations for unix (wdkg).
// Definition of USE_UNIX_LOCATIONS is not supported on Microsoft Windows, due
// to ascii/utf-8 issues.
#ifdef USE_UNIX_LOCATIONS
QString libpath;
QString licensepath;
#endif

int main(int argc, char *argv[])
{
//    dbgfile.open ("debug.txt"); //RP 29/12/08 Debug for use throughout toolbox
//	dbgfile << "Debug file for VPN" << endl;

#ifdef USE_UNIX_LOCATIONS
// Default to the original behaviour.
    libpath = "";
    licensepath = "license.dat";
    QString test;
    const char *lpath = getenv("EVAN_LIB");
    if (lpath == 0) {
      lpath = "/usr/lib/evan";
    }
    test = QString::fromUtf8(lpath);
    QDir qd(test);
    if (qd.exists()) {
      if (test != "" && test[test.length() - 1] != '/') {
        test += "/";
      }
      libpath = test;
    }
    const char *evan_license = getenv("EVAN_LICENSE");
    if(evan_license != NULL) {
      licensepath = QString::fromUtf8(evan_license);
    } else {
      const char *hpath = getenv("HOME");
      if (hpath != 0) {
        test = QString::fromUtf8(hpath);
        QDir qdh(test);
        if (qdh.exists()) {
          if (test != "" && test[test.length() - 1] != '/') {
            test += "/";
          }
          test += ".evanlicense";
          licensepath = test;
        }
      }
    }
#endif

	QApplication app(argc, argv);
	MainWindow mainWindow;
	mainWindow.showMaximized();

    app.setApplicationName("Evan");

	return app.exec();
}
