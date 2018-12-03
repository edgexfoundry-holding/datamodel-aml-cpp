/*******************************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#ifndef AML_LOGGER_H_
#define AML_LOGGER_H_

#include <stdio.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <cinttypes>
#else
#include <inttypes.h>
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define VERIFY_NON_NULL(PARAM) \
if (!PARAM) \
{ \
    AML_LOG_V(ERROR, TAG, "[%s:%d] NULL parameter", __func__, __LINE__); \
    return AML_ERROR; \
}

#define VERIFY_NON_NULL_TOPIC(PARAM) \
if (!PARAM) \
{ \
    AML_LOG_V(ERROR, TAG, "[%s:%d] Invalid Topic", __func__, __LINE__); \
    return AML_INVALID_TOPIC; \
}

#define ALLOC_ASSERT(PARAM) \
if (!PARAM) \
{ \
    AML_LOG_V(ERROR, TAG, "[%s:%d] Memory allocation failed", __func__, __LINE__); \
    abort(); \
} \

// Max buffer size
#define MAX_LOG_V_BUFFER_SIZE (256)

// Setting this flag for a log level means that the corresponding log message
// contains private data. This kind of message is logged only when a call to
// AMLSetLogLevel() enabled private data logging.
#define AML_LOG_PRIVATE_DATA (1 << 31)

typedef enum {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;

#define DEBUG_PRIVATE ((AML_LOG_PRIVATE_DATA) | (DEBUG))
#define INFO_PRIVATE ((AML_LOG_PRIVATE_DATA) | (INFO))
#define WARNING_PRIVATE ((AML_LOG_PRIVATE_DATA) | (WARNING))
#define ERROR_PRIVATE ((AML_LOG_PRIVATE_DATA) | (ERROR))
#define FATAL_PRIVATE ((AML_LOG_PRIVATE_DATA) | (FATAL))

#ifndef AML_LOG_LEVEL
#define AML_MINIMUM_LOG_LEVEL    (DEBUG)
#else
#define AML_MINIMUM_LOG_LEVEL    (AML_LOG_LEVEL)
#endif

#define IF_AML_PRINT_LOG_LEVEL(level) \
    if (((int)AML_MINIMUM_LOG_LEVEL) <= ((int)(level & (~AML_LOG_PRIVATE_DATA))))

/**
 * Set log level and privacy log to print.
 *
 * @param level - log level.
 * @param hidePrivateLogEntries - Hide Private Log.
 */
void AMLSetLogLevel(LogLevel level, bool hidePrivateLogEntries);

/**
* Output a variable argument list log string with the specified priority level.
*
* @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag    - Module name
* @param format - variadic log string
*/
void AMLLogv(int level, const char * tag, const char * format, ...)
#if defined(__GNUC__)
    __attribute__ ((format(printf, 3, 4)))
#endif
    ;

/**
* Output a log string with the specified priority level.
*
* @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag    - Module name
* @param logStr - log string
*/
void AMLLog(int level, const char * tag, const char * logStr);

/**
* Output the contents of the specified buffer (in hex) with the specified priority level.
*
* @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
* @param tag        - Module name
* @param buffer     - pointer to buffer of bytes
* @param bufferSize - max number of byte in buffer
*/
void AMLLogBuffer(int level, const char* tag, const uint8_t* buffer, size_t bufferSize);


#ifdef DEBUG_LOG

#define AML_LOG_BUFFER(level, tag, buffer, bufferSize) \
    do { \
        IF_AML_PRINT_LOG_LEVEL((level)) \
            AMLLogBuffer((level), (tag), (buffer), (bufferSize)); \
    } while(0)


#define AML_LOG(level, tag, logStr) \
    do { \
        IF_AML_PRINT_LOG_LEVEL((level)) \
            AMLLog((level), (tag), (logStr)); \
    } while(0)

// Define variable argument log function
#define AML_LOG_V(level, tag, ...) \
    do { \
        IF_AML_PRINT_LOG_LEVEL((level)) \
            AMLLogv((level), (tag), __VA_ARGS__); \
    } while(0)

#define AML_SCOPE_LOGGER(TAG, FUNC) ScopeLogger scopeLogger(TAG, FUNC)

#else // DEBUG_LOG
#define AML_LOG(level, tag, logStr)
#define AML_LOG_V(level, tag, ...)
#define AML_LOG_BUFFER(level, tag, buffer, bufferSize)
#define AML_SCOPE_LOGGER(TAG, FUNC)
#endif // DEBUG_LOG

namespace aml
{
    class ScopeLogger
    {
        public:
            ScopeLogger(const char *tag, const char *method)
            {
                m_funName = method;
                m_tag = tag;
                AML_LOG_V(DEBUG, m_tag, "[%s] IN", m_funName);
            }

            ~ScopeLogger()
            {
                AML_LOG_V(DEBUG, m_tag, "[%s] OUT", m_funName);
            }

        private:
            const char *m_funName;
            const char* m_tag;
    };
}

#endif // AML_LOGGER_H_

