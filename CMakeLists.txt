cmake_minimum_required(VERSION 3.5)
project(FMI30TestFMUs)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(PUBLIC_LOGGING OFF CACHE BOOL "Enable logging via FMI logger")
set(PRIVATE_LOGGING OFF CACHE BOOL "Enable private logging to file")
if(WIN32)
	set(PRIVATE_LOG_DIRECTORY "C:/TEMP/" CACHE PATH "Directory to write private log file to")
else()
	set(PRIVATE_LOG_DIRECTORY "/tmp/" CACHE PATH "Directory to write private log file to")
endif()
set(VERBOSE_FMI_LOGGING OFF CACHE BOOL "Enable detailed FMI function logging")

set(FMU_FMI_VERSION "3.0-beta.2" CACHE STRING "FMI Standard Version Number")
set(FMU_VERSION "1.0" CACHE STRING "FMU Version Number (informative only)")
set(FMU_AUTHOR "PMSF" CACHE STRING "FMU Author Information")

include_directories( fmi-standard/headers )
add_subdirectory( SimpleVariableTest )
add_subdirectory( SimpleArrayTest )
add_subdirectory( DynamicArrayTest )
