/*
 * PMSF FMU Framework for FMI 3.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2020 PMSF IT Consulting Pierre R. Mai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "SimpleVariableTest.h"

/*
 * Debug Breaks
 *
 * If you define DEBUGBREAKS the FMU will automatically break
 * into an attached Debugger on all major computation functions.
 * Note that the FMU is likely to break all environments if no
 * Debugger is actually attached when the breaks are triggered.
 */
#if defined(DEBUGBREAKS) && !defined(NDEBUG)
#if defined(__has_builtin) && !defined(__ibmxl__)
#if __has_builtin(__builtin_debugtrap)
#define DEBUGBREAK() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define DEBUGBREAK() __debugbreak()
#endif
#endif
#if !defined(DEBUGBREAK)
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#include <intrin.h>
#define DEBUGBREAK() __debugbreak()
#else
#include <signal.h>
#if defined(SIGTRAP)
#define DEBUGBREAK() raise(SIGTRAP)
#else
#define DEBUGBREAK() raise(SIGABRT)
#endif
#endif
#endif
#else
#define DEBUGBREAK()
#endif

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

/*
 * Actual Core Content
 */

#define FMI_BOOLEAN_XOR(a,b) ((a) ? (!(b)) : (b))

fmi3Status doInit(SimpleVariableTest component)
{
    int i;

    DEBUGBREAK();

    /* Booleans */
    for (i = 0; i<FMI_BOOLEAN_VARS; i++) {
        switch (i) {
            case FMI_BOOLEAN_BOOLEANINPUT_IDX:
            case FMI_BOOLEAN_BOOLEANCONSTANT_IDX:
            case FMI_BOOLEAN_BOOLEANPARAMETER_IDX:
                component->boolean_vars[i] = fmi3True;
                break;
            case FMI_BOOLEAN_BOOLEANOUTPUT_IDX:
            case FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX:
                component->boolean_vars[i] = fmi3False;
                break;
        }
    }

    /* Integers */
    for (i = 0; i<FMI_UINT64_VARS; i++) {
        switch (i) {
            case FMI_UINT64_UINT64INPUT_IDX:
                component->uint64_vars[i] = 1;
                break;
            case FMI_UINT64_UINT64OUTPUT_IDX:
                component->uint64_vars[i] = 4;
                break;
            case FMI_UINT64_UINT64CONSTANT_IDX:
                component->uint64_vars[i] = 5;
                break;
            case FMI_UINT64_UINT64PARAMETER_IDX:
                component->uint64_vars[i] = 4;
                break;
            case FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX:
                component->uint64_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_INT64_VARS; i++) {
        switch (i) {
            case FMI_INT64_INT64INPUT_IDX:
                component->int64_vars[i] = 1;
                break;
            case FMI_INT64_INT64OUTPUT_IDX:
                component->int64_vars[i] = 4;
                break;
            case FMI_INT64_INT64CONSTANT_IDX:
                component->int64_vars[i] = 5;
                break;
            case FMI_INT64_INT64PARAMETER_IDX:
                component->int64_vars[i] = 4;
                break;
            case FMI_INT64_INT64CALCULATEDPARAMETER_IDX:
                component->int64_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_UINT32_VARS; i++) {
        switch (i) {
            case FMI_UINT32_UINT32INPUT_IDX:
                component->uint32_vars[i] = 1;
                break;
            case FMI_UINT32_UINT32OUTPUT_IDX:
                component->uint32_vars[i] = 4;
                break;
            case FMI_UINT32_UINT32CONSTANT_IDX:
                component->uint32_vars[i] = 5;
                break;
            case FMI_UINT32_UINT32PARAMETER_IDX:
                component->uint32_vars[i] = 4;
                break;
            case FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX:
                component->uint32_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_INT32_VARS; i++) {
        switch (i) {
            case FMI_INT32_INT32INPUT_IDX:
                component->int32_vars[i] = 1;
                break;
            case FMI_INT32_INT32OUTPUT_IDX:
                component->int32_vars[i] = 4;
                break;
            case FMI_INT32_INT32CONSTANT_IDX:
                component->int32_vars[i] = 5;
                break;
            case FMI_INT32_INT32PARAMETER_IDX:
                component->int32_vars[i] = 4;
                break;
            case FMI_INT32_INT32CALCULATEDPARAMETER_IDX:
                component->int32_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_UINT16_VARS; i++) {
        switch (i) {
            case FMI_UINT16_UINT16INPUT_IDX:
                component->uint16_vars[i] = 1;
                break;
            case FMI_UINT16_UINT16OUTPUT_IDX:
                component->uint16_vars[i] = 4;
                break;
            case FMI_UINT16_UINT16CONSTANT_IDX:
                component->uint16_vars[i] = 5;
                break;
            case FMI_UINT16_UINT16PARAMETER_IDX:
                component->uint16_vars[i] = 4;
                break;
            case FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX:
                component->uint16_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_INT16_VARS; i++) {
        switch (i) {
            case FMI_INT16_INT16INPUT_IDX:
                component->int16_vars[i] = 1;
                break;
            case FMI_INT16_INT16OUTPUT_IDX:
                component->int16_vars[i] = 4;
                break;
            case FMI_INT16_INT16CONSTANT_IDX:
                component->int16_vars[i] = 5;
                break;
            case FMI_INT16_INT16PARAMETER_IDX:
                component->int16_vars[i] = 4;
                break;
            case FMI_INT16_INT16CALCULATEDPARAMETER_IDX:
                component->int16_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_UINT8_VARS; i++) {
        switch (i) {
            case FMI_UINT8_UINT8INPUT_IDX:
                component->uint8_vars[i] = 1;
                break;
            case FMI_UINT8_UINT8OUTPUT_IDX:
                component->uint8_vars[i] = 4;
                break;
            case FMI_UINT8_UINT8CONSTANT_IDX:
                component->uint8_vars[i] = 5;
                break;
            case FMI_UINT8_UINT8PARAMETER_IDX:
                component->uint8_vars[i] = 4;
                break;
            case FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX:
                component->uint8_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_INT8_VARS; i++) {
        switch (i) {
            case FMI_INT8_INT8INPUT_IDX:
                component->int8_vars[i] = 1;
                break;
            case FMI_INT8_INT8OUTPUT_IDX:
                component->int8_vars[i] = 4;
                break;
            case FMI_INT8_INT8CONSTANT_IDX:
                component->int8_vars[i] = 5;
                break;
            case FMI_INT8_INT8PARAMETER_IDX:
                component->int8_vars[i] = 4;
                break;
            case FMI_INT8_INT8CALCULATEDPARAMETER_IDX:
                component->int8_vars[i] = 20;
                break;
        }
    }

    /* Reals */
    for (i = 0; i<FMI_FLOAT64_VARS; i++) {
        switch (i) {
            case FMI_FLOAT64_TIME_IDX:
                component->float64_vars[i] = 0.0;
                break;
            case FMI_FLOAT64_FLOAT64INPUT_IDX:
                component->float64_vars[i] = 1;
                break;
            case FMI_FLOAT64_FLOAT64OUTPUT_IDX:
                component->float64_vars[i] = 4;
                break;
            case FMI_FLOAT64_FLOAT64CONSTANT_IDX:
                component->float64_vars[i] = 5;
                break;
            case FMI_FLOAT64_FLOAT64PARAMETER_IDX:
                component->float64_vars[i] = 4;
                break;
            case FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX:
                component->float64_vars[i] = 20;
                break;
        }
    }
    for (i = 0; i<FMI_FLOAT32_VARS; i++) {
        switch (i) {
            case FMI_FLOAT32_FLOAT32INPUT_IDX:
                component->float32_vars[i] = 1;
                break;
            case FMI_FLOAT32_FLOAT32OUTPUT_IDX:
                component->float32_vars[i] = 4;
                break;
            case FMI_FLOAT32_FLOAT32CONSTANT_IDX:
                component->float32_vars[i] = 5;
                break;
            case FMI_FLOAT32_FLOAT32PARAMETER_IDX:
                component->float32_vars[i] = 4;
                break;
            case FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX:
                component->float32_vars[i] = 20;
                break;
        }
    }

    /* Strings */
    for (i = 0; i<FMI_STRING_VARS; i++) {
        switch (i) {
            case FMI_STRING_STRINGINPUT_IDX:
                component->string_vars[i] = strdup("abcd");
                break;
            case FMI_STRING_STRINGOUTPUT_IDX:
                component->string_vars[i] = strdup("abcdefgh");
                break;
            case FMI_STRING_STRINGCONSTANT_IDX:
                component->string_vars[i] = strdup("ijkl");
                break;
            case FMI_STRING_STRINGPARAMETER_IDX:
                component->string_vars[i] = strdup("efgh");
                break;
            case FMI_STRING_STRINGCALCULATEDPARAMETER_IDX:
                component->string_vars[i] = strdup("efghijkl");
                break;
        }
    }

    /* Binaries */
    for (i = 0; i<FMI_BINARY_VARS; i++) {
        switch (i) {
            case FMI_BINARY_BINARYCONSTANT_IDX:
                component->binary_vars[i] = strdup("\xBE\xEF");
                component->binary_sizes[i] = strlen(component->binary_vars[i]);
                break;
            case FMI_BINARY_BINARYPARAMETER_IDX:
                component->binary_vars[i] = strdup("\x1D\xE4");
                component->binary_sizes[i] = strlen(component->binary_vars[i]);
                break;
            case FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX:
                component->binary_vars[i] = strdup("\xA3\x0B");
                component->binary_sizes[i] = strlen(component->binary_vars[i]);
                break;
            default:
                component->binary_vars[i] = NULL;
                component->binary_sizes[i] = 0;
                break;
        }
    }

    return fmi3OK;
}

fmi3Status doInitCalc(SimpleVariableTest component)
{
    DEBUGBREAK();

    component->boolean_vars[FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX] =
        FMI_BOOLEAN_XOR(component->boolean_vars[FMI_BOOLEAN_BOOLEANPARAMETER_IDX],component->boolean_vars[FMI_BOOLEAN_BOOLEANCONSTANT_IDX]);

    component->uint64_vars[FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX] =
        component->uint64_vars[FMI_UINT64_UINT64PARAMETER_IDX] * component->uint64_vars[FMI_UINT64_UINT64CONSTANT_IDX];
    component->int64_vars[FMI_INT64_INT64CALCULATEDPARAMETER_IDX] =
        component->int64_vars[FMI_INT64_INT64PARAMETER_IDX] * component->int64_vars[FMI_INT64_INT64CONSTANT_IDX];
    component->uint32_vars[FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX] =
        component->uint32_vars[FMI_UINT32_UINT32PARAMETER_IDX] * component->uint32_vars[FMI_UINT32_UINT32CONSTANT_IDX];
    component->int32_vars[FMI_INT32_INT32CALCULATEDPARAMETER_IDX] =
        component->int32_vars[FMI_INT32_INT32PARAMETER_IDX] * component->int32_vars[FMI_INT32_INT32CONSTANT_IDX];
    component->uint16_vars[FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX] =
        component->uint16_vars[FMI_UINT16_UINT16PARAMETER_IDX] * component->uint16_vars[FMI_UINT16_UINT16CONSTANT_IDX];
    component->int16_vars[FMI_INT16_INT16CALCULATEDPARAMETER_IDX] =
        component->int16_vars[FMI_INT16_INT16PARAMETER_IDX] * component->int16_vars[FMI_INT16_INT16CONSTANT_IDX];
    component->uint8_vars[FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX] =
        component->uint8_vars[FMI_UINT8_UINT8PARAMETER_IDX] * component->uint8_vars[FMI_UINT8_UINT8CONSTANT_IDX];
    component->int8_vars[FMI_INT8_INT8CALCULATEDPARAMETER_IDX] =
        component->int8_vars[FMI_INT8_INT8PARAMETER_IDX] * component->int8_vars[FMI_INT8_INT8CONSTANT_IDX];

    component->float64_vars[FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX] =
        component->float64_vars[FMI_FLOAT64_FLOAT64PARAMETER_IDX] * component->float64_vars[FMI_FLOAT64_FLOAT64CONSTANT_IDX];
    component->float32_vars[FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX] =
        component->float32_vars[FMI_FLOAT32_FLOAT32PARAMETER_IDX] * component->float32_vars[FMI_FLOAT32_FLOAT32CONSTANT_IDX];

    {
        my3String buffer;
        size_t buffer_length = 1;
        buffer_length += strlen(component->string_vars[FMI_STRING_STRINGPARAMETER_IDX]);
        buffer_length += strlen(component->string_vars[FMI_STRING_STRINGCONSTANT_IDX]);
        free(component->string_vars[FMI_STRING_STRINGCALCULATEDPARAMETER_IDX]);
        buffer = malloc(buffer_length);
        strcpy(buffer,component->string_vars[FMI_STRING_STRINGPARAMETER_IDX]);
        strcat(buffer,component->string_vars[FMI_STRING_STRINGCONSTANT_IDX]);
        component->string_vars[FMI_STRING_STRINGCALCULATEDPARAMETER_IDX] = buffer;
    }

    free(component->binary_vars[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]);
    component->binary_sizes[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]=component->binary_sizes[FMI_BINARY_BINARYPARAMETER_IDX];
    if (component->binary_sizes[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]) {
        component->binary_vars[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]=malloc(component->binary_sizes[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]);
        if (component->binary_sizes[FMI_BINARY_BINARYCONSTANT_IDX]) {
            fmi3Binary input = component->binary_vars[FMI_BINARY_BINARYPARAMETER_IDX];
            fmi3Binary block = component->binary_vars[FMI_BINARY_BINARYCONSTANT_IDX];
            fmi3Byte* output = component->binary_vars[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX];
            size_t blocklen = component->binary_sizes[FMI_BINARY_BINARYCONSTANT_IDX];
            size_t outputlen = component->binary_sizes[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX];
            size_t i,j;
            for (i=0,j=0;i<outputlen;i++,j=(j+1)%blocklen)
                output[i] = input[i] ^ block[j];
        } else
            memcpy(component->binary_vars[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX],component->binary_vars[FMI_BINARY_BINARYPARAMETER_IDX],component->binary_sizes[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]);
    } else
        component->binary_vars[FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX]=NULL;

    return fmi3OK;
}

fmi3Status doEnterInitializationMode(SimpleVariableTest component, fmi3Boolean toleranceDefined, fmi3Float64 tolerance, fmi3Float64 startTime, fmi3Boolean stopTimeDefined, fmi3Float64 stopTime)
{
    component->last_time = startTime;
    component->float64_vars[FMI_FLOAT64_TIME_IDX] = component->last_time;
    component->init_mode = fmi3True;
    doInitCalc(component);
    return fmi3OK;
}

fmi3Status doExitInitializationMode(SimpleVariableTest component)
{
    component->init_mode = fmi3False;
    return fmi3OK;
}

fmi3Status doCalc(SimpleVariableTest component, fmi3Float64 currentCommunicationPoint, fmi3Float64 communicationStepSize, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean* eventEncountered, fmi3Boolean* terminateSimulation, fmi3Boolean* earlyReturn, fmi3Float64* lastSuccessfulTime)
{
    DEBUGBREAK();

    doInitCalc(component);

    component->boolean_vars[FMI_BOOLEAN_BOOLEANOUTPUT_IDX] =
        FMI_BOOLEAN_XOR(component->boolean_vars[FMI_BOOLEAN_BOOLEANPARAMETER_IDX],component->boolean_vars[FMI_BOOLEAN_BOOLEANINPUT_IDX]);

    component->uint64_vars[FMI_UINT64_UINT64OUTPUT_IDX] =
        component->uint64_vars[FMI_UINT64_UINT64PARAMETER_IDX] * component->uint64_vars[FMI_UINT64_UINT64INPUT_IDX];
    component->int64_vars[FMI_INT64_INT64OUTPUT_IDX] =
        component->int64_vars[FMI_INT64_INT64PARAMETER_IDX] * component->int64_vars[FMI_INT64_INT64INPUT_IDX];
    component->uint32_vars[FMI_UINT32_UINT32OUTPUT_IDX] =
        component->uint32_vars[FMI_UINT32_UINT32PARAMETER_IDX] * component->uint32_vars[FMI_UINT32_UINT32INPUT_IDX];
    component->int32_vars[FMI_INT32_INT32OUTPUT_IDX] =
        component->int32_vars[FMI_INT32_INT32PARAMETER_IDX] * component->int32_vars[FMI_INT32_INT32INPUT_IDX];
    component->uint16_vars[FMI_UINT16_UINT16OUTPUT_IDX] =
        component->uint16_vars[FMI_UINT16_UINT16PARAMETER_IDX] * component->uint16_vars[FMI_UINT16_UINT16INPUT_IDX];
    component->int16_vars[FMI_INT16_INT16OUTPUT_IDX] =
        component->int16_vars[FMI_INT16_INT16PARAMETER_IDX] * component->int16_vars[FMI_INT16_INT16INPUT_IDX];
    component->uint8_vars[FMI_UINT8_UINT8OUTPUT_IDX] =
        component->uint8_vars[FMI_UINT8_UINT8PARAMETER_IDX] * component->uint8_vars[FMI_UINT8_UINT8INPUT_IDX];
    component->int8_vars[FMI_INT8_INT8OUTPUT_IDX] =
        component->int8_vars[FMI_INT8_INT8PARAMETER_IDX] * component->int8_vars[FMI_INT8_INT8INPUT_IDX];

    component->float64_vars[FMI_FLOAT64_FLOAT64OUTPUT_IDX] =
        component->float64_vars[FMI_FLOAT64_FLOAT64PARAMETER_IDX] * component->float64_vars[FMI_FLOAT64_FLOAT64INPUT_IDX];
    component->float32_vars[FMI_FLOAT32_FLOAT32OUTPUT_IDX] =
        component->float32_vars[FMI_FLOAT32_FLOAT32PARAMETER_IDX] * component->float32_vars[FMI_FLOAT32_FLOAT32INPUT_IDX];

    {
        my3String buffer;
        size_t buffer_length = 1;
        buffer_length += strlen(component->string_vars[FMI_STRING_STRINGINPUT_IDX]);
        buffer_length += strlen(component->string_vars[FMI_STRING_STRINGPARAMETER_IDX]);
        free(component->string_vars[FMI_STRING_STRINGOUTPUT_IDX]);
        buffer = malloc(buffer_length);
        strcpy(buffer,component->string_vars[FMI_STRING_STRINGINPUT_IDX]);
        strcat(buffer,component->string_vars[FMI_STRING_STRINGPARAMETER_IDX]);
        component->string_vars[FMI_STRING_STRINGOUTPUT_IDX] = buffer;
    }

    free(component->binary_vars[FMI_BINARY_BINARYOUTPUT_IDX]);
    component->binary_sizes[FMI_BINARY_BINARYOUTPUT_IDX]=component->binary_sizes[FMI_BINARY_BINARYINPUT_IDX];
    if (component->binary_sizes[FMI_BINARY_BINARYOUTPUT_IDX]) {
        component->binary_vars[FMI_BINARY_BINARYOUTPUT_IDX]=malloc(component->binary_sizes[FMI_BINARY_BINARYOUTPUT_IDX]);
        memcpy(component->binary_vars[FMI_BINARY_BINARYOUTPUT_IDX],component->binary_vars[FMI_BINARY_BINARYINPUT_IDX],component->binary_sizes[FMI_BINARY_BINARYOUTPUT_IDX]);
    } else
        component->binary_vars[FMI_BINARY_BINARYOUTPUT_IDX]=NULL;

    free(component->binary_vars[FMI_BINARY_XOROUTPUT_IDX]);
    component->binary_sizes[FMI_BINARY_XOROUTPUT_IDX]=component->binary_sizes[FMI_BINARY_BINARYINPUT_IDX];
    if (component->binary_sizes[FMI_BINARY_XOROUTPUT_IDX]) {
        component->binary_vars[FMI_BINARY_XOROUTPUT_IDX]=malloc(component->binary_sizes[FMI_BINARY_XOROUTPUT_IDX]);
        if (component->binary_sizes[FMI_BINARY_BINARYPARAMETER_IDX]) {
            fmi3Binary input = component->binary_vars[FMI_BINARY_BINARYINPUT_IDX];
            fmi3Binary block = component->binary_vars[FMI_BINARY_BINARYPARAMETER_IDX];
            fmi3Byte* output = component->binary_vars[FMI_BINARY_XOROUTPUT_IDX];
            size_t blocklen = component->binary_sizes[FMI_BINARY_BINARYPARAMETER_IDX];
            size_t outputlen = component->binary_sizes[FMI_BINARY_BINARYOUTPUT_IDX];
            size_t i,j;
            for (i=0,j=0;i<outputlen;i++,j=(j+1)%blocklen)
                output[i] = input[i] ^ block[j];
        } else
            memcpy(component->binary_vars[FMI_BINARY_XOROUTPUT_IDX],component->binary_vars[FMI_BINARY_BINARYINPUT_IDX],component->binary_sizes[FMI_BINARY_XOROUTPUT_IDX]);
    } else
        component->binary_vars[FMI_BINARY_XOROUTPUT_IDX]=NULL;

    component->last_time=currentCommunicationPoint+communicationStepSize;
    component->float64_vars[FMI_FLOAT64_TIME_IDX] = component->last_time;
    *lastSuccessfulTime = component->last_time;
    *eventEncountered = fmi3False;
    *earlyReturn = fmi3False;
    *terminateSimulation = fmi3False;
    return fmi3OK;
}

fmi3Status doTerm(SimpleVariableTest component)
{
    DEBUGBREAK();
    return fmi3OK;
}

void doFree(SimpleVariableTest component)
{
    size_t i;
    DEBUGBREAK();

    for (i = 0; i<FMI_STRING_VARS; i++) {
        free(component->string_vars[i]);
        component->string_vars[i]=NULL;
    }
    for (i = 0; i<FMI_BINARY_VARS; i++) {
        free(component->binary_vars[i]);
        component->binary_vars[i]=NULL;
        component->binary_sizes[i]=0;
    }
}

/*
 * FMI 3.0 Co-Simulation Interface API
 */

FMI3_Export const char* fmi3GetVersion()
{
    return fmi3Version;
}

FMI3_Export fmi3Status fmi3SetDebugLogging(fmi3Instance instance, fmi3Boolean loggingOn, size_t nCategories, const fmi3String categories[])
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3SetDebugLogging(%s)", loggingOn ? "true" : "false");
    myc->loggingOn = loggingOn ? 1 : 0;

    for (;myc->loggingCategories!=NULL && myc->nCategories>0;) free(myc->loggingCategories[--(myc->nCategories)]);
    free(myc->loggingCategories);
    myc->loggingCategories = NULL;
    myc->nCategories = 0;

    if (categories && (nCategories > 0)) {
        myc->loggingCategories = calloc(nCategories,sizeof(char*));
        if (myc->loggingCategories != NULL) {
            size_t i;
            myc->nCategories = nCategories;
            for (i=0;i<nCategories;i++) myc->loggingCategories[i]=strdup(categories[i]);
        }
    } else {
        myc->loggingCategories = calloc(2,sizeof(char*));
        if (myc->loggingCategories != NULL) {
            myc->nCategories = 2;
            myc->loggingCategories[0] = strdup("FMI");
            myc->loggingCategories[1] = strdup("BINARY");
        }
    }

    return fmi3OK;
}

/*
 * Functions for Co-Simulation
 */
FMI3_Export fmi3Instance fmi3InstantiateCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourceLocation,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    eventModeUsed,
    fmi3Boolean                    earlyReturnAllowed,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    SimpleVariableTest myc = NULL;

#ifdef FMU_TOKEN
    if (instantiationToken!=NULL && 0!=strcmp(instantiationToken,FMU_TOKEN)) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = NULL (GUID mismatch, expected %s)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed,
            FMU_TOKEN);
        return NULL;
    }
#endif

    myc = calloc(1,sizeof(struct SimpleVariableTest));

    if (myc == NULL) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = NULL (alloc failure)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed);
        return NULL;
    }

    myc->instanceName=strdup(instanceName);
    myc->instantiationToken=strdup(instantiationToken);
    myc->resourceLocation=strdup(resourceLocation);
    myc->visible=visible;
    myc->loggingOn=loggingOn;
    myc->eventModeUsed=eventModeUsed;
    myc->earlyReturnAllowed=earlyReturnAllowed;
    myc->functions.logMessage=logMessage;
    myc->functions.intermediateUpdate=intermediateUpdate;
    myc->functions.instanceEnvironment=instanceEnvironment;

    myc->last_time=0.0;

    myc->loggingCategories = calloc(2,sizeof(char*));
    if (myc->loggingCategories != NULL) {
        myc->nCategories = 2;
        myc->loggingCategories[0] = strdup("FMI");
        myc->loggingCategories[1] = strdup("BINARY");
    }

    if (doInit(myc) != fmi3OK) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = NULL (doInit failure)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed);
        free(myc->resourceLocation);
        free(myc->instantiationToken);
        free(myc->instanceName);
        for (;myc->loggingCategories!=NULL && myc->nCategories>0;) free(myc->loggingCategories[--(myc->nCategories)]);
        free(myc->loggingCategories);
        free(myc);
        return NULL;
    }
    fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = %p",
        instanceName, instantiationToken,
        (resourceLocation != NULL) ? resourceLocation : "<NULL>",
        visible, loggingOn, eventModeUsed, earlyReturnAllowed,
        myc);

    return (fmi3Instance)myc;
}

