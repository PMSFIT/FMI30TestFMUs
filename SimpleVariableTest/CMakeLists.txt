cmake_minimum_required(VERSION 3.5)
project(SimpleVariableTest)
set(PUBLIC_LOGGING OFF CACHE BOOL "Enable logging via FMI logger")
set(PRIVATE_LOGGING OFF CACHE BOOL "Enable private logging to file")
if(WIN32)
	set(PRIVATE_LOG_PATH "C:/TEMP/SimpleVariableTest.log" CACHE FILEPATH "Path to write private log file to")
else()
	set(PRIVATE_LOG_PATH "/tmp/SimpleVariableTest.log" CACHE FILEPATH "Path to write private log file to")
endif()
set(VERBOSE_FMI_LOGGING OFF CACHE BOOL "Enable detailed FMI function logging")

set(FMU_FMI_VERSION "3.0-alpha.3" CACHE STRING "FMI Standard Version Number")
set(FMU_VERSION "1.0" CACHE STRING "FMU Version Number (informative only)")
set(FMU_AUTHOR "PMSF" CACHE STRING "FMU Author Information")
set(FMU_MODEL_NAME "${PROJECT_NAME}" CACHE STRING "FMU Model Name")
set(FMU_BCS_MODEL_IDENTIFIER "${FMU_MODEL_NAME}BCS" CACHE STRING "FMU BCS Model Identifier")

string(TIMESTAMP FMU_TIMESTAMP UTC)
string(MD5 FMU_TOKEN modelDescription.in.xml)

configure_file(modelDescription.in.xml modelDescription.xml @ONLY)

add_library(${FMU_BCS_MODEL_IDENTIFIER} SHARED SimpleVariableTest.c)
set_target_properties(${FMU_BCS_MODEL_IDENTIFIER} PROPERTIES PREFIX "")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_SHARED_OBJECT")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_MODEL_IDENTIFIER=${FMU_BCS_MODEL_IDENTIFIER}")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_TOKEN=\"${FMU_TOKEN}\"")
if(PRIVATE_LOGGING)
	file(TO_NATIVE_PATH ${PRIVATE_LOG_PATH} PRIVATE_LOG_PATH_NATIVE)
	string(REPLACE "\\" "\\\\" PRIVATE_LOG_PATH_ESCAPED ${PRIVATE_LOG_PATH_NATIVE})
	target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE
		"PRIVATE_LOG_PATH=\"${PRIVATE_LOG_PATH_ESCAPED}\"")
endif()
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE
	$<$<BOOL:${PUBLIC_LOGGING}>:PUBLIC_LOGGING>
	$<$<BOOL:${VERBOSE_FMI_LOGGING}>:VERBOSE_FMI_LOGGING>)
if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(FMI_BINARIES_PLATFORM "x86_64-windows")
	else()
		set(FMI_BINARIES_PLATFORM "i686-windows")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(FMI_BINARIES_PLATFORM "x86_64-windows")
	else()
		set(FMI_BINARIES_PLATFORM "i686-linux")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(FMI_BINARIES_PLATFORM "x86_64-darwin")
	else()
		set(FMI_BINARIES_PLATFORM "i686-darwin")
	endif()
endif()
add_custom_command(TARGET ${FMU_BCS_MODEL_IDENTIFIER}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/sources"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/modelDescription.xml"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/SimpleVariableTest.c" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/SimpleVariableTest.c"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/SimpleVariableTest.h" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/SimpleVariableTest.h"
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${FMU_BCS_MODEL_IDENTIFIER}> "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
	COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/buildfmu" ${CMAKE_COMMAND} -E tar "cfv" "../${FMU_MODEL_NAME}.fmu" --format=zip "modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}")
