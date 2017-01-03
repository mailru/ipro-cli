cmake_minimum_required(VERSION 2.6)
project(ipro_cli)

set(SOURCE_FILES buf.c buf.h)
add_executable(ipro_scanf ${SOURCE_FILES} ipro_scanf.c)
add_executable(ipro_printf ${SOURCE_FILES} ipro_printf.c)

message(STATUS "")
