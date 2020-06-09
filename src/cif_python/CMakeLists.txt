cmake_minimum_required(VERSION 2.8)

#Keep boost from trying to be too helpful w/ VS compiler
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/python")

FIND_PACKAGE(PythonInterp REQUIRED)

FIND_PACKAGE(PythonLibs REQUIRED)

#FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(Boost COMPONENTS python iostreams REQUIRED)

INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}" "${PYTHON_INCLUDE_DIRS}"
  "${CMAKE_HOME_DIRECTORY}/python")

#ADD_LIBRARY(cif_ext 
#  SHARED 
PYTHON_ADD_MODULE(cif_ext 
  module.cpp table.cpp block.cpp file.cpp parse.cpp
  save.cpp vector_block.cpp vector_cell.cpp vector_save.cpp
  vector_string.cpp vector_table.cpp)
 
#LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(cif_ext ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

#set correct extension for windows python extension
#IF(WIN32)
#  SET_TARGET_PROPERTIES(cif_ext PROPERTIES SUFFIX ".pyd")
#ENDIF(WIN32)

#get python installation path
EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; import sys; sys.stdout.write(distutils.sysconfig.get_python_lib())"
  OUTPUT_VARIABLE PYTHON_PACKAGES_PATH
)
#INSTALL(TARGETS cif_ext)
INSTALL(TARGETS cif_ext LIBRARY DESTINATION ${PYTHON_PACKAGES_PATH}/cif)

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall")
ENDIF()

