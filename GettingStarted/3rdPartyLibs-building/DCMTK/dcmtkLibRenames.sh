#!/bin/sh

echo ---- Preparing to copy and rename external libraries ----

THIRD_PARTY_PATH="/e/3rdParty-build"
DCMTK_EXTERNAL_PATH="${THIRD_PARTY_PATH}/DCMTK-external"

ICONV_VER="libiconv-1.14"
PNG_VER="libpng-1.6.7"
TIFF_VER="tiff-3.9.7"
XML_VER="libxml2-2.9.1"
ZLIB_VER="zlib-1.2.8"

rm -R -f ${DCMTK_EXTERNAL_PATH}
mkdir -p ${DCMTK_EXTERNAL_PATH}

mkdir -p ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}/include
mkdir -p ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}/lib

mkdir -p ${DCMTK_EXTERNAL_PATH}/${PNG_VER}/include
mkdir -p ${DCMTK_EXTERNAL_PATH}/${PNG_VER}/lib

mkdir -p ${DCMTK_EXTERNAL_PATH}/${TIFF_VER}/include
mkdir -p ${DCMTK_EXTERNAL_PATH}/${TIFF_VER}/lib

mkdir -p ${DCMTK_EXTERNAL_PATH}/${XML_VER}/include
mkdir -p ${DCMTK_EXTERNAL_PATH}/${XML_VER}/lib

mkdir -p ${DCMTK_EXTERNAL_PATH}/${ZLIB_VER}/include
mkdir -p ${DCMTK_EXTERNAL_PATH}/${ZLIB_VER}/lib


cp -R -v -f -p ${THIRD_PARTY_PATH}/${ICONV_VER}/include ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}
cp -v -f -p ${THIRD_PARTY_PATH}/${ICONV_VER}/lib/libiconv.a ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}/lib/iconv_o.lib
cp -v -f -p ${THIRD_PARTY_PATH}/${ICONV_VER}/lib/libiconv.a ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}/lib/libiconv_o.lib
cp -v -f -p ${THIRD_PARTY_PATH}/${ICONV_VER}/lib/libcharset.a ${DCMTK_EXTERNAL_PATH}/${ICONV_VER}/lib/libchset_o.lib

cp -R -v -f -p ${THIRD_PARTY_PATH}/${PNG_VER}/include ${DCMTK_EXTERNAL_PATH}/${PNG_VER}
cp -v -f -p ${THIRD_PARTY_PATH}/${PNG_VER}/lib/libpng.a ${DCMTK_EXTERNAL_PATH}/${PNG_VER}/lib/libpng_o.lib

cp -R -v -f -p ${THIRD_PARTY_PATH}/${TIFF_VER}/include ${DCMTK_EXTERNAL_PATH}/${TIFF_VER}
cp -v -f -p ${THIRD_PARTY_PATH}/${TIFF_VER}/lib/libtiff.a ${DCMTK_EXTERNAL_PATH}/${TIFF_VER}/lib/libtiff_o.lib

cp -R -v -f -p ${THIRD_PARTY_PATH}/${XML_VER}/include ${DCMTK_EXTERNAL_PATH}/${XML_VER}
cp -v -f -p ${THIRD_PARTY_PATH}/${XML_VER}/lib/libxml2.dll.a ${DCMTK_EXTERNAL_PATH}/${XML_VER}/lib/libxml2_o.lib
#cp -v -f -p ${THIRD_PARTY_PATH}/${XML_VER}/lib/libxml2.a ${DCMTK_EXTERNAL_PATH}/${XML_VER}/lib/libxml2_o.lib
mv ${DCMTK_EXTERNAL_PATH}/${XML_VER}/include/libxml2/libxml ${DCMTK_EXTERNAL_PATH}/${XML_VER}/include
rm -f -R ${DCMTK_EXTERNAL_PATH}/${XML_VER}/include/libxml2

cp -R -v -f -p ${THIRD_PARTY_PATH}/${ZLIB_VER}/include ${DCMTK_EXTERNAL_PATH}/${ZLIB_VER}
cp -v -f -p ${THIRD_PARTY_PATH}/${ZLIB_VER}/lib/libz.a ${DCMTK_EXTERNAL_PATH}/${ZLIB_VER}/lib/zlib_o.lib

echo ---- Preparing and renaming of external libraries done ----