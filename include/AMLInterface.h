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

#ifndef AML_INTERFACE_H_
#define AML_INTERFACE_H_

#include <string>
#include <vector>
#include <map>

namespace AML
{

class AMLData;

/**
 * @class AMLValueType
 * @brief This class represent AMLdata Value type
 */
enum class AMLValueType
{
    String = 0,
    StringArray,
    AMLData
};

/**
 * @class AMLValue
 * @brief This class use for AMLValue interface.
 */
class AMLValue
{
public:
    AMLValue(AMLValueType t) : m_type(t)
    {
    }
    virtual ~AMLValue()
    {
    }

    AMLValueType getType()
    {
        return m_type;
    }
private:
    AMLValueType m_type;
};

/**
 * @class AMLObject
 * @brief This class have AMLData.
 * @see AMLData
 */
class AMLObject
{
public:
    /**
     * @brief       Constructor.
     * @param       deviceId    [in] Device id that source device of AMLObject.
     * @param       timestamp   [in] timestamp value of AMLObject delibered by device.
     */
    AMLObject(const std::string& deviceId, const std::string& timeStamp);

    /**
     * @brief       Constructor.
     * @param       deviceId    [in] Device id that source device of AMLObject.
     * @param       timestamp   [in] timestamp value of AMLObject delibered by device.
     * @param       id          [in] id of AMLObject.
     */
    AMLObject(const std::string& deviceId, const std::string& timeStamp, const std::string& id);

    /**
     * @brief       Copy Constructor Overloading.
     */
    AMLObject(const AMLObject& t);

    /**
     * @brief       Assignment Operator Overloading.
     */
    AMLObject& operator=(const AMLObject& t);

    virtual ~AMLObject(void);

    /**
     * @fn void addData(const std::string& name, const AMLData& data)
     * @brief       This function add AMLData to AMLObject using AMLData key that to match AMLData value.
     * @param       name    [in] AMLData key.
     * @param       data    [in] AMLData value.
     * @exception   AMLException If AMLData key is duplicated on AMLObject or if name is a invalid key.
     */
    void                            addData(const std::string& name, const AMLData& data);

    /**
     * @fn AMLData getData(const std::string& name) const
     * @brief       This function return AMLData which matched input name string with AMLObject's amlDatas key.
     * @param       name    [in] String value to use matching with key.
     * @return      AMLData that have sub key value fair.
     * @exception   AMLException If the input name does not exist in amlDatas.
     */
    const AMLData&                  getData(const std::string& name) const;

    /**
     * @fn std::vector<std::string> getDataNames() const
     * @brief       This function return string list about AMLObject's amlDatas keys string array.
     * @return      vector of string data's keys value list.
     */
    std::vector<std::string>        getDataNames() const;

    /**
     * @fn std::string getDeviceId() const;
     * @brief       This function return Device's ID saved on AMLObject.
     * @return      String value of device's Id.
     */    
    const std::string&              getDeviceId() const;

    /**
     * @fn std::string getTimeStamp() const;
     * @brief       This function return timestamp that deliveried device.
     * @return      String value of timestamp.
     */
    const std::string&              getTimeStamp() const;

    /**
     * @fn std::string getId() const;
     * @brief       This function return ID that AMLObject.
     * @return      String value of AMLObject Id.
     */
    const std::string&              getId() const;

private:
    /**
     * @fn void copyData(AMLObject* target) const
     * @brief       This function copy AMLData Data 
     * @param       target     [in] AMLObject that target class want to input data
     */
    void                            copyObject(AMLObject* target) const;

    const std::string m_deviceId;
    const std::string m_timeStamp;
    const std::string m_id;
    mutable std::map<std::string, AMLData*> m_amlDatas;
};

/**
 * @class AMLData
 * @brief This class have RawData map which have key value pair.
 */
class AMLData
{
public:
    AMLData(void);

    /**
     * @brief       Copy Constructor Overloading.
     */
    AMLData(const AMLData& t);

    /**
     * @brief       Copy Constructor Overloading.
     */
    AMLData& operator=(const AMLData& t);

    virtual ~AMLData(void);

    /**
     * @fn void setValue(const std::string& key, const std::string& value)
     * @brief       This function set key and string type value pair on AMLData.
     * @param       key     [in] AMLData key.
     * @param       value   [in] AMLData value.
     */
    void                            setValue(const std::string& key, const std::string& value);

    /**
     * @fn void setValue(const std::string& key, const std::vector<std::string>& value)
     * @brief       This function set key and string vector type value pair on AMLData.
     * @param       key     [in] AMLData key.
     * @param       value   [in] AMLData value.
     */
    void                            setValue(const std::string& key, const std::vector<std::string>& value); 
    
    /**
     * @fn void setValue(const std::string& key, const AMLData& value)
     * @brief       This function set key and AMLData type value pair on AMLData.
     * @param       key     [in] AMLData key
     * @param       value   [in] AMLData value
     */
    void                            setValue(const std::string& key, const AMLData& value);

    /**
     * @fn std::string getValueToStr(const std::string& key) const
     * @brief       This function return string which matched key in a AMLData's AMLMap.
     * @param       key     [in] pair's which has string value, key.
     * @return      String value which matched using key on AMLMap.
     * @exception   AMLException If input key is not matching on AMLMap.
     */
    const std::string&              getValueToStr(const std::string& key) const;
    
    /**
     * @fn std::string getValueToStrArr(const std::string& key) const
     * @brief       This function return string array which matched key in a AMLData's AMLMap.
     * @param       key     [in] pair's which has string Array value, key.
     * @return      String array value which matched using key on AMLMap.
     * @exception   AMLException If input key is not matching on AMLMap.
     */
    const std::vector<std::string>& getValueToStrArr(const std::string& key) const;
    
    /**
     * @fn std::string getValueToAMLData(const std::string& key) const
     * @brief       This function return AMLData which matched key in a AMLData's AMLMap.
     * @param       key     [in] pair's which has AMLData, key.
     * @return      AMLData value which matched using key on AMLMap.
     * @exception   AMLException If input key is not matching on AMLMap.
     */
    const AMLData&                  getValueToAMLData(const std::string& key) const;

    /**
     * @fn std::vector<std::string> getKeys() const
     * @brief       This function return string list about AMLData's AMLMap keys string array.
     * @return      vector of string data's keys value list.
     */
    std::vector<std::string>        getKeys() const;

    /**
     * @fn AMLValueType getValueType(const std::string& key) const
     * @brief       This function return string list about AMLData's AMLMap keys string array.
     * @param       key     [in] string of the AMLData value to check.
     * @return      value's AMLValueType of pre defined data type.
     */
    AMLValueType                    getValueType(const std::string& key) const;

private:
    /**
     * @fn void copyData(AMLData* target) const
     * @brief       This function copy AMLValue Data 
     * @param       target     [in] AMLData that target class want to input data
     */
    void                            copyData(AMLData* target) const;

    std::map<std::string, AMLValue*> m_values;
};

} // namespace AML

#endif // AML_INTERFACE_H_
