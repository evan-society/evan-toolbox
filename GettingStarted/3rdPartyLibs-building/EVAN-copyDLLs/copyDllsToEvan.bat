:: echo ---- using robocopy ----

set GCC_BIN_DIR=E:\TDM-GCC-32\bin
set THIRD_PARTY_DIR=E:\3rdParty-build

set QT_PATH="E:\qt\qt-4.8.5"

:: note that we automatically build the directory 
:: with the currently set lapack version
set LAPACK_BASE_DIR=E:\3rdParty

:: dll for the Evan binary
set EVAN_DIR=E:\ET-git-build-withEW\VPN\Evan
:: includes and libs for building the Evan executable
set EVAN_EXTERNAL_DIR=E:\ET-git-build-withEW\external

set CURL_VER=curl-7.20.1


:: usually no DLLs in here, but we need the libs and includes anyway
set DCMTK_VER=dcmtk-3.6.1_20121102

set GLUT_VER=freeglut-2.8.1
set FREETYPE_VER=freetype-2.4.12
set GIF_VER=giflib-4.2.3
set GLEW_VER=glew-1.10.0
set JPEG_VER=jpeg-9

:: usually only static lib
set LAPACK_VER=lapack-3.4.0
set LAPACK_BASE_DIR=%LAPACK_BASE_DIR%\%LAPACK_VER%
echo "LAPACK_BASE_DIR = %LAPACK_BASE_DIR%"
wait

set ICONV_VER=libiconv-1.14
set PNG_VER=libpng-1.6.7
set XML_VER=libxml2-2.9.1

set QWT_VER=qwt-5.2
set QWTPLOT_VER=qwtplot3d-0.2.7
set SDL_VER=SDL-1.2.15
set TIFF_VER=tiff-3.9.7
set ZLIB_VER=zlib-1.2.8

set OSG_VER_NUM=3.0.1
set OSG_VER=OSG-%OSG_VER_NUM%
set OSG_PLUGINS_VER=osgPlugins-%OSG_VER_NUM%


echo ---- moving includes and libs to "%EVAN_EXTERNAL_DIR%" ----
::del /Q /S %EVAN_EXTERNAL_DIR%\*
rmdir /S /Q "%EVAN_EXTERNAL_DIR%"
mkdir "%EVAN_EXTERNAL_DIR%"

mkdir "%EVAN_EXTERNAL_DIR%\%CURL_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%CURL_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%CURL_VER%\include"
copy "%THIRD_PARTY_DIR%\%CURL_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%CURL_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%CURL_VER%\include" "%EVAN_EXTERNAL_DIR%\%CURL_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%DCMTK_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%DCMTK_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%DCMTK_VER%\include"
copy "%THIRD_PARTY_DIR%\%DCMTK_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%DCMTK_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%DCMTK_VER%\include" "%EVAN_EXTERNAL_DIR%\%DCMTK_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%GLUT_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%GLUT_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%GLUT_VER%\include"
copy "%THIRD_PARTY_DIR%\%GLUT_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%GLUT_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%GLUT_VER%\include" "%EVAN_EXTERNAL_DIR%\%GLUT_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%FREETYPE_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%FREETYPE_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%FREETYPE_VER%\include"
copy "%THIRD_PARTY_DIR%\%FREETYPE_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%FREETYPE_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%FREETYPE_VER%\include" "%EVAN_EXTERNAL_DIR%\%FREETYPE_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%GIF_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%GIF_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%GIF_VER%\include"
copy "%THIRD_PARTY_DIR%\%GIF_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%GIF_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%GIF_VER%\include" "%EVAN_EXTERNAL_DIR%\%GIF_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%GLEW_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%GLEW_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%GLEW_VER%\include"
copy "%THIRD_PARTY_DIR%\%GLEW_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%GLEW_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%GLEW_VER%\include" "%EVAN_EXTERNAL_DIR%\%GLEW_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%JPEG_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%JPEG_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%JPEG_VER%\include"
copy "%THIRD_PARTY_DIR%\%JPEG_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%JPEG_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%JPEG_VER%\include" "%EVAN_EXTERNAL_DIR%\%JPEG_VER%\include"

::LAPACK special handling!
mkdir "%EVAN_EXTERNAL_DIR%\%LAPACK_VER%"
copy "%LAPACK_BASE_DIR%\liblapack.a" "%EVAN_EXTERNAL_DIR%\%LAPACK_VER%"
copy "%LAPACK_BASE_DIR%\librefblas.a" "%EVAN_EXTERNAL_DIR%\%LAPACK_VER%"

