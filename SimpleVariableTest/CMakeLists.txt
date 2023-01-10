cmake_minimum_required(VERSION 3.5)
project(SimpleVariableTest)
set(FMU_MODEL_NAME "${PROJECT_NAME}")
set(FMU_BCS_MODEL_IDENTIFIER "${FMU_MODEL_NAME}BCS")

string(TIMESTAMP FMU_TIMESTAMP UTC)
string(MD5 FMU_TOKEN modelDescription.in.xml)

configure_file(modelDescription.in.xml modelDescription.xml @ONLY)
configure_file(buildDescription.in.xml buildDescription.xml @ONLY)

add_library(${FMU_BCS_MODEL_IDENTIFIER} SHARED SimpleVariableTest.c)
set_target_properties(${FMU_BCS_MODEL_IDENTIFIER} PROPERTIES PREFIX "")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_SHARED_OBJECT")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_MODEL_IDENTIFIER=${FMU_BCS_MODEL_IDENTIFIER}")
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE "FMU_TOKEN=\"${FMU_TOKEN}\"")
if(PRIVATE_LOGGING)
	string(CONCAT PRIVATE_LOG_PATH ${PRIVATE_LOG_DIRECTORY} ${PROJECT_NAME} ".log")
	file(TO_NATIVE_PATH ${PRIVATE_LOG_PATH} PRIVATE_LOG_PATH_NATIVE)
	string(REPLACE "\\" "\\\\" PRIVATE_LOG_PATH_ESCAPED ${PRIVATE_LOG_PATH_NATIVE})
	target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE
		"PRIVATE_LOG_PATH=\"${PRIVATE_LOG_PATH_ESCAPED}\"")
endif()
target_compile_definitions(${FMU_BCS_MODEL_IDENTIFIER} PRIVATE
	$<$<BOOL:${PUBLIC_LOGGING}>:PUBLIC_LOGGING>
	$<$<BOOL:${VERBOSE_FMI_LOGGING}>:VERBOSE_FMI_LOGGING>)
add_custom_command(TARGET ${FMU_BCS_MODEL_IDENTIFIER}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/sources"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/buildDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/buildDescription.xml"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/modelDescription.xml"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/SimpleVariableTest.c" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/SimpleVariableTest.c"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/SimpleVariableTest.h" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources/SimpleVariableTest.h"
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${FMU_BCS_MODEL_IDENTIFIER}> "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}"
	COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_BINARY_DIR}/buildfmu" ${CMAKE_COMMAND} -E tar "cfv" "../${FMU_MODEL_NAME}.fmu" --format=zip "modelDescription.xml" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/sources" "${CMAKE_CURRENT_BINARY_DIR}/buildfmu/binaries/${FMI_BINARIES_PLATFORM}")
