cmake_minimum_required (VERSION 3.0)

project(clit)
if(DEFINED ENV{NIX_CC})
    message("Detected nix project")
    include(nix.cmake)
    return()
endif()

## This is legacy code

set(PROJECT_TYPE "header-only")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Libreria header only con multiples sublibrerias")

# Directorio base donde instalamos la libreria
set(CMAKE_INSTALL_PREFIX "/data/programs/vtcommon")

list(APPEND VT_LIBS "dl")

# Modules cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

include(GenericProject)
