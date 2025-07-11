/*
 * PMSF FMU Framework for FMI 3.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2025 PMSF IT Consulting Pierre R. Mai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef _MSC_VER
#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

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
 * used internally and the publicly visible ValueReference, allowing
 * e.g. to have globally unique VRs (which are not required for either
 * FMI 1.0 or 2.0, but are for FMI 3.0 support).
 */

typedef fmi3Byte* my3Binary;
typedef fmi3Char* my3String;

/* Boolean Variables */
#define FMI_BOOLEAN_BASE_VR 0
#define FMI_BOOLEAN_BOOLEANINPUT_IDX 0
#define FMI_BOOLEAN_BOOLEANOUTPUT_IDX 1
#define FMI_BOOLEAN_BOOLEANCONSTANT_IDX 2
#define FMI_BOOLEAN_BOOLEANPARAMETER_IDX 3
#define FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX 4
#define FMI_BOOLEAN_LAST_IDX FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX
#define FMI_BOOLEAN_VARS (FMI_BOOLEAN_LAST_IDX+1)

/* Integer Variables */
#define FMI_UINT64_BASE_VR (FMI_BOOLEAN_BASE_VR + FMI_BOOLEAN_VARS)
#define FMI_UINT64_UINT64INPUT_IDX 0
#define FMI_UINT64_UINT64OUTPUT_IDX 1
#define FMI_UINT64_UINT64CONSTANT_IDX 2
#define FMI_UINT64_UINT64PARAMETER_IDX 3
#define FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX 4
#define FMI_UINT64_LAST_IDX FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX
#define FMI_UINT64_VARS (FMI_UINT64_LAST_IDX+1)

#define FMI_INT64_BASE_VR (FMI_UINT64_BASE_VR + FMI_UINT64_VARS)
#define FMI_INT64_INT64INPUT_IDX 0
#define FMI_INT64_INT64OUTPUT_IDX 1
#define FMI_INT64_INT64CONSTANT_IDX 2
#define FMI_INT64_INT64PARAMETER_IDX 3
#define FMI_INT64_INT64CALCULATEDPARAMETER_IDX 4
#define FMI_INT64_LAST_IDX FMI_INT64_INT64CALCULATEDPARAMETER_IDX
#define FMI_INT64_VARS (FMI_INT64_LAST_IDX+1)

#define FMI_UINT32_BASE_VR (FMI_INT64_BASE_VR + FMI_INT64_VARS)
#define FMI_UINT32_UINT32INPUT_IDX 0
#define FMI_UINT32_UINT32OUTPUT_IDX 1
#define FMI_UINT32_UINT32CONSTANT_IDX 2
#define FMI_UINT32_UINT32PARAMETER_IDX 3
#define FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX 4
#define FMI_UINT32_LAST_IDX FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX
#define FMI_UINT32_VARS (FMI_UINT32_LAST_IDX+1)

#define FMI_INT32_BASE_VR (FMI_UINT32_BASE_VR + FMI_UINT32_VARS)
#define FMI_INT32_INT32INPUT_IDX 0
#define FMI_INT32_INT32OUTPUT_IDX 1
#define FMI_INT32_INT32CONSTANT_IDX 2
#define FMI_INT32_INT32PARAMETER_IDX 3
#define FMI_INT32_INT32CALCULATEDPARAMETER_IDX 4
#define FMI_INT32_LAST_IDX FMI_INT32_INT32CALCULATEDPARAMETER_IDX
#define FMI_INT32_VARS (FMI_INT32_LAST_IDX+1)

#define FMI_UINT16_BASE_VR (FMI_INT32_BASE_VR + FMI_INT32_VARS)
#define FMI_UINT16_UINT16INPUT_IDX 0
#define FMI_UINT16_UINT16OUTPUT_IDX 1
#define FMI_UINT16_UINT16CONSTANT_IDX 2
#define FMI_UINT16_UINT16PARAMETER_IDX 3
#define FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX 4
#define FMI_UINT16_LAST_IDX FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX
#define FMI_UINT16_VARS (FMI_UINT16_LAST_IDX+1)

#define FMI_INT16_BASE_VR (FMI_UINT16_BASE_VR + FMI_UINT16_VARS)
#define FMI_INT16_INT16INPUT_IDX 0
#define FMI_INT16_INT16OUTPUT_IDX 1
#define FMI_INT16_INT16CONSTANT_IDX 2
#define FMI_INT16_INT16PARAMETER_IDX 3
#define FMI_INT16_INT16CALCULATEDPARAMETER_IDX 4
#define FMI_INT16_LAST_IDX FMI_INT16_INT16CALCULATEDPARAMETER_IDX
#define FMI_INT16_VARS (FMI_INT16_LAST_IDX+1)

