SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0020 OLD)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui)

FILE(GLOB LG_sources "*.cpp")
FILE(GLOB LG_headers "*.h")
QT4_WRAP_UI(LG_uisrc generator.ui)

IF(WIN32)
	SET(LG_Libs Qt4::QtCore Qt4::QtGui iphlpapi winmm ws2_32 imm32 version)
ELSE(WIN32)
	SET(LG_Libs Qt4::QtCore Qt4::QtGui)
ENDIF()

ADD_EXECUTABLE(LicenseGenerator ${LG_sources} ${LG_uisrc} ${LG_headers})
TARGET_LINK_LIBRARIES(LicenseGenerator ${LG_Libs})

SET_PROPERTY(TARGET LicenseGenerator PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
#INSTALL(TARGETS LicenseGenerator RUNTIME DESTINATION ${ET_INSTALL_BIN_DIR} COMPONENT binaries)