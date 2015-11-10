# QwtPlot3D is not a  program, but a feature-rich Qt/OpenGL-based C++ programming library, providing essentially a  bunch of 3D-widgets for programmers. The library is zlib licensed.
# available at http://http://qwtplot3d.sourceforge.net/
#
# The module defines the following variables:
#  QWT3D_FOUND - the system has Qwt
#  QWT3D_INCLUDE_DIR - where to find qwt_plot.h
#  QWT3D_INCLUDE_DIRS - qwt includes
#  QWT3D_LIBRARY - where to find the Qwt library
#  QWT3D_LIBRARIES - aditional libraries
#  QWT3D_MAJOR_VERSION - major version
#  QWT3D_MINOR_VERSION - minor version
#  QWT3D_PATCH_VERSION - patch version
#  QWT3D_VERSION_STRING - version (ex. 5.2.1)
#  QWT3D_ROOT_DIR - root dir (ex. /usr/local)

#=============================================================================
# Copyright 2010-2013, Julien Schueller
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies, 
# either expressed or implied, of the FreeBSD Project.
#=============================================================================


find_path ( QWT3D_INCLUDE_DIR
  NAMES qwt3d_plot.h
  HINTS ${QT_INCLUDE_DIR}
  PATH_SUFFIXES qwtplot3d qwtplot3d-qt3 qwtplot3d-qt4 qwtplot3d-qt5
)

set ( QWT3D_INCLUDE_DIRS ${QWT3D_INCLUDE_DIR} )

# version
set ( _VERSION_FILE ${QWT3D_INCLUDE_DIR}/qwt3d_global.h )
if ( EXISTS ${_VERSION_FILE} )
  file ( STRINGS ${_VERSION_FILE} _VERSION_LINE REGEX "define[ ]+QWT3D_VERSION_STR" )
  if ( _VERSION_LINE )
    string ( REGEX REPLACE ".*define[ ]+QWT3D_VERSION_STR[ ]+\"(.*)\".*" "\\1" QWT3D_VERSION_STRING "${_VERSION_LINE}" )
    string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" QWT3D_MAJOR_VERSION "${QWT3D_VERSION_STRING}" )
    string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2" QWT3D_MINOR_VERSION "${QWT3D_VERSION_STRING}" )
    string ( REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" QWT3D_PATCH_VERSION "${QWT3D_VERSION_STRING}" )
  endif ()
endif ()


# check version
set ( _QWT3D_VERSION_MATCH TRUE )
if ( Qwt3D_FIND_VERSION AND QWT3D_VERSION_STRING )
  if ( Qwt3D_FIND_VERSION_EXACT )
    if ( NOT Qwt3D_FIND_VERSION VERSION_EQUAL QWT3D_VERSION_STRING )
      set ( _QWT3D_VERSION_MATCH FALSE )
    endif ()
  else ()
    if ( QWT3D_VERSION_STRING VERSION_LESS Qwt_FIND_VERSION )
      set ( _QWT3D_VERSION_MATCH FALSE )
    endif ()
  endif ()
endif ()


find_library ( QWT3D_LIBRARY
  NAMES qwtplot3d qwtplot3d-qt3 qwtplot3d-qt4 qwtplot3d-qt5
  HINTS ${QT_LIBRARY_DIR}
)

set ( QWT3D_LIBRARIES ${QWT3D_LIBRARY} )


# try to guess root dir from include dir
if ( QWT3D_INCLUDE_DIR )
  string ( REGEX REPLACE "(.*)/include.*" "\\1" QWT3D_ROOT_DIR ${QWT3D_INCLUDE_DIR} )
# try to guess root dir from library dir
elseif ( QWT3D_LIBRARY )
  string ( REGEX REPLACE "(.*)/lib[/|32|64].*" "\\1" QWT3D_ROOT_DIR ${QWT3D_LIBRARY} )
endif ()


# handle the QUIETLY and REQUIRED arguments
include ( FindPackageHandleStandardArgs )
if ( CMAKE_VERSION LESS 2.8.3 )
  find_package_handle_standard_args( Qwt3D DEFAULT_MSG QWT3D_LIBRARY QWT3D_INCLUDE_DIR _QWT3D_VERSION_MATCH )
else ()
  find_package_handle_standard_args( Qwt3D REQUIRED_VARS QWT3D_LIBRARY QWT3D_INCLUDE_DIR _QWT3D_VERSION_MATCH VERSION_VAR QWT3D_VERSION_STRING )
endif ()


mark_as_advanced (
  QWT3D_LIBRARY 
  QWT3D_LIBRARIES
  QWT3D_INCLUDE_DIR
  QWT3D_INCLUDE_DIRS
  QWT3D_MAJOR_VERSION
  QWT3D_MINOR_VERSION
  QWT3D_PATCH_VERSION
  QWT3D_VERSION_STRING
  QWT3D_ROOT_DIR
)
