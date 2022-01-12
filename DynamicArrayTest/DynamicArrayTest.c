/*
 * PMSF FMU Framework for FMI 3.0 Co-Simulation FMUs
 *
 * (C) 2016 -- 2022 PMSF IT Consulting Pierre R. Mai
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "DynamicArrayTest.h"

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
        fprintf(private_log_file,"DynamicArrayTest::Global: ");
        vfprintf(private_log_file, format, ap);
        fputc('\n',private_log_file);
        fflush(private_log_file);
    }
#endif
#endif
}

void internal_log(DynamicArrayTest component,const char* category, const char* format, va_list arg)
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
        fprintf(private_log_file,"DynamicArrayTest::%s<%p>: %s\n",component->instanceName,component,buffer);
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

void fmi_verbose_log(DynamicArrayTest component,const char* format, ...)
{
#if defined(VERBOSE_FMI_LOGGING) && (defined(PRIVATE_LOG_PATH) || defined(PUBLIC_LOGGING))
    va_list ap;
    va_start(ap, format);
    internal_log(component,"FMI",format,ap);
    va_end(ap);
#endif
}

/* Normal Logging */
void normal_log(DynamicArrayTest component, const char* category, const char* format, ...) {
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

fmi3Status doInit(DynamicArrayTest component)
{
    DEBUGBREAK();

    /* Structural Parameters */
    component->x_dimension_size = 4;
    component->y_dimension_size = 3;

    /* Arrays */
    component->float64_parameter = calloc(component->x_dimension_size*component->y_dimension_size,sizeof(fmi3Float64));
    component->float64_input = calloc(component->x_dimension_size*component->y_dimension_size,sizeof(fmi3Float64));
    component->float64_output = calloc(component->x_dimension_size*component->y_dimension_size,sizeof(fmi3Float64));

    return fmi3OK;
}

fmi3Status doInitCalc(DynamicArrayTest component)
{
    DEBUGBREAK();

    return fmi3OK;
}

fmi3Status doEnterInitializationMode(DynamicArrayTest component, fmi3Boolean toleranceDefined, fmi3Float64 tolerance, fmi3Float64 startTime, fmi3Boolean stopTimeDefined, fmi3Float64 stopTime)
{
    component->last_time = startTime;
    component->init_mode = fmi3True;
    doInitCalc(component);
    return fmi3OK;
}

fmi3Status doExitInitializationMode(DynamicArrayTest component)
{
    component->init_mode = fmi3False;
    return fmi3OK;
}

fmi3Status doCalc(DynamicArrayTest component, fmi3Float64 currentCommunicationPoint, fmi3Float64 communicationStepSize, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean* eventEncountered, fmi3Boolean* terminateSimulation, fmi3Boolean* earlyReturn, fmi3Float64* lastSuccessfulTime)
{
    size_t i,size;
    DEBUGBREAK();

    doInitCalc(component);

    for (i=0,size=component->x_dimension_size*component->y_dimension_size;i<size;i++)
        component->float64_output[i]=component->float64_input[i]*component->float64_parameter[i];

    component->last_time=currentCommunicationPoint+communicationStepSize;
    *lastSuccessfulTime = component->last_time;
    *eventEncountered = fmi3False;
    *earlyReturn = fmi3False;
    *terminateSimulation = fmi3False;
    return fmi3OK;
}

fmi3Status doTerm(DynamicArrayTest component)
{
    DEBUGBREAK();
    return fmi3OK;
}

void doFree(DynamicArrayTest component)
{
    DEBUGBREAK();

    free(component->float64_parameter);
    free(component->float64_input);
    free(component->float64_output);
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
    DynamicArrayTest myc = (DynamicArrayTest)instance;
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
    fmi3String                     resourcePath,
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
    DynamicArrayTest myc = NULL;

#ifdef FMU_TOKEN
    if (instantiationToken!=NULL && 0!=strcmp(instantiationToken,FMU_TOKEN)) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = NULL (GUID mismatch, expected %s)",
            instanceName, instantiationToken,
            (resourcePath != NULL) ? resourcePath : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed,
            FMU_TOKEN);
        return NULL;
    }