FMI3_Export fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3EnterInitializationMode(%d,%g,%g,%d,%g)", toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
    return doEnterInitializationMode(myc,toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3ExitInitializationMode()");
    return doExitInitializationMode(myc);
}

FMI3_Export fmi3Status fmi3DoStep(fmi3Instance instance,
                                  fmi3Float64 currentCommunicationPoint,
                                  fmi3Float64 communicationStepSize,
                                  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                  fmi3Boolean* eventEncountered,
                                  fmi3Boolean* terminateSimulation,
                                  fmi3Boolean* earlyReturn,
                                  fmi3Float64* lastSuccessfulTime)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3DoStep(%g,%g,%d,%p,%p,%p,%p)", currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, eventEncountered, terminateSimulation, earlyReturn, lastSuccessfulTime);
    return doCalc(myc,currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, eventEncountered, terminateSimulation, earlyReturn, lastSuccessfulTime);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Instance instance)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3Terminate()");
    return doTerm(myc);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Instance instance)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3Reset()");
    doFree(myc);
    return doInit(myc);
}

FMI3_Export void fmi3FreeInstance(fmi3Instance instance)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    fmi_verbose_log(myc,"fmi3FreeInstance()");
    doFree(myc);

    free(myc->resourceLocation);
    free(myc->instantiationToken);
    free(myc->instanceName);
    for (;myc->loggingCategories!=NULL && myc->nCategories>0;) free(myc->loggingCategories[--(myc->nCategories)]);
    free(myc->loggingCategories);
    free(myc);
}

