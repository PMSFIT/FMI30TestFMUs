/*
 * PMSF FMU Framework for FMI 2.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2020 PMSF IT Consulting Pierre R. Mai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef _MSC_VER
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#if defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING)
#include <stdio.h>
#endif


#ifndef FMU_SHARED_OBJECT
#define FMI3_FUNCTION_PREFIX FMU_MODEL_IDENTIFIER ## _
#endif
#include "fmi3Functions.h"

/*
 * Variable Definitions
 *
 * Define FMI_*_LAST_IDX to the zero-based index of the last variable
 * of the given type (0 if no variables of the type exist).  This
 * ensures proper space allocation, initialisation and handling of
 * the given variables in the template code.  Optionally you can
 * define FMI_TYPENAME_VARNAME_IDX definitions (e.g. FMI_REAL_MYVAR_IDX)
 * to refer to individual variables inside your code, or for example
 * FMI_REAL_MYARRAY_OFFSET and FMI_REAL_MYARRAY_SIZE definitions for
 * array variables.
 *
 * The FMI_*_BASE_VR defines define offsets between the array indices
 * used internally and the publically visible ValueReference, allowing
 * e.g. to have globally unique VRs (which are not required for either
 * FMI 1.0 or 2.0, but are for the FMI 3.0 draft support).
 */

typedef fmi3Byte* my3Binary;
typedef fmi3Char* my3String;

/* Boolean Variables */
#define FMI_BOOLEAN_BASE_VR 0
#define FMI_BOOLEAN_LAST_IDX 0
#define FMI_BOOLEAN_VARS (FMI_BOOLEAN_LAST_IDX+1)

/* Integer Variables */
#define FMI_UINT64_BASE_VR (FMI_BOOLEAN_BASE_VR + FMI_BOOLEAN_VARS)
#define FMI_UINT64_LAST_IDX 0
#define FMI_UINT64_VARS (FMI_UINT64_LAST_IDX+1)

#define FMI_INT64_BASE_VR (FMI_UINT64_BASE_VR + FMI_UINT64_VARS)
#define FMI_INT64_LAST_IDX 0
#define FMI_INT64_VARS (FMI_INT64_LAST_IDX+1)

#define FMI_UINT32_BASE_VR (FMI_INT64_BASE_VR + FMI_INT64_VARS)
#define FMI_UINT32_LAST_IDX 0
#define FMI_UINT32_VARS (FMI_UINT32_LAST_IDX+1)

#define FMI_INT32_BASE_VR (FMI_UINT32_BASE_VR + FMI_UINT32_VARS)
#define FMI_INT32_LAST_IDX 0
#define FMI_INT32_VARS (FMI_INT32_LAST_IDX+1)

#define FMI_UINT16_BASE_VR (FMI_INT32_BASE_VR + FMI_INT32_VARS)
#define FMI_UINT16_LAST_IDX 0
#define FMI_UINT16_VARS (FMI_UINT16_LAST_IDX+1)

#define FMI_INT16_BASE_VR (FMI_UINT16_BASE_VR + FMI_UINT16_VARS)
#define FMI_INT16_LAST_IDX 0
#define FMI_INT16_VARS (FMI_INT16_LAST_IDX+1)

#define FMI_UINT8_BASE_VR (FMI_INT16_BASE_VR + FMI_INT16_VARS)
#define FMI_UINT8_LAST_IDX 0
#define FMI_UINT8_VARS (FMI_UINT8_LAST_IDX+1)

#define FMI_INT8_BASE_VR (FMI_UINT8_BASE_VR + FMI_UINT8_VARS)
#define FMI_INT8_LAST_IDX 0
#define FMI_INT8_VARS (FMI_INT8_LAST_IDX+1)

/* Real Variables */
#define FMI_FLOAT64_BASE_VR (FMI_INT8_BASE_VR + FMI_INT8_VARS)
#define FMI_FLOAT64_LAST_IDX 0
#define FMI_FLOAT64_VARS (FMI_FLOAT64_LAST_IDX+1)

#define FMI_FLOAT32_BASE_VR (FMI_FLOAT64_BASE_VR + FMI_FLOAT64_VARS)
#define FMI_FLOAT32_LAST_IDX 0
#define FMI_FLOAT32_VARS (FMI_FLOAT32_LAST_IDX+1)

/* String Variables */
#define FMI_STRING_BASE_VR (FMI_FLOAT32_BASE_VR + FMI_FLOAT32_VARS)
#define FMI_STRING_LAST_IDX 0
#define FMI_STRING_VARS (FMI_STRING_LAST_IDX+1)

/* Binary Variables */
#define FMI_BINARY_BASE_VR (FMI_STRING_BASE_VR + FMI_STRING_VARS)
#define FMI_BINARY_BINARYINPUT_IDX 0
#define FMI_BINARY_BINARYOUTPUT_IDX 1
#define FMI_BINARY_XOROUTPUT_IDX 2
#define FMI_BINARY_BINARYCONSTANT_IDX 3
#define FMI_BINARY_BINARYPARAMETER_IDX 4
#define FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX 5
#define FMI_BINARY_LAST_IDX FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX
#define FMI_BINARY_VARS (FMI_BINARY_LAST_IDX+1)

