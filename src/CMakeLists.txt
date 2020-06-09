include (FeatureSummary)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "DEBUG")
  #SET(CMAKE_BUILD_TYPE "RELEASE")
  #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
  #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

INCLUDE_DIRECTORIES("${CMAKE_HOME_DIRECTORY}")

option(WITH_SAMPLE "Compile sample programs using lexer and/or parser" ON)
add_feature_info(Sample WITH_SAMPLE "Sample programs.")
if(WITH_SAMPLE)
  add_subdirectory(sample)
endif(WITH_SAMPLE)

option(WITH_PYTHON "Compile python extension." ON)
add_feature_info(Sample WITH_PYTHON "Python extension.")
if(WITH_PYTHON)
  add_subdirectory(cif_python)
endif(WITH_PYTHON)

feature_summary(WHAT ALL)
