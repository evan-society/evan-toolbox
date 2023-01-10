![Logo](https://user-images.githubusercontent.com/696719/211114994-0b5d63f0-8d21-4661-ba32-e9208887fb57.png)

# ET Open Space
The EVAN Toolbox (ET) is a software package developed by the European Virtual Anthropology Network – EVAN (www.evan.at) and the EVAN-Society to facilitate form and shape analysis of objects featuring a complex geometry. It uses Geometric Morphometrics (GM) which includes methods such as General Procrustes Analysis, Principal Component Analysis, Thin-Plate Spline Warping or Partial Least Squares Analysis. The software supports data acquisition, i.e. to locate landmarks and semilandmarks on curves and surfaces.

The idea behind the EVAN Toolbox is to make your life easier if your task is to analyse 3D shape and form of complexly shaped objects as we find them, for instance, in biology and medicine. ET is particularly focused on the advanced visualisation of shape and form comparisons. ET interacts with its users in the form of a programmable network in which the operator specifies the flow of information and control among various components, called nodes.

The EVAN Toolbox is intended for research and education purposes. If you use ET for your work, please don’t forget to acknowledge the EVAN Toolbox in the appropriate section of your publication.

## Dependencies
ET requires the following libraries. It was tested with the versions listed below, but newer versions may also work if backward compatible.
- CMake>=3.0.2
- lapack==3.4.0
- libiconv==1.14
- libxml2==2.9.1
- OpenSceneGraph==3.4.1
- Qt==4.8.7
- qwt==5.2.3
- qwtplot3d==0.2.6
- SDL==1.2.15

## Building from source
Cmake is used to configure the compilation and linking process for ET. It was previously built and tested on Windows (with `TDM-GCC-32`)  and Linux (with `gcc`) operating systems and should also work on MacOS systems if the dependencies are available and built correctly. Example build steps are the following (after installing/building dependencies):
```
git clone https://github.com/evan-society/evan-toolbox.git
cd evan-toolbox
mkdir build
cd build && cmake-gui ..
```
After setting the dpendency paths in CMake GUI or the command line and generating a Makefile (or equivalent), a binary can be generated with
```
make install
```
If all goes well, A binary called `ET` will be created under the top directory, or under the specified `CMAKE_INSTALL_PREFIX`.

## Pre-built Binaries
A pre-built installer for Windows is available from the official [Evan-Society webpage](https://www.evan-society.org/support/download-evan-toolbox/)

## License
The Evan Toolbox has a GPL license, as found in the [LICENSE](LICENSE) file.
