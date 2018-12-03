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

#ifndef REPRESENTAITON_H_
#define REPRESENTAITON_H_

#include <string>

#include "AMLInterface.h"

namespace AML
{

/**
 *  @class  Representation
 *  @brief  This class converts between AMLObject, AML(XML) string, AML(Protobuf) byte.
 *  @see AMLObject
 */
class Representation
{
public:
    /**
     * @brief       Constructor.
     * @param       amlFilePath [in] File path of AML that contains a data model information.
     */
    Representation(const std::string amlFilePath);
    virtual ~Representation(void);

    /**
     * @fn std::string DataToAml(const AMLObject& amlObject) const
     * @brief       This function converts AMLObject to AML(XML) string to match the AML model information which is set by constructor.
     * @param       amlObject [in] AMLObject to be converted.
     * @return      AML(XML) string converted from amlObject.
     * @exception   AMLException If the schema of amlObject does not match to AML model information
     */
    std::string DataToAml(const AMLObject& amlObject) const;

    /**
     * @fn AMLObject* AmlToData(const std::string& xmlStr) const
     * @brief       This function converts AML(XML) string to AMLObject to match the AML model information which is set by constructor.
     * @param       xmlStr [in] AML(XML) string to be converted.
     * @return      AMLObject instance converted from AML(XML) string.
     * @exception   AMLException If the schema of xmlStr does not match to AML model information
     * @note        AMLObject instance will be allocated and returned, so it should be deleted after use.
     */
    AMLObject* AmlToData(const std::string& xmlStr) const;

    /**
     * @fn std::string DataToByte(const AMLObject& amlObject) const
     * @brief       This function converts AMLObject to Protobuf byte data to match the AML model information which is set by constructor.
     * @param       amlObject [in] AMLObject to be converted.
     * @return      Protobuf byte data(string) converted from amlObject.
     * @exception   AMLException If the schema of amlObject does not match to AML model information
     * @node        If 'disable_protobuf' build option is enabled, this API will be DISABLED and throw AMLException with code 'API_NOT_ENABLED'.
     */
    std::string DataToByte(const AMLObject& amlObject) const;

    /**
     * @fn AMLObject* ByteToData(const std::string& byte) const
     * @brief       This function converts Protobuf byte data to AMLObject to match the AML model information which is set by constructor.
     * @param       byte [in] Protobuf byte data(string) to be converted.
     * @return      AMLObject instance converted from amlObject.
     * @exception   AMLException If the schema of byte does not match to AML model information
     * @node        If 'disable_protobuf' build option is enabled, this API will be DISABLED and throw AMLException with code 'API_NOT_ENABLED'.
     * @note        AMLObject instance will be allocated and returned, so it should be deleted after use.
     */
    AMLObject* ByteToData(const std::string& byte) const;

    /**
     * @fn std::string getRepresentationId() const
     * @brief       This function returns AutomationML SystemUnitClassLib's unique ID
     * @return      string value of AML SystemUnitClassLIb's ID
     */
    std::string getRepresentationId() const;

    /**
     * @fn AMLObject* getConfigInfo() const
     * @brief       This function returns AMLObject that contains configuration data which is present in RoleClassLib.
     * @return      AMLObject that has configuration data.
     * @note        AMLObject instance will be allocated and returned, so it should be deleted after use.
     */
    AMLObject* getConfigInfo() const;

private:
    class AMLModel;
    AMLModel* m_amlModel;
};

} // namespace AML

#endif // REPRESENTAITON_H_