/*
 * Logging Control
 *
 * Logging is controlled via three definitions:
 *
 * - If PRIVATE_LOG_PATH is defined it gives the name of a file
 *   that is to be used as a private log file.
 * - If PUBLIC_LOGGING is defined then we will (also) log to
 *   the FMI logging facility where appropriate.
 * - If VERBOSE_FMI_LOGGING is defined then logging of basic
 *   FMI calls is enabled, which can get very verbose.
 */

/* Callbacks without const */
typedef struct {
    fmi3InstanceEnvironment        instanceEnvironment;
    fmi3CallbackLogMessage         logMessage;
    fmi3CallbackIntermediateUpdate intermediateUpdate;
} fmi3CallbackFunctionsVar;

/* FMU Instance */
typedef struct SimpleVariableTest {
    /* Members */
    my3String instanceName;
    my3String instantiationToken;
    my3String resourceLocation;
    fmi3Boolean visible;
    fmi3Boolean loggingOn;
    fmi3Boolean intermediateVariableGetRequired;
    fmi3Boolean intermediateInternalVariableGetRequired;
    fmi3Boolean intermediateVariableSetRequired;
    size_t nCategories;
    char** loggingCategories;
    fmi3CallbackFunctionsVar functions;
    fmi3Boolean boolean_vars[FMI_BOOLEAN_VARS];
    fmi3UInt64 uint64_vars[FMI_UINT64_VARS];
    fmi3Int64 int64_vars[FMI_INT64_VARS];
    fmi3UInt32 uint32_vars[FMI_UINT32_VARS];
    fmi3Int32 int32_vars[FMI_INT32_VARS];
    fmi3UInt16 uint16_vars[FMI_UINT16_VARS];
    fmi3Int16 int16_vars[FMI_INT16_VARS];
    fmi3UInt8 uint8_vars[FMI_UINT8_VARS];
    fmi3Int8 int8_vars[FMI_INT8_VARS];
    fmi3Float64 float64_vars[FMI_FLOAT64_VARS];
    fmi3Float32 float32_vars[FMI_FLOAT32_VARS];
    my3String string_vars[FMI_STRING_VARS];
    my3Binary binary_vars[FMI_BINARY_VARS];
    size_t binary_sizes[FMI_BINARY_VARS];
    double last_time;
    fmi3Boolean init_mode;
} *SimpleVariableTest;

/* Private File-based Logging just for Debugging */
#ifdef PRIVATE_LOG_PATH
static FILE* private_log_file = NULL;
#endif

void fmi_verbose_log_global(const char* format, ...)
{
#ifdef VERBOSE_FMI_LOGGING
#ifdef PRIVATE_LOG_PATH
    va_list ap;
    va_start(ap, format);
    if (private_log_file == NULL)
        private_log_file = fopen(PRIVATE_LOG_PATH,"a");
    if (private_log_file != NULL) {
        fprintf(private_log_file,"SimpleVariableTest::Global: ");
        vfprintf(private_log_file, format, ap);
        fputc('\n',private_log_file);
        fflush(private_log_file);
    }
#endif
#endif
}

void internal_log(SimpleVariableTest component,const char* category, const char* format, va_list arg)
{
#if defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING)
    char buffer[1024];
#ifdef _WIN32
    vsnprintf_s(buffer, 1024, _TRUNCATE, format, arg);
#else
    vsnprintf(buffer, 1024, format, arg);
    buffer[1023]='\0';
#endif
#ifdef PRIVATE_LOG_PATH
    if (private_log_file == NULL)
        private_log_file = fopen(PRIVATE_LOG_PATH,"a");
    if (private_log_file != NULL) {
        fprintf(private_log_file,"SimpleVariableTest::%s<%p>: %s\n",component->instanceName,component,buffer);
        fflush(private_log_file);
    }
#endif
#ifdef PUBLIC_LOGGING
    if (component->loggingOn) {
        size_t i;
        int active = component->nCategories == 0;
        for (i=0;i<component->nCategories;i++) {
            if (0==strcmp(category,component->loggingCategories[i])) {
                active = 1;
                break;
            }
        }
        if (active)
            component->functions.logMessage(component->functions.instanceEnvironment,component->instanceName,fmi3OK,category,buffer);
    }
#endif
#endif
}

void fmi_verbose_log(SimpleVariableTest component,const char* format, ...)
{
#if defined(VERBOSE_FMI_LOGGING) && (defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING))
    va_list ap;
    va_start(ap, format);
    internal_log(component,"FMI",format,ap);
    va_end(ap);
#endif
}

/* Normal Logging */
void normal_log(SimpleVariableTest component, const char* category, const char* format, ...) {
#if defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING)
    va_list ap;
    va_start(ap, format);
    internal_log(component,category,format,ap);
    va_end(ap);
#endif
}
