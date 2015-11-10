#
# Sets ET installation paths.
#

# NOTE: These paths are all relative to the project installation prefix.

# Executables
if(NOT DEFINED ET_INSTALL_BIN_DIR)
  set(ET_INSTALL_BIN_DIR "bin" CACHE PATH "Installation path for executables")
endif()

# Libraries
if(NOT DEFINED ET_INSTALL_LIB_DIR)
  set(ET_INSTALL_LIB_DIR "lib" CACHE PATH "Installation path for libraries")
endif()

# Header files
if(NOT DEFINED ET_INSTALL_INC_DIR)
  set(ET_INSTALL_INC_DIR "include" CACHE PATH "Installation path for headers")
endif()
