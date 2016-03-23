CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtOpenGL QtXml QtSvg)
FIND_PACKAGE(Qwt REQUIRED)
FIND_PACKAGE(Qwt3D REQUIRED)
FIND_PACKAGE(OpenSceneGraph 2.0.0 REQUIRED osg osgDB osgUtil osgVolume osgGA osgViewer osgManipulator)
FIND_PACKAGE(LAPACK REQUIRED)
find_package(OpenGL REQUIRED)

OPTION(BUILD_GMM "Build GMM Library" ON)

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

ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/Nodes")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/GMM/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/EW/include")
INCLUDE_DIRECTORIES("${QT_HEADERS_DIR}" "${QT_QTCORE_INCLUDE_DIR}" "${QT_QTGUI_INCLUDE_DIR}" "${QT_QTOPENGL_INCLUDE_DIR}" "${QWT_INCLUDE_DIRS}" "${QWT3D_INCLUDE_DIRS}")

SET(ET_Libs GMM ew Qt4::QtCore Qt4::QtGui Qt4::QtOpenGL Qt4::QtXml Qt4::QtSvg  ${OPENSCENEGRAPH_LIBRARIES} ${OSGVOLUME_LIBRARIES} ${LAPACK_LIBRARIES} ${OPENGL_LIBRARIES} ${QWT_LIBRARIES} ${QWT3D_LIBRARIES})

ADD_EXECUTABLE(ET Evan.cpp ${ET_sources} ${ET_headers} ${Node_sources} ${ET_resources})
TARGET_LINK_LIBRARIES(ET ${ET_Libs})
SET_PROPERTY(TARGET ET PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
INSTALL(TARGETS ET RUNTIME DESTINATION ${ET_INSTALL_BIN_DIR} COMPONENT binaries)
INSTALL(FILES ${ET_shaders} DESTINATION ${ET_INSTALL_BIN_DIR}/Shaders COMPONENT shaders)
INSTALL(FILES ${ET_examples} DESTINATION ${ET_INSTALL_BIN_DIR}/Examples COMPONENT VPN_Examples)
INSTALL(FILES ${Templand_examples} DESTINATION ${ET_INSTALL_BIN_DIR}/Examples/Templand COMPONENT Templand_Examples)
