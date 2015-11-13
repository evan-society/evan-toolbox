Getting Started
===============
The source code for the EVAN Toolkit (ET) is located in `./VPN/Evan`. The `Edgewarp` source code is found in `./VPN/Evan/EW`

Dependencies:
- CMake v2.8 or higher for building the toolbox [https://cmake.org/]*
- Qt4 tested with Qt v4.8.6
- Qwt-q4 library for 2D Plots [http://qwt.sourceforge.net/]
- Qwt3D-q4 library for 3D Plots [http://qwtplot3d.sourceforge.net/]
- OpenSceneGraph v2.0.0 or higher [http://www.openscenegraph.org/]
- LAPACK for building EW and GMM
* cmake-gui recommended for novice users


Steps
=================
Summary: Create a directory named build under the main EVAN toolkit folder, change to this build directory and run cmake .., then make install

Linux/MacOS instructions:
```
mkdir build
cd build
cmake ..
make install
```
If all goes well an executable named ET will be built under `./bin`, and libraries for EW and GMM will be located under `./lib`
If cmake produces errors about any not found libraries, you can use cmake-gui under build to set these variables to the location of those libraries in your system.

