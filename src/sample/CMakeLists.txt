FIND_PACKAGE(Boost COMPONENTS iostreams REQUIRED)
  
#Keep boost from trying to be too helpful w/ VS compiler
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}")
#SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_USE_MULTITHREADED ON)
#SET(Boost_USE_STATIC_RUNTIME OFF)

ADD_EXECUTABLE(testLexerFile testLexerFile.cxx)

ADD_EXECUTABLE(testParserFile testParserFile.cxx)
TARGET_LINK_LIBRARIES(testParserFile ${Boost_LIBRARIES})

#ADD_EXECUTABLE(testLexer testLexer.cxx lexer_file_iterator.cpp)

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall")
ENDIF()

