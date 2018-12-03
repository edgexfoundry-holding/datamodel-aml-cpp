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

#include "AMLException.h"

namespace Exception
{
    static const char NO_ERROR[]                    = "No Error";
    static const char INVALID_PARAM[]               = "Invalid Parameter";
    static const char INVALID_FILE_PATH[]           = "Invalid File Path";
    static const char INVALID_AML_SCHEMA[]          = "Invalid AutomationML Schema File";
    static const char INVALID_XML_STR[]             = "Invalid XML Style String";
    static const char NOT_MATCH_TO_AML_MODEL[]      = "Data does not match to AML model";
    static const char INVALID_BYTE_STR[]            = "Invalid Byte string";
    static const char SERIALIZE_FAIL[]              = "Failed to serialize";
    static const char NO_MEMORY[]                   = "No Momory";
    static const char KEY_NOT_EXIST[]               = "Key does Not Exist";
    static const char KEY_ALREADY_EXIST[]           = "Key already Exists";
    static const char WRONG_GETTER_TYPE[]           = "Wrong Getter function call for Value";
    static const char API_NOT_ENABLED[]             = "API is Not Enabled";
}

std::string AML::AMLException::reason(const ResultCode resCode)
{
    switch (resCode)
    {
        case AML::INVALID_PARAM:
            return Exception::INVALID_PARAM;
        case AML::INVALID_FILE_PATH:
            return Exception::INVALID_FILE_PATH;
        case AML::INVALID_XML_STR:
            return Exception::INVALID_XML_STR;
        case AML::INVALID_AML_SCHEMA:
            return Exception::INVALID_AML_SCHEMA;
        case AML::NOT_MATCH_TO_AML_MODEL:
            return Exception::NOT_MATCH_TO_AML_MODEL;
        case AML::INVALID_BYTE_STR:
            return Exception::INVALID_BYTE_STR;
        case AML::SERIALIZE_FAIL:
            return Exception::SERIALIZE_FAIL;
        case AML::NO_MEMORY:
            return Exception::NO_MEMORY;
        case AML::KEY_NOT_EXIST:
            return Exception::KEY_NOT_EXIST;
        case AML::KEY_ALREADY_EXIST:
            return Exception::KEY_ALREADY_EXIST;
        case AML::WRONG_GETTER_TYPE:
            return Exception::WRONG_GETTER_TYPE;
        case AML::API_NOT_ENABLED:
            return Exception::API_NOT_ENABLED;

        default:
            return Exception::NO_ERROR;
    }
}
