/*
 * PMSF FMU Framework for FMI 3.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2020 PMSF IT Consulting Pierre R. Mai
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

#ifdef _MSC_VER
#define myrecalloc _recalloc
#else
#define myrecalloc(ptr,num,size) realloc(ptr,num*size)
#endif

#if defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING)
#include <stdio.h>
#endif

#ifndef FMU_SHARED_OBJECT
#define FMI3_FUNCTION_PREFIX FMU_MODEL_IDENTIFIER ## _
#endif
#include "fmi3Functions.h"

typedef fmi3Byte* my3Binary;
typedef fmi3Char* my3String;

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

#define FMI_FLOAT64_TIME_VR         0
#define FMI_UINT64_X_SIZE_VR        1
#define FMI_UINT64_Y_SIZE_VR        2
#define FMI_FLOAT64_PARAMETER_VR    3
#define FMI_FLOAT64_INPUT_VR        4
#define FMI_FLOAT64_OUTPUT_VR       5

/* FMU Instance */
typedef struct DynamicArrayTest {
    /* Members */
    my3String instanceName;
    my3String instantiationToken;
    my3String resourceLocation;
    fmi3Boolean visible;
    fmi3Boolean loggingOn;
    fmi3Boolean eventModeUsed;
    fmi3Boolean earlyReturnAllowed;
    size_t nCategories;
    char** loggingCategories;
    fmi3CallbackFunctionsVar functions;
    fmi3UInt64 x_dimension_size;
    fmi3UInt64 y_dimension_size;
    fmi3Float64* float64_parameter;
    fmi3Float64* float64_input;
    fmi3Float64* float64_output;
    double last_time;
    fmi3Boolean init_mode;
    fmi3Boolean reconfiguration_mode;
} *DynamicArrayTest;