#endif

    myc = calloc(1,sizeof(struct DynamicArrayTest));

    if (myc == NULL) {
        fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = NULL (alloc failure)",
            instanceName, instantiationToken,
            (resourcePath != NULL) ? resourcePath : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed);
        return NULL;
    }

    myc->instanceName=strdup(instanceName);
    myc->instantiationToken=strdup(instantiationToken);
    myc->resourcePath=strdup(resourcePath);
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
            (resourcePath != NULL) ? resourcePath : "<NULL>",
            visible, loggingOn, eventModeUsed, earlyReturnAllowed);
        free(myc->resourcePath);
        free(myc->instantiationToken);
        free(myc->instanceName);
        for (;myc->loggingCategories!=NULL && myc->nCategories>0;) free(myc->loggingCategories[--(myc->nCategories)]);
        free(myc->loggingCategories);
        free(myc);
        return NULL;
    }
    fmi_verbose_log_global("fmi3InstantiateCoSimulation(\"%s\",\"%s\",\"%s\",%d,%d,%d,%d,...) = %p",
        instanceName, instantiationToken,
        (resourcePath != NULL) ? resourcePath : "<NULL>",
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
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3EnterInitializationMode(%d,%g,%g,%d,%g)", toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
    return doEnterInitializationMode(myc,toleranceDefined, tolerance, startTime, stopTimeDefined, stopTime);
}

FMI3_Export fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3ExitInitializationMode()");
    return doExitInitializationMode(myc);
}

FMI3_Export fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3EnterConfigurationMode()");
    myc->reconfiguration_mode = 1;
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3ExitConfigurationMode()");
    myc->float64_parameter = myrecalloc(myc->float64_parameter,myc->x_dimension_size*myc->y_dimension_size,sizeof(fmi3Float64));
    myc->float64_input = myrecalloc(myc->float64_input,myc->x_dimension_size*myc->y_dimension_size,sizeof(fmi3Float64));
    myc->float64_output = myrecalloc(myc->float64_output,myc->x_dimension_size*myc->y_dimension_size,sizeof(fmi3Float64));
    myc->reconfiguration_mode = 0;
    return fmi3OK;
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
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3DoStep(%g,%g,%d,%p,%p,%p,%p)", currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, eventEncountered, terminateSimulation, earlyReturn, lastSuccessfulTime);
    return doCalc(myc,currentCommunicationPoint, communicationStepSize, noSetFMUStatePriorToCurrentPoint, eventEncountered, terminateSimulation, earlyReturn, lastSuccessfulTime);
}

FMI3_Export fmi3Status fmi3Terminate(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3Terminate()");
    return doTerm(myc);
}

FMI3_Export fmi3Status fmi3Reset(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3Reset()");
    doFree(myc);
    return doInit(myc);
}

