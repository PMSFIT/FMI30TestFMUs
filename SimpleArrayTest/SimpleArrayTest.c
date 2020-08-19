/*
 * PMSF FMU Framework for FMI 3.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2020 PMSF IT Consulting Pierre R. Mai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "SimpleArrayTest.h"

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

#include <stdio.h>


#ifdef PRIVATE_LOG_PATH
FILE* SimpleArrayTest_private_log_file;
#endif

#define FMI_BOOLEAN_XOR(a,b) ((a) ? (!(b)) : (b))

#define CopyOut(a,i,b) do { \
                         a[i++] = b[0][0]; \
                         a[i++] = b[0][1]; \
                         a[i++] = b[0][2]; \
                         a[i++] = b[1][0]; \
                         a[i++] = b[1][1]; \
                         a[i++] = b[1][2]; \
                       } while(0)

#define CopyIn(a,i,b) do { \
                         b[0][0] = a[i++]; \
                         b[0][1] = a[i++]; \
                         b[0][2] = a[i++]; \
                         b[1][0] = a[i++]; \
                         b[1][1] = a[i++]; \
                         b[1][2] = a[i++]; \
                       } while(0)

#define CopyInStr(a,i,b) do { \
                         b[0][0] = strdup(a[i++]); \
                         b[0][1] = strdup(a[i++]); \
                         b[0][2] = strdup(a[i++]); \
                         b[1][0] = strdup(a[i++]); \
                         b[1][1] = strdup(a[i++]); \
                         b[1][2] = strdup(a[i++]); \
                       } while(0)

#define BinDup(a,b,c,d) do { \
                         c = a; \
                         if (c) { \
                             d = malloc(c); \
                             memcpy(d,b,c); \
                         } else \
                             d = NULL; \
                       } while(0)

#define CopyInBin(a,b,i,c,d) do { \
                        BinDup(a[i],b[i],c[0][0],d[0][0]); \
                        i++; \
                        BinDup(a[i],b[i],c[0][1],d[0][1]); \
                        i++; \
                        BinDup(a[i],b[i],c[0][2],d[0][2]); \
                        i++; \
                        BinDup(a[i],b[i],c[1][0],d[1][0]); \
                        i++; \
                        BinDup(a[i],b[i],c[1][1],d[1][1]); \
                        i++; \
                        BinDup(a[i],b[i],c[1][2],d[1][2]); \
                        i++; \
                       } while(0)

#define SetAll(a,b) do { \
                      (a)[0][0] = b; \
                      (a)[0][1] = b; \
                      (a)[0][2] = b; \
                      (a)[1][0] = b; \
                      (a)[1][1] = b; \
                      (a)[1][2] = b; \
                    } while(0)

#define DoAll(a,op) do { \
                      op((a)[0][0]); \
                      op((a)[0][1]); \
                      op((a)[0][2]); \
                      op((a)[1][0]); \
                      op((a)[1][1]); \
                      op((a)[1][2]); \
                    } while(0)

#define BindoAll(a,op,b,c) do { \
                      (a)[0][0] = op((b)[0][0],(c)[0][0]); \
                      (a)[0][1] = op((b)[0][1],(c)[0][1]); \
                      (a)[0][2] = op((b)[0][2],(c)[0][2]); \
                      (a)[1][0] = op((b)[1][0],(c)[1][0]); \
                      (a)[1][1] = op((b)[1][1],(c)[1][1]); \
                      (a)[1][2] = op((b)[1][2],(c)[1][2]); \
                    } while(0)

#define BinopAll(a,b,op,c) do { \
                      (a)[0][0] = (b)[0][0] op (c)[0][0]; \
                      (a)[0][1] = (b)[0][1] op (c)[0][1]; \
                      (a)[0][2] = (b)[0][2] op (c)[0][2]; \
                      (a)[1][0] = (b)[1][0] op (c)[1][0]; \
                      (a)[1][1] = (b)[1][1] op (c)[1][1]; \
                      (a)[1][2] = (b)[1][2] op (c)[1][2]; \
                    } while(0)

#define FMI_STRING_CONCAT(a,b,c) \
    do { \
        my3String buffer; \
        size_t buffer_length = 1; \
        buffer_length += strlen(b); \
        buffer_length += strlen(c); \
        free(a); \
        buffer = malloc(buffer_length); \
        strcpy(buffer,b); \
        strcat(buffer,c); \
        a = buffer; \
    } while(0)

#define StringConcatAll(a,b,c) do { \
                      FMI_STRING_CONCAT((a)[0][0],(b)[0][0],(c)[0][0]); \
                      FMI_STRING_CONCAT((a)[0][1],(b)[0][1],(c)[0][1]); \
                      FMI_STRING_CONCAT((a)[0][2],(b)[0][2],(c)[0][2]); \
                      FMI_STRING_CONCAT((a)[1][0],(b)[1][0],(c)[1][0]); \
                      FMI_STRING_CONCAT((a)[1][1],(b)[1][1],(c)[1][1]); \
                      FMI_STRING_CONCAT((a)[1][2],(b)[1][2],(c)[1][2]); \
                    } while(0)

/*
 * Actual Core Content
 */

