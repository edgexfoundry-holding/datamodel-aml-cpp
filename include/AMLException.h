/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

#ifndef AML_EXCEPTION_H_
#define AML_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace AML
{

typedef enum
{
    NO_ERROR = 0,

    INVALID_PARAM = 100,
    INVALID_FILE_PATH,
    INVALID_AML_SCHEMA,
    INVALID_XML_STR,
    NOT_MATCH_TO_AML_MODEL,
    INVALID_BYTE_STR,
    SERIALIZE_FAIL,
    NO_MEMORY,
    KEY_NOT_EXIST,
    KEY_ALREADY_EXIST,
    WRONG_GETTER_TYPE,
    API_NOT_ENABLED,
} ResultCode;

class AMLException : public std::runtime_error
{
    public:
        AMLException(ResultCode resCode = NO_ERROR)
        : std::runtime_error(this->reason(resCode)), m_code(resCode) {}

        static std::string reason(const ResultCode resCode);

        std::string reason() const
        {
            return reason(m_code);
        }

        ResultCode code() const
        {
            return m_code;
        }

    private:
        ResultCode m_code;
};

} // namespace AML

#endif // AML_EXCEPTION_H_