/*
 * Data Exchange Functions
 */

FMI3_Export fmi3Status fmi3GetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetFloat64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT64_BASE_VR;
        if (idx<FMI_FLOAT64_VARS)
            values[i] = myc->float64_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetFloat32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT32_BASE_VR;
        if (idx<FMI_FLOAT32_VARS)
            values[i] = myc->float32_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetUInt64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT64_BASE_VR;
        if (idx<FMI_UINT64_VARS)
            values[i] = myc->uint64_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetInt64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT64_BASE_VR;
        if (idx<FMI_INT64_VARS)
            values[i] = myc->int64_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetUInt32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT32_BASE_VR;
        if (idx<FMI_UINT32_VARS)
            values[i] = myc->uint32_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetInt32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT32_BASE_VR;
        if (idx<FMI_INT32_VARS)
            values[i] = myc->int32_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetUInt16(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT16_BASE_VR;
        if (idx<FMI_UINT16_VARS)
            values[i] = myc->uint16_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetInt16(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT16_BASE_VR;
        if (idx<FMI_INT16_VARS)
            values[i] = myc->int16_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetUInt8(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT8_BASE_VR;
        if (idx<FMI_UINT8_VARS)
            values[i] = myc->uint8_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetInt8(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT8_BASE_VR;
        if (idx<FMI_INT8_VARS)
            values[i] = myc->int8_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetBoolean(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BOOLEAN_BASE_VR;
        if (idx<FMI_BOOLEAN_VARS)
            values[i] = myc->boolean_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetString(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_STRING_BASE_VR;
        if (idx<FMI_STRING_VARS)
            values[i] = myc->string_vars[idx];
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t sizes[], fmi3Binary values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    fmi_verbose_log(myc,"fmi3GetBinary(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BINARY_BASE_VR;
        if (idx<FMI_BINARY_VARS) {
            sizes[i] = myc->binary_sizes[idx];
            values[i] = myc->binary_vars[idx];
        } else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT64_BASE_VR;
        if (idx<FMI_FLOAT64_VARS)
            myc->float64_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_FLOAT64_FLOAT64PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT32_BASE_VR;
        if (idx<FMI_FLOAT32_VARS)
            myc->float32_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_FLOAT32_FLOAT32PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT64_BASE_VR;
        if (idx<FMI_UINT64_VARS)
            myc->uint64_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_UINT64_UINT64PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt64(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT64_BASE_VR;
        if (idx<FMI_INT64_VARS)
            myc->int64_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_INT64_INT64PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT32_BASE_VR;
        if (idx<FMI_UINT32_VARS)
            myc->uint32_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_UINT32_UINT32PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt32(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT32_BASE_VR;
        if (idx<FMI_INT32_VARS)
            myc->int32_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_INT32_INT32PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt16(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT16_BASE_VR;
        if (idx<FMI_UINT16_VARS)
            myc->uint16_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_UINT16_UINT16PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt16(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT16_BASE_VR;
        if (idx<FMI_INT16_VARS)
            myc->int16_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_INT16_INT16PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt8(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT8_BASE_VR;
        if (idx<FMI_UINT8_VARS)
            myc->uint8_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_UINT8_UINT8PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt8(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT8_BASE_VR;
        if (idx<FMI_INT8_VARS)
            myc->int8_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_INT8_INT8PARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBoolean(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BOOLEAN_BASE_VR;
        if (idx<FMI_BOOLEAN_VARS)
            myc->boolean_vars[idx] = values[i];
        else
            return fmi3Error;
        tuned |= (idx == FMI_BOOLEAN_BOOLEANPARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetString(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_STRING_BASE_VR;
        if (idx<FMI_STRING_VARS) {
            free(myc->string_vars[idx]);
            myc->string_vars[idx] = strdup(values[i]);
        } else
            return fmi3Error;
        tuned |= (idx == FMI_STRING_STRINGPARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t sizes[], const fmi3Binary values[], size_t nValues)
{
    SimpleVariableTest myc = (SimpleVariableTest)instance;
    size_t i;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBinary(...)");
    if (nValues != nValueReferences)
        return fmi3Error;
    for (i = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BINARY_BASE_VR;
        if (idx<FMI_BINARY_VARS) {
            free(myc->binary_vars[idx]);
            myc->binary_sizes[idx] = sizes[i];
            if (sizes[i]) {
                myc->binary_vars[idx] = malloc(sizes[i]);
                memcpy(myc->binary_vars[idx],values[i],sizes[i]);
            } else
                myc->binary_vars[idx] = NULL;
        } else
            return fmi3Error;
        tuned |= (idx == FMI_BINARY_BINARYPARAMETER_IDX);
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

/*
 * Unsupported Features (FMUState, Derivatives, Status Enquiries)
 */

FMI3_Export fmi3Status fmi3GetClock(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    fmi3Clock values[],
                                    size_t nValues)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetClock(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Clock values[],
                                    size_t nValues)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                              const fmi3ValueReference valueReferences[],
                                              size_t nValueReferences,
                                              fmi3Float64 intervals[],
                                              fmi3IntervalQualifier qualifiers[],
                                              size_t nIntervals)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               fmi3UInt64 intervalCounters[],
                                               fmi3UInt64 resolutions[],
                                               fmi3IntervalQualifier qualifiers[],
                                               size_t nIntervals)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetShiftDecimal(fmi3Instance instance,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Float64 shifts[],
                                           size_t nShifts)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetShiftFraction(fmi3Instance instance,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt64 shiftCounters[],
                                            fmi3UInt64 resolutions[],
                                            size_t nShifts)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetIntervalDecimal(fmi3Instance instance,
                                              const fmi3ValueReference valueReferences[],
                                              size_t nValueReferences,
                                              const fmi3Float64 intervals[],
                                              size_t nIntervals)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetIntervalFraction(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3UInt64 intervalCounters[],
                                               const fmi3UInt64 resolutions[],
                                               size_t nIntervals)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3UpdateDiscreteStates(fmi3Instance instance,
                                                fmi3Boolean* discreteStatesNeedUpdate,
                                                fmi3Boolean* terminateSimulation,
                                                fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                fmi3Boolean* valuesOfContinuousStatesChanged,
                                                fmi3Boolean* nextEventTimeDefined,
                                                fmi3Float64* nextEventTime)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3EnterEventMode(fmi3Instance instance,
                                          fmi3Boolean stepEvent,
                                          fmi3Boolean stateEvent,
                                          const fmi3Int32 rootsFound[],
                                          size_t nEventIndicators,
                                          fmi3Boolean timeEvent)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3EnterStepMode(fmi3Instance instance)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance, fmi3ValueReference valueReference, size_t* nDependencies)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                                   fmi3ValueReference dependent,
                                                   size_t elementIndicesOfDependent[],
                                                   fmi3ValueReference independents[],
                                                   size_t elementIndicesOfIndependents[],
                                                   fmi3DependencyKind dependencyKinds[],
                                                   size_t nDependencies)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetFMUState(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetFMUState(fmi3Instance instance, fmi3FMUState FMUState)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3FreeFMUState(fmi3Instance instance, fmi3FMUState* FMUState)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance, fmi3FMUState FMUState, size_t *size)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SerializeFMUState (fmi3Instance instance, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3DeSerializeFMUState (fmi3Instance instance, const fmi3Byte serializedState[], size_t size, fmi3FMUState* FMUState)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetDirectionalDerivative(fmi3Instance instance,
                                                    const fmi3ValueReference unknowns[],
                                                    size_t nUnknowns,
                                                    const fmi3ValueReference knowns[],
                                                    size_t nKnowns,
                                                    const fmi3Float64 seed[],
                                                    size_t nSeed,
                                                    fmi3Float64 sensitivity[],
                                                    size_t nSensitivity)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetAdjointDerivative(fmi3Instance instance,
                                                const fmi3ValueReference unknowns[],
                                                size_t nUnknowns,
                                                const fmi3ValueReference knowns[],
                                                size_t nKnowns,
                                                const fmi3Float64 seed[],
                                                size_t nSeed,
                                                fmi3Float64 sensitivity[],
                                                size_t nSensitivity)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                const fmi3Int32 orders[],
                                                fmi3Float64 values[],
                                                size_t nValues)
{
    return fmi3Error;
}