mkdir "%EVAN_EXTERNAL_DIR%\%ICONV_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%ICONV_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%ICONV_VER%\include"
copy "%THIRD_PARTY_DIR%\%ICONV_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%ICONV_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%ICONV_VER%\include" "%EVAN_EXTERNAL_DIR%\%ICONV_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%PNG_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%PNG_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%PNG_VER%\include"
copy "%THIRD_PARTY_DIR%\%PNG_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%PNG_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%PNG_VER%\include" "%EVAN_EXTERNAL_DIR%\%PNG_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%XML_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%XML_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%XML_VER%\include"
copy "%THIRD_PARTY_DIR%\%XML_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%XML_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%XML_VER%\include" "%EVAN_EXTERNAL_DIR%\%XML_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%OSG_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%OSG_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%OSG_VER%\include"
copy "%THIRD_PARTY_DIR%\%OSG_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%OSG_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%OSG_VER%\include" "%EVAN_EXTERNAL_DIR%\%OSG_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%QWT_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%QWT_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%QWT_VER%\include"
copy "%THIRD_PARTY_DIR%\%QWT_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%QWT_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%QWT_VER%\include" "%EVAN_EXTERNAL_DIR%\%QWT_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%QWTPLOT_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%QWTPLOT_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%QWTPLOT_VER%\include"
copy "%THIRD_PARTY_DIR%\%QWTPLOT_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%QWTPLOT_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%QWTPLOT_VER%\include" "%EVAN_EXTERNAL_DIR%\%QWTPLOT_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%SDL_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%SDL_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%SDL_VER%\include"
copy "%THIRD_PARTY_DIR%\%SDL_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%SDL_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%SDL_VER%\include" "%EVAN_EXTERNAL_DIR%\%SDL_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%TIFF_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%TIFF_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%TIFF_VER%\include"
copy "%THIRD_PARTY_DIR%\%TIFF_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%TIFF_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%TIFF_VER%\include" "%EVAN_EXTERNAL_DIR%\%TIFF_VER%\include"

mkdir "%EVAN_EXTERNAL_DIR%\%ZLIB_VER%"
mkdir "%EVAN_EXTERNAL_DIR%\%ZLIB_VER%\lib"
mkdir "%EVAN_EXTERNAL_DIR%\%ZLIB_VER%\include"
copy "%THIRD_PARTY_DIR%\%ZLIB_VER%\lib\*" "%EVAN_EXTERNAL_DIR%\%ZLIB_VER%\lib"
robocopy /S "%THIRD_PARTY_DIR%\%ZLIB_VER%\include" "%EVAN_EXTERNAL_DIR%\%ZLIB_VER%\include"





echo ---- deleting all DLLs in %EVAN_DIR% ----
del /q %EVAN_DIR%\*.dll

echo ---- copying TDM-GCC dlls ----

copy %GCC_BIN_DIR%\libgcc_s_sjlj-1.dll %EVAN_DIR%
copy %GCC_BIN_DIR%\libgfortran-3.dll %EVAN_DIR%
copy %GCC_BIN_DIR%\libquadmath-0.dll %EVAN_DIR%

echo ---- copying Qt release dlls ----
::copy %QT_PATH%\plugins\imageformats\*.dll %EVAN_DIR%
::del /Q %EVAN_DIR%\q*d4.dll

copy %QT_PATH%\bin\*.dll %EVAN_DIR%
::del /Q %EVAN_DIR%\Qt*d4.dll

echo ---- copying dlls from %THIRD_PARTY_DIR% to %EVAN_DIR% ----

copy %THIRD_PARTY_DIR%\%CURL_VER%\bin\*.dll %EVAN_DIR%
:: copy %THIRD_PARTY_DIR%\%DCMTK_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%GLUT_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%FREETYPE_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%GIF_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%GLEW_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%JPEG_VER%\bin\*.dll %EVAN_DIR%
:: copy %THIRD_PARTY_DIR%\%LAPACK_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%ICONV_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%PNG_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%XML_VER%\bin\*.dll %EVAN_DIR%
:: due to the DCMTK-lib-renaming and since xml2 was used as a shared lib
::copy %THIRD_PARTY_DIR%\%XML_VER%\bin\libxml2*.dll %EVAN_DIR%\libxml2_o.dll

copy %THIRD_PARTY_DIR%\%OSG_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%OSG_VER%\bin\%OSG_PLUGINS_VER%\*.dll %EVAN_DIR%

copy %THIRD_PARTY_DIR%\%QWT_VER%\lib\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%QWTPLOT_VER%\lib\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%SDL_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%TIFF_VER%\bin\*.dll %EVAN_DIR%
copy %THIRD_PARTY_DIR%\%ZLIB_VER%\bin\*.dll %EVAN_DIR%
