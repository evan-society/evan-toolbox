#!/bin/sh

echo ---- EXECUTING cmake call ----
echo ---- MAKE SURE TO RUN THIS FROM THE DESIRED OSG-CMAKE-BUILD DIRECTORY  ----
echo ---- IN ORDER NOT TO CLUTTER THE OSG-SRC DIRECTORY WITH CMAKE FILES ----
echo ---- USE ANOTHER DISTINCT INSTALL DIRECTORY ---
echo ---- i.e. do not use the OSG-git nor the cmake-build directory ----

# NOTE: relative path is OK, too!
# -DLIBXML2_INCLUDE_DIR="../libxml2-2.9.1/include"

# don't have the following ...
#  Set QT_QASSISTANTCLIENT_LIBRARY to: C:/Qt/4.6.2/lib/libQtAssistantClient4.a
# -DQT_QASSISTANTCLIENT_LIBRARY="${QT_PATH}/lib/libQtAssistantClient4.a"

#  Manually-specified variables were not used by the project:
#    FREETYPE_INCLUDE_DIR
#    GLUT_INCLUDE_DIR
#    GLUT_LIBRARY
#    GLUT_glut_LIBRARY
#    QT_MAIN_LIBRARY

# after adding "-DBUILD_OSG_EXAMPLES=1":
#  Manually-specified variables were not used by the project:
#    FREETYPE_INCLUDE_DIR
#    QT_MAIN_LIBRARY

THIRD_PARTY_PATH="/e/3rdParty-build"
QT_PATH="/e/qt/qt-4.8.5"

# this is needed, so that qmake.exe is properly recognized...
echo ---- PATH before Qt adjustments ----
echo ${PATH}
QTDIR="${QT_PATH}"
PATH="${QTDIR}/bin:${PATH}"
QMAKESPEC="${QTDIR}/mkspecs/win32-g++"
echo ---- PATH after Qt adjustments ----
echo ${PATH}
echo ---- QMAKESPEC is ----
echo ${QMAKESPEC}

echo ---- PWD is ----
echo ${PWD}

echo ---- OSG SOURCE is in directory ----
echo "$1"

# DOES NOT WORK! osg needs the missing <curl/types.h> which is no longer there in 7.33 => use 7.20 CURL_VER="curl-7.33.0"
CURL_VER="curl-7.20.1"
FFMPEG_VER="ffmpeg"
FREETYPE_VER="freetype-2.4.0"
GIF_VER="giflib-4.2.3"
GLUT_VER="freeglut-2.8.1"
JPEG_VER="jpeg-9"
XML2_VER="libxml2-2.9.1"
PNG_VER="libpng-1.4.12"
SDL_VER="SDL-1.2.15"
TIFF_VER="tiff-3.9.7"
ZLIB_VER="zlib-1.2.8"

OSG_VER="OSG-2.9.16"

# -DCMAKE_INCLUDE_PATH="${THIRD_PARTY_PATH}/${FREETYPE_VER}/include/freetype2"
 
# cmake /e/OSG-git/osg/ -G "MSYS Makefiles" \
cmake "$1" -G "MSYS Makefiles" \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX="${THIRD_PARTY_PATH}/${OSG_VER}" \
-DBUILD_OSG_EXAMPLES=1 \
\
-DCURL_INCLUDE_DIR="${THIRD_PARTY_PATH}/${CURL_VER}/include" \
-DCURL_LIBRARY="${THIRD_PARTY_PATH}/${CURL_VER}/lib/libcurl.dll.a" \
\
-DFFMPEG_ROOT="${THIRD_PARTY_PATH}/${FFMPEG_VER}" \
\
-DFREETYPE_INCLUDE_DIR="${THIRD_PARTY_PATH}/${FREETYPE_VER}/include" \
-DFREETYPE_INCLUDE_DIR_freetype2="${THIRD_PARTY_PATH}/${FREETYPE_VER}/include/freetype2" \
-DFREETYPE_INCLUDE_DIR_ft2build="${THIRD_PARTY_PATH}/${FREETYPE_VER}/include" \
-DFREETYPE_LIBRARY="${THIRD_PARTY_PATH}/${FREETYPE_VER}/lib/libfreetype.a" \
\
-DGIFLIB_INCLUDE_DIR="${THIRD_PARTY_PATH}/${GIF_VER}/include" \
-DGIFLIB_LIBRARY="${THIRD_PARTY_PATH}/${GIF_VER}/lib/libgif.a" \
\
-DGLUT_INCLUDE_DIR="${THIRD_PARTY_PATH}/${GLUT_VER}/include" \
-DGLUT_LIBRARY="${THIRD_PARTY_PATH}/${GLUT_VER}/lib/libglut.dll.a" \
-DGLUT_glut_LIBRARY="${THIRD_PARTY_PATH}/${GLUT_VER}/lib/libglut.dll.a" \
\
-DJPEG_INCLUDE_DIR="${THIRD_PARTY_PATH}/${JPEG_VER}/include" \
-DJPEG_LIBRARY="${THIRD_PARTY_PATH}/${JPEG_VER}/lib/libjpeg.a" \
\
-DLIBXML2_INCLUDE_DIR="${THIRD_PARTY_PATH}/${XML2_VER}/include/libxml2" \
-DLIBXML2_LIBRARIES="${THIRD_PARTY_PATH}/${XML2_VER}/lib/libxml2.a" \
\
-DPNG_PNG_INCLUDE_DIR="${THIRD_PARTY_PATH}/${PNG_VER}/include" \
-DPNG_LIBRARY="${THIRD_PARTY_PATH}/${PNG_VER}/lib/libpng.a" \
\
-DQT_QMAKE_EXECUTABLE="${QT_PATH}/bin/qmake.exe" \
-DQT_INCLUDE_DIR="${QT_PATH}/include" \
-DQT_MOC_EXECUTABLE="${QT_PATH}/bin/moc.exe" \
-DQT_MAIN_LIBRARY="${QT_PATH}/lib/libqtmain.a" \
-DQT_UIC_EXECUTABLE="${QT_PATH}/bin/uic.exe" \
-DQT_RCC_EXECUTABLE="${QT_PATH}/bin/rcc.exe" \
-DQT_LIBRARY_DIR="${QT_PATH}/lib" \
-DQT_QTCORE_LIBRARY="${QT_PATH}/lib/libQtCore4.a" \
\
-DSDL_INCLUDE_DIR="${THIRD_PARTY_PATH}/${SDL_VER}/include/SDL" \
-DSDL_LIBRARY="${THIRD_PARTY_PATH}/${SDL_VER}/lib/libSDL.dll.a" \
-DSDLMAIN_LIBRARY="${THIRD_PARTY_PATH}/${SDL_VER}/lib/libSDLmain.a" \
\
-DTIFF_INCLUDE_DIR="${THIRD_PARTY_PATH}/${TIFF_VER}/include" \
-DTIFF_LIBRARY="${THIRD_PARTY_PATH}/${TIFF_VER}/lib/libtiff.dll.a" \
\
-DZLIB_INCLUDE_DIR="${THIRD_PARTY_PATH}/${ZLIB_VER}/include"

echo ---- after cmake call ----