#define FMI_UINT8_BASE_VR (FMI_INT16_BASE_VR + FMI_INT16_VARS)
#define FMI_UINT8_UINT8INPUT_IDX 0
#define FMI_UINT8_UINT8OUTPUT_IDX 1
#define FMI_UINT8_UINT8CONSTANT_IDX 2
#define FMI_UINT8_UINT8PARAMETER_IDX 3
#define FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX 4
#define FMI_UINT8_LAST_IDX FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX
#define FMI_UINT8_VARS (FMI_UINT8_LAST_IDX+1)

#define FMI_INT8_BASE_VR (FMI_UINT8_BASE_VR + FMI_UINT8_VARS)
#define FMI_INT8_INT8INPUT_IDX 0
#define FMI_INT8_INT8OUTPUT_IDX 1
#define FMI_INT8_INT8CONSTANT_IDX 2
#define FMI_INT8_INT8PARAMETER_IDX 3
#define FMI_INT8_INT8CALCULATEDPARAMETER_IDX 4
#define FMI_INT8_LAST_IDX FMI_INT8_INT8CALCULATEDPARAMETER_IDX
#define FMI_INT8_VARS (FMI_INT8_LAST_IDX+1)

/* Real Variables */
#define FMI_FLOAT64_BASE_VR (FMI_INT8_BASE_VR + FMI_INT8_VARS)
#define FMI_FLOAT64_TIME_IDX 0
#define FMI_FLOAT64_FLOAT64INPUT_IDX 1
#define FMI_FLOAT64_FLOAT64OUTPUT_IDX 2
#define FMI_FLOAT64_FLOAT64CONSTANT_IDX 3
#define FMI_FLOAT64_FLOAT64PARAMETER_IDX 4
#define FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX 5
#define FMI_FLOAT64_LAST_IDX FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX
#define FMI_FLOAT64_VARS (FMI_FLOAT64_LAST_IDX+1)

#define FMI_FLOAT32_BASE_VR (FMI_FLOAT64_BASE_VR + FMI_FLOAT64_VARS)
#define FMI_FLOAT32_FLOAT32INPUT_IDX 0
#define FMI_FLOAT32_FLOAT32OUTPUT_IDX 1
#define FMI_FLOAT32_FLOAT32CONSTANT_IDX 2
#define FMI_FLOAT32_FLOAT32PARAMETER_IDX 3
#define FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX 4
#define FMI_FLOAT32_LAST_IDX FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX
#define FMI_FLOAT32_VARS (FMI_FLOAT32_LAST_IDX+1)

/* String Variables */
#define FMI_STRING_BASE_VR (FMI_FLOAT32_BASE_VR + FMI_FLOAT32_VARS)
#define FMI_STRING_STRINGINPUT_IDX 0
#define FMI_STRING_STRINGOUTPUT_IDX 1
#define FMI_STRING_STRINGCONSTANT_IDX 2
#define FMI_STRING_STRINGPARAMETER_IDX 3
#define FMI_STRING_STRINGCALCULATEDPARAMETER_IDX 4
#define FMI_STRING_LAST_IDX FMI_STRING_STRINGCALCULATEDPARAMETER_IDX
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
    fmi3LogMessageCallback         logMessage;
    fmi3IntermediateUpdateCallback intermediateUpdate;
} fmi3CallbackFunctionsVar;

/* FMU Instance */
typedef struct SimpleArrayTest {
    /* Members */
    my3String instanceName;
    my3String instantiationToken;
    my3String resourcePath;
    fmi3Boolean visible;
    fmi3Boolean loggingOn;
    fmi3Boolean eventModeUsed;
    fmi3Boolean earlyReturnAllowed;
    size_t nCategories;
    char** loggingCategories;
    fmi3CallbackFunctionsVar functions;
    fmi3Boolean boolean_vars[FMI_BOOLEAN_VARS][2][3];
    fmi3UInt64 uint64_vars[FMI_UINT64_VARS][2][3];
    fmi3Int64 int64_vars[FMI_INT64_VARS][2][3];
    fmi3UInt32 uint32_vars[FMI_UINT32_VARS][2][3];
    fmi3Int32 int32_vars[FMI_INT32_VARS][2][3];
    fmi3UInt16 uint16_vars[FMI_UINT16_VARS][2][3];
    fmi3Int16 int16_vars[FMI_INT16_VARS][2][3];
    fmi3UInt8 uint8_vars[FMI_UINT8_VARS][2][3];
    fmi3Int8 int8_vars[FMI_INT8_VARS][2][3];
    fmi3Float64 float64_vars[FMI_FLOAT64_VARS][2][3];
    fmi3Float32 float32_vars[FMI_FLOAT32_VARS][2][3];
    my3String string_vars[FMI_STRING_VARS][2][3];
    my3Binary binary_vars[FMI_BINARY_VARS][2][3];
    size_t binary_sizes[FMI_BINARY_VARS][2][3];
    double last_time;
    fmi3Boolean init_mode;
} *SimpleArrayTest;
