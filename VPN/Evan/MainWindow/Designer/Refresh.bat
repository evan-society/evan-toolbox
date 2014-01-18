uic MainWindow.ui -o ui_MainWindow.h

REM only the single line above was meant to be executed in the latest SVN revision...
REM however, the next line is needed in order for the license text to be updated in the app
REM itself and not just in the installer dialogue
uic license.ui -o ui_license.h
REM uic register.ui -o ui_register.h
REM uic registerInstructions.ui -o ui_regInstruct.h