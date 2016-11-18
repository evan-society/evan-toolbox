SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0020 OLD)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtOpenGL QtXml QtSvg)
FIND_PACKAGE(Qwt REQUIRED)
FIND_PACKAGE(Qwt3D REQUIRED)
FIND_PACKAGE(OpenSceneGraph 2.0.0 REQUIRED osgViewer osgText osgDB osgManipulator osgGA osgUtil osgVolume osg)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Iconv REQUIRED)

IF(WIN32)
	SET(LAPACKE_DIR $ENV{LAPACKE_DIR} CACHE PATH "Path where LAPACKE cmake files are located")
	FIND_PACKAGE(LAPACKE NO_MODULE REQUIRED)
	FIND_PACKAGE(SDL REQUIRED)
ELSE(WIN32)
	FIND_PACKAGE(LAPACK REQUIRED)
ENDIF()

OPTION(BUILD_GMM "Build GMM Library" ON)
OPTION(LINK_STATICLY "Attempt to create one big binary, with all dependencies linked staticly" ON)

#FILE(GLOB ET_resources "*.qrc")
QT4_ADD_RESOURCES(ET_resources Resources.qrc)

FILE(GLOB ET_shaders "Shaders/*.frag" "Shaders/*.vert")
FILE(GLOB ET_examples "Examples/*.vpn" "Examples/*.txt" "Examples/*.obj")
FILE(GLOB Templand_examples "Examples/Templand/*.ltb" "Examples/Templand/*.frm" "Examples/Templand/*.obj")
FILE(GLOB ET_sources "DataTypes/*.cpp" "MainWindow/*.cpp" "Nodes/*.cpp" "Scheduler/*.cpp" "Utilities/*.cpp" "Utilities/*.c" "DataTypes/*.h" "MainWindow/*.h" "Nodes/*.h" "Scheduler/*.h" "Utilities/*.h" "DataTypes/*.hpp" "MainWindow/*.hpp" "Nodes/*.hpp" "Scheduler/*.hpp" "Utilities/*.hpp")

IF(${BUILD_GMM})
  ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/EW")
  ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/GMM")
ENDIF()

IF(${LINK_STATICLY})
	SET(CMAKE_CXX_FLAGS "-fpermissive ${CMAKE_CXX_FLAGS}")
	SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libgfortran ${CMAKE_EXE_LINKER_FLAGS}") 
	ADD_DEFINITIONS(-DOSG_LIBRARY_STATIC)
ENDIF()

ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Nodes")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/GMM/include" "${CMAKE_CURRENT_SOURCE_DIR}/EW/include" "${ICONV_INCLUDE_DIR}" "${QT_HEADERS_DIR}" "${QT_QTCORE_INCLUDE_DIR}" "${QT_QTGUI_INCLUDE_DIR}" "${QT_QTOPENGL_INCLUDE_DIR}" "${QWT_INCLUDE_DIRS}" "${QWT3D_INCLUDE_DIRS}" "${OSG_INCLUDE_DIR}")

SET(ET_Libs GMM ew ${QWT_LIBRARY} ${QWT3D_LIBRARY} Qt4::QtCore Qt4::QtGui Qt4::QtOpenGL Qt4::QtXml Qt4::QtSvg ${OPENSCENEGRAPH_LIBRARIES} ${OPENGL_LIBRARIES}  ${LIBXML2_LIBRARIES} ${ICONV_LIBRARY})
IF(WIN32)
	SET(ET_Libs ${ET_Libs} ${LAPACKE_LIBRARIES} ${SDL_LIBRARY} winmm ws2_32 imm32 version)
ELSE(WIN32)
	SET(ET_Libs ${ET_Libs} ${LAPACK_LIBRARIES})
ENDIF()

ADD_EXECUTABLE(ET Evan.cpp ${ET_sources} ${ET_headers} ${Node_sources} ${ET_resources})
TARGET_LINK_LIBRARIES(ET ${ET_Libs})

SET_PROPERTY(TARGET ET PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
INSTALL(TARGETS ET RUNTIME DESTINATION ${ET_INSTALL_BIN_DIR} COMPONENT binaries)
INSTALL(FILES ${ET_shaders} DESTINATION ${ET_INSTALL_BIN_DIR}/Shaders COMPONENT shaders)
INSTALL(FILES ${ET_examples} DESTINATION ${ET_INSTALL_BIN_DIR}/Examples COMPONENT VPN_Examples)
INSTALL(FILES ${Templand_examples} DESTINATION ${ET_INSTALL_BIN_DIR}/Examples/Templand COMPONENT Templand_Examples)