fmi3Status doInit(SimpleArrayTest component)
{
    int i;

    DEBUGBREAK();

    /* Booleans */
    for (i = 0; i<FMI_BOOLEAN_VARS; i++) {
        switch (i) {
            case FMI_BOOLEAN_BOOLEANINPUT_IDX:
            case FMI_BOOLEAN_BOOLEANCONSTANT_IDX:
            case FMI_BOOLEAN_BOOLEANPARAMETER_IDX:
                SetAll(component->boolean_vars[i],fmi3True);
                break;
            case FMI_BOOLEAN_BOOLEANOUTPUT_IDX:
            case FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX:
                SetAll(component->boolean_vars[i],fmi3False);
                break;
        }
    }

    /* Integers */
    for (i = 0; i<FMI_UINT64_VARS; i++) {
        switch (i) {
            case FMI_UINT64_UINT64INPUT_IDX:
                SetAll(component->uint64_vars[i],1);
                break;
            case FMI_UINT64_UINT64OUTPUT_IDX:
                SetAll(component->uint64_vars[i],4);
                break;
            case FMI_UINT64_UINT64CONSTANT_IDX:
                SetAll(component->uint64_vars[i],5);
                break;
            case FMI_UINT64_UINT64PARAMETER_IDX:
                SetAll(component->uint64_vars[i],4);
                break;
            case FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX:
                SetAll(component->uint64_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_INT64_VARS; i++) {
        switch (i) {
            case FMI_INT64_INT64INPUT_IDX:
                SetAll(component->int64_vars[i],1);
                break;
            case FMI_INT64_INT64OUTPUT_IDX:
                SetAll(component->int64_vars[i],4);
                break;
            case FMI_INT64_INT64CONSTANT_IDX:
                SetAll(component->int64_vars[i],5);
                break;
            case FMI_INT64_INT64PARAMETER_IDX:
                SetAll(component->int64_vars[i],4);
                break;
            case FMI_INT64_INT64CALCULATEDPARAMETER_IDX:
                SetAll(component->int64_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_UINT32_VARS; i++) {
        switch (i) {
            case FMI_UINT32_UINT32INPUT_IDX:
                SetAll(component->uint32_vars[i],1);
                break;
            case FMI_UINT32_UINT32OUTPUT_IDX:
                SetAll(component->uint32_vars[i],4);
                break;
            case FMI_UINT32_UINT32CONSTANT_IDX:
                SetAll(component->uint32_vars[i],5);
                break;
            case FMI_UINT32_UINT32PARAMETER_IDX:
                SetAll(component->uint32_vars[i],4);
                break;
            case FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX:
                SetAll(component->uint32_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_INT32_VARS; i++) {
        switch (i) {
            case FMI_INT32_INT32INPUT_IDX:
                SetAll(component->int32_vars[i],1);
                break;
            case FMI_INT32_INT32OUTPUT_IDX:
                SetAll(component->int32_vars[i],4);
                break;
            case FMI_INT32_INT32CONSTANT_IDX:
                SetAll(component->int32_vars[i],5);
                break;
            case FMI_INT32_INT32PARAMETER_IDX:
                SetAll(component->int32_vars[i],4);
                break;
            case FMI_INT32_INT32CALCULATEDPARAMETER_IDX:
                SetAll(component->int32_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_UINT16_VARS; i++) {
        switch (i) {
            case FMI_UINT16_UINT16INPUT_IDX:
                SetAll(component->uint16_vars[i],1);
                break;
            case FMI_UINT16_UINT16OUTPUT_IDX:
                SetAll(component->uint16_vars[i],4);
                break;
            case FMI_UINT16_UINT16CONSTANT_IDX:
                SetAll(component->uint16_vars[i],5);
                break;
            case FMI_UINT16_UINT16PARAMETER_IDX:
                SetAll(component->uint16_vars[i],4);
                break;
            case FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX:
                SetAll(component->uint16_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_INT16_VARS; i++) {
        switch (i) {
            case FMI_INT16_INT16INPUT_IDX:
                SetAll(component->int16_vars[i],1);
                break;
            case FMI_INT16_INT16OUTPUT_IDX:
                SetAll(component->int16_vars[i],4);
                break;
            case FMI_INT16_INT16CONSTANT_IDX:
                SetAll(component->int16_vars[i],5);
                break;
            case FMI_INT16_INT16PARAMETER_IDX:
                SetAll(component->int16_vars[i],4);
                break;
            case FMI_INT16_INT16CALCULATEDPARAMETER_IDX:
                SetAll(component->int16_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_UINT8_VARS; i++) {
        switch (i) {
            case FMI_UINT8_UINT8INPUT_IDX:
                SetAll(component->uint8_vars[i],1);
                break;
            case FMI_UINT8_UINT8OUTPUT_IDX:
                SetAll(component->uint8_vars[i],4);
                break;
            case FMI_UINT8_UINT8CONSTANT_IDX:
                SetAll(component->uint8_vars[i],5);
                break;
            case FMI_UINT8_UINT8PARAMETER_IDX:
                SetAll(component->uint8_vars[i],4);
                break;
            case FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX:
                SetAll(component->uint8_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_INT8_VARS; i++) {
        switch (i) {
            case FMI_INT8_INT8INPUT_IDX:
                SetAll(component->int8_vars[i],1);
                break;
            case FMI_INT8_INT8OUTPUT_IDX:
                SetAll(component->int8_vars[i],4);
                break;
            case FMI_INT8_INT8CONSTANT_IDX:
                SetAll(component->int8_vars[i],5);
                break;
            case FMI_INT8_INT8PARAMETER_IDX:
                SetAll(component->int8_vars[i],4);
                break;
            case FMI_INT8_INT8CALCULATEDPARAMETER_IDX:
                SetAll(component->int8_vars[i],20);
                break;
        }
    }

    /* Reals */
    for (i = 0; i<FMI_FLOAT64_VARS; i++) {
        switch (i) {
            case FMI_FLOAT64_TIME_IDX:
                SetAll(component->float64_vars[i],0.0);
                break;
            case FMI_FLOAT64_FLOAT64INPUT_IDX:
                SetAll(component->float64_vars[i],1);
                break;
            case FMI_FLOAT64_FLOAT64OUTPUT_IDX:
                SetAll(component->float64_vars[i],4);
                break;
            case FMI_FLOAT64_FLOAT64CONSTANT_IDX:
                SetAll(component->float64_vars[i],5);
                break;
            case FMI_FLOAT64_FLOAT64PARAMETER_IDX:
                SetAll(component->float64_vars[i],4);
                break;
            case FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX:
                SetAll(component->float64_vars[i],20);
                break;
        }
    }
    for (i = 0; i<FMI_FLOAT32_VARS; i++) {
        switch (i) {
            case FMI_FLOAT32_FLOAT32INPUT_IDX:
                SetAll(component->float32_vars[i],1);
                break;
            case FMI_FLOAT32_FLOAT32OUTPUT_IDX:
                SetAll(component->float32_vars[i],4);
                break;
            case FMI_FLOAT32_FLOAT32CONSTANT_IDX:
                SetAll(component->float32_vars[i],5);
                break;
            case FMI_FLOAT32_FLOAT32PARAMETER_IDX:
                SetAll(component->float32_vars[i],4);
                break;
            case FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX:
                SetAll(component->float32_vars[i],20);
                break;
        }
    }

    /* Strings */
    for (i = 0; i<FMI_STRING_VARS; i++) {
        switch (i) {
            case FMI_STRING_STRINGINPUT_IDX:
                SetAll(component->string_vars[i],strdup("abcd"));
                break;
            case FMI_STRING_STRINGOUTPUT_IDX:
                SetAll(component->string_vars[i],strdup("abcdefgh"));
                break;
            case FMI_STRING_STRINGCONSTANT_IDX:
                SetAll(component->string_vars[i],strdup("ijkl"));
                break;
            case FMI_STRING_STRINGPARAMETER_IDX:
                SetAll(component->string_vars[i],strdup("efgh"));
                break;
            case FMI_STRING_STRINGCALCULATEDPARAMETER_IDX:
                SetAll(component->string_vars[i],strdup("efghijkl"));
                break;
        }
    }

    /* Binaries */
    for (i = 0; i<FMI_BINARY_VARS; i++) {
        switch (i) {
            case FMI_BINARY_BINARYCONSTANT_IDX:
                SetAll(component->binary_vars[i],strdup("\xBE\xEF"));
                SetAll(component->binary_sizes[i],strlen(component->binary_vars[i][0][0]));
                break;
            case FMI_BINARY_BINARYPARAMETER_IDX:
                SetAll(component->binary_vars[i],strdup("\x1D\xE4"));
                SetAll(component->binary_sizes[i],strlen(component->binary_vars[i][0][0]));
                break;
            case FMI_BINARY_BINARYCALCULATEDPARAMETER_IDX:
                SetAll(component->binary_vars[i],strdup("\xA3\x0B"));
                SetAll(component->binary_sizes[i],strlen(component->binary_vars[i][0][0]));
                break;
            default:
                SetAll(component->binary_vars[i],NULL);
                SetAll(component->binary_sizes[i],0);
                break;
        }
    }

    return fmi3OK;
}

fmi3Status doInitCalc(SimpleArrayTest component)
{
    DEBUGBREAK();

    BindoAll(component->boolean_vars[FMI_BOOLEAN_BOOLEANCALCULATEDPARAMETER_IDX],
        FMI_BOOLEAN_XOR,component->boolean_vars[FMI_BOOLEAN_BOOLEANPARAMETER_IDX],component->boolean_vars[FMI_BOOLEAN_BOOLEANCONSTANT_IDX]);

    BinopAll(component->uint64_vars[FMI_UINT64_UINT64CALCULATEDPARAMETER_IDX],
        component->uint64_vars[FMI_UINT64_UINT64PARAMETER_IDX],*,component->uint64_vars[FMI_UINT64_UINT64CONSTANT_IDX]);
    BinopAll(component->int64_vars[FMI_INT64_INT64CALCULATEDPARAMETER_IDX],
        component->int64_vars[FMI_INT64_INT64PARAMETER_IDX],*,component->int64_vars[FMI_INT64_INT64CONSTANT_IDX]);
    BinopAll(component->uint32_vars[FMI_UINT32_UINT32CALCULATEDPARAMETER_IDX],
        component->uint32_vars[FMI_UINT32_UINT32PARAMETER_IDX],*,component->uint32_vars[FMI_UINT32_UINT32CONSTANT_IDX]);
    BinopAll(component->int32_vars[FMI_INT32_INT32CALCULATEDPARAMETER_IDX],
        component->int32_vars[FMI_INT32_INT32PARAMETER_IDX],*,component->int32_vars[FMI_INT32_INT32CONSTANT_IDX]);
    BinopAll(component->uint16_vars[FMI_UINT16_UINT16CALCULATEDPARAMETER_IDX],
        component->uint16_vars[FMI_UINT16_UINT16PARAMETER_IDX],*,component->uint16_vars[FMI_UINT16_UINT16CONSTANT_IDX]);
    BinopAll(component->int16_vars[FMI_INT16_INT16CALCULATEDPARAMETER_IDX],
        component->int16_vars[FMI_INT16_INT16PARAMETER_IDX],*,component->int16_vars[FMI_INT16_INT16CONSTANT_IDX]);
    BinopAll(component->uint8_vars[FMI_UINT8_UINT8CALCULATEDPARAMETER_IDX],
        component->uint8_vars[FMI_UINT8_UINT8PARAMETER_IDX],*,component->uint8_vars[FMI_UINT8_UINT8CONSTANT_IDX]);
    BinopAll(component->int8_vars[FMI_INT8_INT8CALCULATEDPARAMETER_IDX],
        component->int8_vars[FMI_INT8_INT8PARAMETER_IDX],*,component->int8_vars[FMI_INT8_INT8CONSTANT_IDX]);

    BinopAll(component->float64_vars[FMI_FLOAT64_FLOAT64CALCULATEDPARAMETER_IDX],
        component->float64_vars[FMI_FLOAT64_FLOAT64PARAMETER_IDX],*,component->float64_vars[FMI_FLOAT64_FLOAT64CONSTANT_IDX]);
    BinopAll(component->float32_vars[FMI_FLOAT32_FLOAT32CALCULATEDPARAMETER_IDX],
        component->float32_vars[FMI_FLOAT32_FLOAT32PARAMETER_IDX],*,component->float32_vars[FMI_FLOAT32_FLOAT32CONSTANT_IDX]);

    StringConcatAll(component->string_vars[FMI_STRING_STRINGCALCULATEDPARAMETER_IDX],
        component->string_vars[FMI_STRING_STRINGPARAMETER_IDX],component->string_vars[FMI_STRING_STRINGCONSTANT_IDX]);
#if 0
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
#endif
    return fmi3OK;
}

fmi3Status doEnterInitializationMode(SimpleArrayTest component, fmi3Boolean toleranceDefined, fmi3Float64 tolerance, fmi3Float64 startTime, fmi3Boolean stopTimeDefined, fmi3Float64 stopTime)
{
    component->last_time = startTime;
    SetAll(component->float64_vars[FMI_FLOAT64_TIME_IDX],component->last_time);
    component->init_mode = fmi3True;
    doInitCalc(component);
    return fmi3OK;
}

fmi3Status doExitInitializationMode(SimpleArrayTest component)
{
    component->init_mode = fmi3False;
    return fmi3OK;
}

fmi3Status doCalc(SimpleArrayTest component, fmi3Float64 currentCommunicationPoint, fmi3Float64 communicationStepSize, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean* terminate, fmi3Boolean* earlyReturn, fmi3Float64* lastSuccessfulTime)
{
    DEBUGBREAK();

    doInitCalc(component);

    BindoAll(component->boolean_vars[FMI_BOOLEAN_BOOLEANOUTPUT_IDX],
        FMI_BOOLEAN_XOR,component->boolean_vars[FMI_BOOLEAN_BOOLEANPARAMETER_IDX],component->boolean_vars[FMI_BOOLEAN_BOOLEANINPUT_IDX]);

    BinopAll(component->uint64_vars[FMI_UINT64_UINT64OUTPUT_IDX],
        component->uint64_vars[FMI_UINT64_UINT64PARAMETER_IDX],*,component->uint64_vars[FMI_UINT64_UINT64INPUT_IDX]);
    BinopAll(component->int64_vars[FMI_INT64_INT64OUTPUT_IDX],
        component->int64_vars[FMI_INT64_INT64PARAMETER_IDX],*,component->int64_vars[FMI_INT64_INT64INPUT_IDX]);
    BinopAll(component->uint32_vars[FMI_UINT32_UINT32OUTPUT_IDX],
        component->uint32_vars[FMI_UINT32_UINT32PARAMETER_IDX],*,component->uint32_vars[FMI_UINT32_UINT32INPUT_IDX]);
    BinopAll(component->int32_vars[FMI_INT32_INT32OUTPUT_IDX],
        component->int32_vars[FMI_INT32_INT32PARAMETER_IDX],*,component->int32_vars[FMI_INT32_INT32INPUT_IDX]);
    BinopAll(component->uint16_vars[FMI_UINT16_UINT16OUTPUT_IDX],
        component->uint16_vars[FMI_UINT16_UINT16PARAMETER_IDX],*,component->uint16_vars[FMI_UINT16_UINT16INPUT_IDX]);
    BinopAll(component->int16_vars[FMI_INT16_INT16OUTPUT_IDX],
        component->int16_vars[FMI_INT16_INT16PARAMETER_IDX],*,component->int16_vars[FMI_INT16_INT16INPUT_IDX]);
    BinopAll(component->uint8_vars[FMI_UINT8_UINT8OUTPUT_IDX],
        component->uint8_vars[FMI_UINT8_UINT8PARAMETER_IDX],*,component->uint8_vars[FMI_UINT8_UINT8INPUT_IDX]);
    BinopAll(component->int8_vars[FMI_INT8_INT8OUTPUT_IDX],
        component->int8_vars[FMI_INT8_INT8PARAMETER_IDX],*,component->int8_vars[FMI_INT8_INT8INPUT_IDX]);

    BinopAll(component->float64_vars[FMI_FLOAT64_FLOAT64OUTPUT_IDX],
        component->float64_vars[FMI_FLOAT64_FLOAT64PARAMETER_IDX],*,component->float64_vars[FMI_FLOAT64_FLOAT64INPUT_IDX]);
    BinopAll(component->float32_vars[FMI_FLOAT32_FLOAT32OUTPUT_IDX],
        component->float32_vars[FMI_FLOAT32_FLOAT32PARAMETER_IDX],*,component->float32_vars[FMI_FLOAT32_FLOAT32INPUT_IDX]);

    StringConcatAll(component->string_vars[FMI_STRING_STRINGOUTPUT_IDX],
        component->string_vars[FMI_STRING_STRINGINPUT_IDX],component->string_vars[FMI_STRING_STRINGPARAMETER_IDX]);
#if 0
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
#endif
    component->last_time=currentCommunicationPoint+communicationStepSize;
    SetAll(component->float64_vars[FMI_FLOAT64_TIME_IDX],component->last_time);
    *lastSuccessfulTime = component->last_time;
    *earlyReturn = fmi3False;
    *terminate = fmi3False;
    return fmi3OK;
}

fmi3Status doTerm(SimpleArrayTest component)
{
    DEBUGBREAK();
    return fmi3OK;
}

void doFree(SimpleArrayTest component)
{
    size_t i;
    DEBUGBREAK();

    for (i = 0; i<FMI_STRING_VARS; i++) {
        DoAll(component->string_vars[i],free);
        SetAll(component->string_vars[i],NULL);
    }
    for (i = 0; i<FMI_BINARY_VARS; i++) {
        DoAll(component->binary_vars[i],free);
        SetAll(component->binary_vars[i],NULL);
        SetAll(component->binary_sizes[i],0);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
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
    fmi3Boolean                    eventModeRequired,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    SimpleArrayTest myc = NULL;

#ifdef FMU_TOKEN
    if (instantiationToken!=NULL && 0!=strcmp(instantiationToken,FMU_TOKEN)) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,...) = NULL (GUID mismatch, expected %s)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeRequired,
            FMU_TOKEN);
        return NULL;
    }
#endif

    myc = calloc(1,sizeof(struct SimpleArrayTest));

    if (myc == NULL) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,...) = NULL (alloc failure)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeRequired);
        return NULL;
    }

    myc->instanceName=strdup(instanceName);
    myc->instantiationToken=strdup(instantiationToken);
    myc->resourceLocation=strdup(resourceLocation);
    myc->visible=visible;
    myc->loggingOn=loggingOn;
    myc->eventModeRequired=eventModeRequired;
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
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,...) = NULL (doInit failure)",
            instanceName, instantiationToken,
            (resourceLocation != NULL) ? resourceLocation : "<NULL>",
            visible, loggingOn, eventModeRequired);
        free(myc->resourceLocation);
        free(myc->instantiationToken);
        free(myc->instanceName);
        for (;myc->loggingCategories!=NULL && myc->nCategories>0;) free(myc->loggingCategories[--(myc->nCategories)]);
        free(myc->loggingCategories);
        free(myc);
        return NULL;
    }
    fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,...) = %p",
        instanceName, instantiationToken,
        (resourceLocation != NULL) ? resourceLocation : "<NULL>",
        visible, loggingOn, eventModeRequired,
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    fmi_verbose_log(myc,"fmi3EnterInitializationMode(%d,%g,%g,%d,%g)", toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
    return doEnterInitializationMode(myc,toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    fmi_verbose_log(myc,"fmi3ExitInitializationMode()");
    return doExitInitializationMode(myc);
}

FMI3_Export fmi3Status fmi3DoStep(fmi3Instance instance,
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* terminate,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    fmi_verbose_log(myc,"fmi3DoStep(%g,%g,%d,%p)", currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, earlyReturn);
    return doCalc(myc,currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, terminate, earlyReturn, lastSuccessfulTime);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Instance instance)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    fmi_verbose_log(myc,"fmi3Terminate()");
    return doTerm(myc);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Instance instance)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    fmi_verbose_log(myc,"fmi3Reset()");
    doFree(myc);
    return doInit(myc);
}

FMI3_Export void fmi3FreeInstance(fmi3Instance instance)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetFloat64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT64_BASE_VR;
        if (idx<FMI_FLOAT64_VARS) {
            if (idx==FMI_FLOAT64_TIME_IDX)
                values[j++]=myc->float64_vars[idx][0][0];
            else
                CopyOut(values,j,myc->float64_vars[idx]);
        } else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetFloat32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT32_BASE_VR;
        if (idx<FMI_FLOAT32_VARS)
            CopyOut(values,j,myc->float32_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT64_BASE_VR;
        if (idx<FMI_UINT64_VARS)
            CopyOut(values,j,myc->uint64_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT64_BASE_VR;
        if (idx<FMI_INT64_VARS)
            CopyOut(values,j,myc->int64_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT32_BASE_VR;
        if (idx<FMI_UINT32_VARS)
            CopyOut(values,j,myc->uint32_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT32_BASE_VR;
        if (idx<FMI_INT32_VARS)
            CopyOut(values,j,myc->int32_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT16_BASE_VR;
        if (idx<FMI_UINT16_VARS)
            CopyOut(values,j,myc->uint16_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT16_BASE_VR;
        if (idx<FMI_INT16_VARS)
            CopyOut(values,j,myc->int16_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT8_BASE_VR;
        if (idx<FMI_UINT8_VARS)
            CopyOut(values,j,myc->uint8_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT8_BASE_VR;
        if (idx<FMI_INT8_VARS)
            CopyOut(values,j,myc->int8_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetBoolean(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BOOLEAN_BASE_VR;
        if (idx<FMI_BOOLEAN_VARS)
            CopyOut(values,j,myc->boolean_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetString(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_STRING_BASE_VR;
        if (idx<FMI_STRING_VARS)
            CopyOut(values,j,myc->string_vars[idx]);
        else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t sizes[], fmi3Binary values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetBinary(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BINARY_BASE_VR;
        if (idx<FMI_BINARY_VARS) {
            size_t j_init = j;
            CopyOut(sizes,j,myc->binary_sizes[idx]);
            CopyOut(values,j_init,myc->binary_vars[idx]);
        } else
            return fmi3Error;
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 values[], size_t nValues)
{
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT64_BASE_VR;
        if (idx<FMI_FLOAT64_VARS && idx!=FMI_FLOAT64_TIME_IDX)
            CopyIn(values,j,myc->float64_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_FLOAT32_BASE_VR;
        if (idx<FMI_FLOAT32_VARS)
            CopyIn(values,j,myc->float32_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT64_BASE_VR;
        if (idx<FMI_UINT64_VARS)
            CopyIn(values,j,myc->uint64_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT64_BASE_VR;
        if (idx<FMI_INT64_VARS)
            CopyIn(values,j,myc->int64_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT32_BASE_VR;
        if (idx<FMI_UINT32_VARS)
            CopyIn(values,j,myc->uint32_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT32_BASE_VR;
        if (idx<FMI_INT32_VARS)
            CopyIn(values,j,myc->int32_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT16_BASE_VR;
        if (idx<FMI_UINT16_VARS)
            CopyIn(values,j,myc->uint16_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT16_BASE_VR;
        if (idx<FMI_INT16_VARS)
            CopyIn(values,j,myc->int16_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_UINT8_BASE_VR;
        if (idx<FMI_UINT8_VARS)
            CopyIn(values,j,myc->uint8_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_INT8_BASE_VR;
        if (idx<FMI_INT8_VARS)
            CopyIn(values,j,myc->int8_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBoolean(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BOOLEAN_BASE_VR;
        if (idx<FMI_BOOLEAN_VARS)
            CopyIn(values,j,myc->boolean_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetString(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_STRING_BASE_VR;
        if (idx<FMI_STRING_VARS) {
            DoAll(myc->string_vars[idx],free);
            CopyInStr(values,j,myc->string_vars[idx]);
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
    SimpleArrayTest myc = (SimpleArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBinary(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        fmi3ValueReference idx = valueReferences[i] - FMI_BINARY_BASE_VR;
        if (idx<FMI_BINARY_VARS) {
            DoAll(myc->binary_vars[idx],free);
            CopyInBin(sizes,values,j,myc->binary_sizes[idx],myc->binary_vars[idx]);
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

FMI3_Export fmi3Status fmi3GetDoStepDiscardedStatus(fmi3Instance instance,
                                                    fmi3Boolean* terminate,
                                                    fmi3Float64* lastSuccessfulTime)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3DoEarlyReturn(fmi3Instance instance, fmi3Float64 earlyReturnTime)
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

FMI3_Export fmi3Status fmi3SetInputDerivatives(fmi3Instance instance,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3Int32 orders[],
                                               const fmi3Float64 values[],
                                               size_t nValues)
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