FMI3_Export void fmi3FreeInstance(fmi3Instance instance)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    fmi_verbose_log(myc,"fmi3FreeInstance()");
    doFree(myc);

    free(myc->resourcePath);
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
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j,k,size;
    fmi_verbose_log(myc,"fmi3GetFloat64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            case FMI_FLOAT64_TIME_VR:
                values[j++]=myc->last_time;
                break;
            case FMI_FLOAT64_PARAMETER_VR:
                size=myc->x_dimension_size*myc->y_dimension_size;
                for (k=0;k<size;k++)
                    values[j++]=myc->float64_parameter[k];
                break;
            case FMI_FLOAT64_INPUT_VR:
                size=myc->x_dimension_size*myc->y_dimension_size;
                for (k=0;k<size;k++)
                    values[j++]=myc->float64_input[k];
                break;
            case FMI_FLOAT64_OUTPUT_VR:
                size=myc->x_dimension_size*myc->y_dimension_size;
                for (k=0;k<size;k++)
                    values[j++]=myc->float64_output[k];
                break;
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Float32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetFloat32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt64 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            case FMI_UINT64_X_SIZE_VR:
                values[j++]=myc->x_dimension_size;
                break;
            case FMI_UINT64_Y_SIZE_VR:
                values[j++]=myc->y_dimension_size;
                break;
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int64 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt16 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int16 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3UInt8 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetUInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Int8 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetBoolean(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetString(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t sizes[], fmi3Binary values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    fmi_verbose_log(myc,"fmi3GetBinary(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float64 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j,k,size;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            case FMI_FLOAT64_TIME_VR:
                return fmi3Error;
            case FMI_FLOAT64_PARAMETER_VR:
                size=myc->x_dimension_size*myc->y_dimension_size;
                for (k=0;k<size;k++)
                    myc->float64_parameter[k]=values[j++];
                break;
            case FMI_FLOAT64_INPUT_VR:
                size=myc->x_dimension_size*myc->y_dimension_size;
                for (k=0;k<size;k++)
                    myc->float64_input[k]=values[j++];
                break;
            case FMI_FLOAT64_OUTPUT_VR:
                return fmi3Error;
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetFloat32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Float32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetFloat32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt64 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            case FMI_UINT64_X_SIZE_VR:
                if (!myc->reconfiguration_mode)
                    return fmi3Error;
                myc->x_dimension_size=values[j++];
                break;
            case FMI_UINT64_Y_SIZE_VR:
                if (!myc->reconfiguration_mode)
                    return fmi3Error;
                myc->y_dimension_size=values[j++];
                break;
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt64(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int64 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt64(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt32(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int32 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt32(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt16 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt16(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int16 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt16(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetUInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3UInt8 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetUInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetInt8(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Int8 values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetInt8(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetBoolean(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBoolean(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetString(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetString(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
    }
    if (myc->init_mode || tuned)
        doInitCalc(myc);
    return fmi3OK;
}

FMI3_Export fmi3Status fmi3SetBinary(fmi3Instance instance, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t sizes[], const fmi3Binary values[], size_t nValues)
{
    DynamicArrayTest myc = (DynamicArrayTest)instance;
    size_t i,j;
    int tuned = 0;
    fmi_verbose_log(myc,"fmi3SetBinary(...)");
    for (i = 0,j = 0; i<nValueReferences; i++) {
        switch (valueReferences[i]) {
            default:
                return fmi3Error;
        }
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

FMI3_Export fmi3Status fmi3EvaluateDiscreteStates(fmi3Instance instance)
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

/*
 * Unsupported Interfaces (Model Exchange, Scheduled Execution)
 */

FMI3_Export fmi3Instance fmi3InstantiateModelExchange(
    fmi3String                 instanceName,
    fmi3String                 instantiationToken,
    fmi3String                 resourcePath,
    fmi3Boolean                visible,
    fmi3Boolean                loggingOn,
    fmi3InstanceEnvironment    instanceEnvironment,
    fmi3CallbackLogMessage     logMessage)
{
    return NULL;
}

FMI3_Export fmi3Instance fmi3InstantiateScheduledExecution(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourcePath,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate,
    fmi3CallbackLockPreemption     lockPreemption,
    fmi3CallbackUnlockPreemption   unlockPreemption)
{
    return NULL;
}

FMI3_Export fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3CompletedIntegratorStep(fmi3Instance instance,
                                                   fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                   fmi3Boolean* enterEventMode,
                                                   fmi3Boolean* terminateSimulation)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3SetContinuousStates(fmi3Instance instance,
                                               const fmi3Float64 continuousStates[],
                                               size_t nContinuousStates)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetContinuousStateDerivatives(fmi3Instance instance,
                                                         fmi3Float64 derivatives[],
                                                         size_t nContinuousStates)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetEventIndicators(fmi3Instance instance,
                                              fmi3Float64 eventIndicators[],
                                              size_t nEventIndicators)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetContinuousStates(fmi3Instance instance,
                                               fmi3Float64 continuousStates[],
                                               size_t nContinuousStates)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Instance instance,
                                                         fmi3Float64 nominals[],
                                                         size_t nContinuousStates)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance,
                                                      size_t* nEventIndicators)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance,
                                                       size_t* nContinuousStates)
{
    return fmi3Error;
}

FMI3_Export fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
                                                  fmi3ValueReference clockReference,
                                                  size_t clockElementIndex,
                                                  fmi3Float64 activationTime)
{
    return fmi3Error;
}
