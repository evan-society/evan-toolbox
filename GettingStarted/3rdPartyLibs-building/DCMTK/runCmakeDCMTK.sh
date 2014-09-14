#!/bin/sh

echo ---- EXECUTING cmake call ----
echo ---- MAKE SURE TO RUN THIS FROM THE DESIRED DCMTK-CMAKE-BUILD DIRECTORY  ----
echo ---- IN ORDER NOT TO CLUTTER THE DCMTK-SRC DIRECTORY WITH CMAKE FILES ----
echo ---- USE ANOTHER DISTINCT INSTALL DIRECTORY ---
echo ---- i.e. do not use the DCMTK-source nor the cmake-build directory ----

echo ---- CURRENT, i.e. the BUILD - directory is ----
echo ${PWD}

echo ---- DCMTK SOURCE is in directory ----
echo "$1"

INSTALL_PATH="/e/3rdParty-build"

# THIRD_PARTY_PATH="/e/3rdParty-build"
# THIRD_PARTY_PATH="e:/3rdParty-build"
THIRD_PARTY_PATH="${INSTALL_PATH}/DCMTK-external"


ICONV_VER="libiconv-1.14" # used by libxml2
XML2_VER="libxml2-2.9.1"
#PNG_VER="libpng-1.4.12"
PNG_VER="libpng-1.6.7"
TIFF_VER="tiff-3.9.7"
ZLIB_VER="zlib-1.2.8"

DCMTK_VER="dcmtk-3.6.1_20121102"

# iconv seems to need special treatment =>
# CMAKE_INCLUDE_PATH, CMAKE_PREFIX_PATH, CMAKE_CXX_FLAGS
# and CMAKE_VERBOSE_MAKEFILE to better spot the problem

#JPEG_VER=""
#-llibtiff_o -llibjpeg

# xml static didn't work as expected
# -DCMAKE_CXX_FLAGS="-DLIBXML_STATIC -I ${THIRD_PARTY_PATH}/${ICONV_VER}/include -L ${THIRD_PARTY_PATH}/${ICONV_VER}/lib" \

cmake "$1" -G "MSYS Makefiles" \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX="${INSTALL_PATH}/${DCMTK_VER}" \
\
-DCMAKE_INCLUDE_PATH="${THIRD_PARTY_PATH}/${ICONV_VER}/include" \
-DCMAKE_PREFIX_PATH="${THIRD_PARTY_PATH}/${ICONV_VER}/include" \
-DCMAKE_CXX_FLAGS="-I ${THIRD_PARTY_PATH}/${ICONV_VER}/include -L ${THIRD_PARTY_PATH}/${ICONV_VER}/lib" \
-DCMAKE_VERBOSE_MAKEFILE=1 \
\
-DDCMTK_WITH_ICONV=1 \
-DWITH_LIBICONVINC="${THIRD_PARTY_PATH}/${ICONV_VER}" \
-DDCMTK_WITH_XML=1 \
-DWITH_LIBXMLINC="${THIRD_PARTY_PATH}/${XML2_VER}" \
\
-DCMTK_WITH_PNG=1 \
-DWITH_LIBPNGINC="${THIRD_PARTY_PATH}/${PNG_VER}" \
\
-DCMTK_WITH_TIFF=1 \
-DWITH_LIBTIFFINC="${THIRD_PARTY_PATH}/${TIFF_VER}" \
\
-DCMTK_WITH_ZLIB=1 \
-DWITH_ZLIBINC="${THIRD_PARTY_PATH}/${ZLIB_VER}"
