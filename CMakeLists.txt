# Used for conan
cmake_minimum_required(VERSION 3.10)

project(kvarint)

add_library(kvarint STATIC kvarint.c)

include(GNUInstallDirs)
message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

file(GLOB 
  KVARINT_INCLUDE 
  *.h
)

#################################
# Install include
#################################
install(FILES ${KVARINT_INCLUDE} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kvarint)

target_include_directories(kvarint 
  PUBLIC 
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/kvarint>"
)

#################################
# Install lib
#################################
install(TARGETS kvarint DESTINATION ${CMAKE_INSTALL_LIBDIR})

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
