CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(NODEDIRS ${CMAKE_CURRENT_SOURCE_DIR})

SET(Node_src CACHE INTERNAL "")

FOREACH(subdir ${NODEDIRS})
  FILE(GLOB subdir_sources "${subdir}/*.cpp" "${subdir}/*.h" "${subdir}/*.hpp")
  FILE(GLOB_RECURSE subsubdir_sources "${subdir}/*.cpp" "${subdir}/*.h" "${subdir}/*.hpp")
  SET(Node_src "${Node_src}" "${subdir_sources}" "${subsubdir_sources}")
ENDFOREACH()

SET(Node_sources "${Node_src}" PARENT_SCOPE)
