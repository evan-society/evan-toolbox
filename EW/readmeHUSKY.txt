for release:
D:\Users\phouska\Desktop\EW\src>make NODEBUG='1'

for debug:
D:\Users\phouska\Desktop\EW\src>make

check this output to see, which functions are in the library:
nm -A -l --demangle=gnu dEvan.exe > dEvanSymbols.txt

in affinic debugger:
* pause execution
* Source List -> Retrieve Source List -> Filter:"view3widget"
* select View3Widget.cpp, RightClick, OpenManually and navigate to <git-repo-root-folder>\EW\src then select the cpp file
* place breakpoint in editor and continue
* ... breakpoint will now be triggered!
