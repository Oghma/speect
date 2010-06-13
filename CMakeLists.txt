######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : 7 June 2010                                                            ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## Main CMakeList for Speect                                                        ##
##                                                                                  ##
##                                                                                  ##
######################################################################################


#------------------------------------------------------------------------------------#
#                                Build type                                          #
#------------------------------------------------------------------------------------#

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING  #  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profile."
      FORCE)
endif()

# Restrict configuration types to the selected build type.
# Note: This needs to be done before the project command
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" CACHE INTERNAL "internal")


#------------------------------------------------------------------------------------#
#                          CMake minimum version                                     #
#------------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


#------------------------------------------------------------------------------------#
#                             Get all CMake modules                                  #
#------------------------------------------------------------------------------------#

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


#------------------------------------------------------------------------------------#
#                        Top level system checks and options                         #
#------------------------------------------------------------------------------------#

# Version information
include(spctVersion)

# Build options
include(spctOptions)

# System tests/checks
include(spctSystem)

# Compiler options/tests
include(spctCompilers)

# Operating system checks
include(spctOS)

# Build options
include(spctBuild)

# Speect settings
include(spctSettings)



add_subdirectory(engine)
add_subdirectory(plugins)