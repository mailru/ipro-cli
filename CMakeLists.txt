cmake_minimum_required(VERSION 3.6)
project(ipro_cli C)

set(SOURCE_FILES buf.c buf.h)
add_executable(ipro_scanf ${SOURCE_FILES} ipro_scanf.c)
add_executable(ipro_printf ${SOURCE_FILES} ipro_printf.c)

include(GNUInstallDirs)
install(TARGETS ipro_printf ipro_scanf DESTINATION ${CMAKE_INSTALL_BINDIR})

message(STATUS "")
