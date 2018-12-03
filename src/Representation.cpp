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

#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>

#include "pugixml.hpp"

#include "Representation.h"
#include "AMLInterface.h"
#include "AMLException.h"
#include "AMLLogger.h"

#ifndef _DISABLE_PROTOBUF_
#include "AML.pb.h"
#endif

#define TAG "Representation"

using namespace std;
using namespace AML;

static const char CAEX_FILE[]                       = "CAEXFile";
static const char INSTANCE_HIERARCHY[]              = "InstanceHierarchy";
static const char ROLE_CLASS_LIB[]                  = "RoleClassLib";
static const char ROLE_CLASS[]                      = "RoleClass";
static const char SYSTEM_UNIT_CLASS_LIB[]           = "SystemUnitClassLib";
static const char SYSTEM_UNIT_CLASS[]               = "SystemUnitClass";
static const char INTERNAL_ELEMENT[]                = "InternalElement";
static const char ATTRIBUTE[]                       = "Attribute";
static const char ADDITIONAL_INFORMATION[]          = "AdditionalInformation";
static const char REF_SEMANTIC[]                    = "RefSemantic";
static const char VERSION[]                         = "Version";

static const char NAME[]                            = "Name";
static const char VALUE[]                           = "Value";
static const char ATTRIBUTE_DATA_TYPE[]             = "AttributeDataType";
static const char DESCRIPTION[]                     = "Description";
static const char REF_BASE_SYSTEM_UNIT_PATH[]       = "RefBaseSystemUnitPath";
static const char REF_ROLE_CLASS_PATH[]             = "RefRoleClassPath";
static const char SUPPORTED_ROLE_CLASS[]            = "SupportedRoleClass";
static const char CORRESPONDING_ATTRIBUTE_PATH[]    = "CorrespondingAttributePath";
static const char ORDERED_LIST_TYPE[]               = "OrderedListType";

static const char EVENT[]                           = "Event";

static const char KEY_DEVICE[]                      = "device";
static const char KEY_ID[]                          = "id";
static const char KEY_TIMESTAMP[]                   = "timestamp";

#define IS_NAME(node, name)                     (std::string((node).attribute(NAME).value()) == (name))
#define ADD_VALUE(node, value)                  ((node).append_child(VALUE).text().set((value).c_str())) //#TODO: verify non-null after append_child()

#define VERIFY_NON_NULL_THROW_EXCEPTION(var)    if (NULL == (var)) throw AMLException(NO_MEMORY); 

#define IS_VALUE_TYPE_STRING(node)              (NULL != (node).child(VALUE))
#define IS_VALUE_TYPE_STRING_ARRAY(node)        ((NULL != (node).child(REF_SEMANTIC)) && \
                                                 0 != strncmp((node).attribute(CORRESPONDING_ATTRIBUTE_PATH).value(), ORDERED_LIST_TYPE, strlen(ORDERED_LIST_TYPE)))
#define IS_VALUE_TYPE_MAP(node)                 ((NULL == (node).child(REF_SEMANTIC)) && (NULL != (node).child(ATTRIBUTE)))

// for test ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRINT_NODE(node)    for (pugi::xml_node tool = (node).first_child(); tool; tool = tool.next_sibling()) \
                            {\
                                std::cout << "Tool:";\
                                for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())\
                                {\
                                     std::cout << " " << attr.name() << "=" << attr.value();\
                                }\
                                std::cout << std::endl;\
                            }\
                            std::cout<<std::endl;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DISABLE_PROTOBUF_
template <typename T>
static void extractAttribute(T* attr, pugi::xml_node xmlNode);

template <typename T>
static void extractInternalElement(T* ie, pugi::xml_node xmlNode);

template <typename T>
static void extractProtoAttribute(pugi::xml_node xmlNode, T* attr);

template <typename T>
static void extractProtoInternalElement(pugi::xml_node xmlNode, T* ie);
#endif // _DISABLE_PROTOBUF_

template <typename T>
static std::string toString(const T& t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

class Representation::AMLModel
{
public:
    AMLModel (const std::string& amlFilePath)
    {
        m_doc = new pugi::xml_document();

        pugi::xml_parse_result result = m_doc->load_file(amlFilePath.c_str());
        if (pugi::status_ok != result.status) 
        {
            AML_LOG_V(ERROR, TAG, "Failed to load file : %s", amlFilePath.c_str());
            delete m_doc;
            throw AMLException(INVALID_FILE_PATH);
        }

        pugi::xml_node xmlCaexFile = m_doc->child(CAEX_FILE);
        if (NULL == xmlCaexFile)
        {
            AML_LOG(ERROR, TAG, "Invalid AML File : <CAEXFile> does not exist");
            delete m_doc;
            throw AMLException(INVALID_AML_SCHEMA);
        }

        m_systemUnitClassLib = xmlCaexFile.child(SYSTEM_UNIT_CLASS_LIB);
        if (NULL == m_systemUnitClassLib) 
        {
            AML_LOG(ERROR, TAG, "Invalid AML File : <SystemUnitClassLib> does not exist");
            delete m_doc;
            throw AMLException(INVALID_AML_SCHEMA);
        }

        m_roleClassLib = xmlCaexFile.child(ROLE_CLASS_LIB);
        if (NULL == m_roleClassLib) 
        {
            AML_LOG(ERROR, TAG, "Invalid AML File : <RoleClassLib> does not exist");
            delete m_doc;
            throw AMLException(INVALID_AML_SCHEMA);
        }

        // remove "AdditionalInformation" and "InstanceHierarchy" data
        while (xmlCaexFile.child(ADDITIONAL_INFORMATION))   xmlCaexFile.remove_child(ADDITIONAL_INFORMATION);
        while (xmlCaexFile.child(INSTANCE_HIERARCHY))       xmlCaexFile.remove_child(INSTANCE_HIERARCHY);
    }

    ~AMLModel()
    {
        delete m_doc;
    }

    AMLObject* constructConfigAmlObject()
    {
        std::string deviceName(m_roleClassLib.attribute(NAME).value());

        AMLObject* amlObj = new AMLObject(deviceName, "0");

        for (pugi::xml_node xml_suc = m_systemUnitClassLib.child(SYSTEM_UNIT_CLASS); xml_suc; xml_suc = xml_suc.next_sibling(SYSTEM_UNIT_CLASS))
        {
            std::string className = xml_suc.attribute(NAME).value();
            if (0 == className.compare(EVENT)) // Skip "Event"
            {
                continue;
            }

            pugi::xml_node xml_rc = m_roleClassLib.find_child_by_attribute(ROLE_CLASS, NAME, className.c_str());
            if (NULL == xml_rc) 
            {
                AML_LOG_V(ERROR, TAG, "Invalid AML File : <RoleClass NAME=\"%s\"> does not exist", className.c_str());
                throw AMLException(KEY_NOT_EXIST); //@TODO: need to be more specific
            }

            AMLData amlData;
            for (pugi::xml_node xml_attr = xml_rc.child(ATTRIBUTE); xml_attr; xml_attr = xml_attr.next_sibling(ATTRIBUTE))
            {
                amlData.setValue(xml_attr.attribute(NAME).value(), xml_attr.child_value(VALUE));
            }

            amlObj->addData(className, amlData);
        }

        return amlObj;
    }

    AMLObject* constructAmlObject(pugi::xml_document* xml_doc)
    {
        assert(nullptr != xml_doc);

        if (NULL == xml_doc->child(CAEX_FILE) ||
            NULL == xml_doc->child(CAEX_FILE).child(INSTANCE_HIERARCHY))
        {
            AML_LOG(ERROR, TAG, "<CAEXFile> or <InstanceHierarchy> does not exist");
            throw AMLException(INVALID_AML_SCHEMA);
        }

        pugi::xml_node xml_event = xml_doc->child(CAEX_FILE).child(INSTANCE_HIERARCHY).find_child_by_attribute(INTERNAL_ELEMENT, NAME, EVENT);
        if (NULL == xml_event) 
        {
            AML_LOG(ERROR, TAG, "<Event> does not exist");
            throw AMLException(INVALID_AML_SCHEMA);
        }

        std::string deviceId, timeStamp, id;
        for (pugi::xml_node xml_attr = xml_event.child(ATTRIBUTE); xml_attr; xml_attr = xml_attr.next_sibling(ATTRIBUTE))
        {
            if      (IS_NAME(xml_attr, KEY_DEVICE))     deviceId = xml_attr.child_value(VALUE);
            else if (IS_NAME(xml_attr, KEY_TIMESTAMP))  timeStamp = xml_attr.child_value(VALUE);
            else if (IS_NAME(xml_attr, KEY_ID))         id = xml_attr.child_value(VALUE);
        }

        AMLObject* amlObj = new AMLObject(deviceId, timeStamp, id);

        for (pugi::xml_node xml_ie = xml_event.child(INTERNAL_ELEMENT); xml_ie; xml_ie = xml_ie.next_sibling(INTERNAL_ELEMENT))
        {
            AMLData amlData = constructAmlData(xml_ie);
            std::string name = xml_ie.attribute(NAME).value();

            amlObj->addData(name, amlData);
        }

        return amlObj;
    }

    pugi::xml_document* constructXmlDoc()
    {
        pugi::xml_document* xml_doc = new pugi::xml_document();

        initializeAML(xml_doc);

        return xml_doc;
    }

    pugi::xml_document* constructXmlDoc(const AMLObject& amlObject)
    {
        pugi::xml_document* xml_doc = constructXmlDoc();
        // add InstanceHierarchy
        pugi::xml_node xml_ih = xml_doc->child(CAEX_FILE).append_child(INSTANCE_HIERARCHY);
        VERIFY_NON_NULL_THROW_EXCEPTION(xml_ih);

        xml_ih.append_attribute(NAME) = m_systemUnitClassLib.attribute(NAME).value(); // set IH name to be the same as SUCL name

        // add Event as InternalElement
        pugi::xml_node xml_event = addInternalElement(xml_ih, EVENT);
        assert(NULL != xml_event);

        // set default attributes of Event (This has a dependency on AMLObject class..)
        for (pugi::xml_node xml_attr = xml_event.child(ATTRIBUTE); xml_attr; xml_attr = xml_attr.next_sibling(ATTRIBUTE))
        {
            if      (IS_NAME(xml_attr, KEY_DEVICE))     ADD_VALUE(xml_attr, amlObject.getDeviceId());
            else if (IS_NAME(xml_attr, KEY_TIMESTAMP))  ADD_VALUE(xml_attr, amlObject.getTimeStamp());
            else if (IS_NAME(xml_attr, KEY_ID))         ADD_VALUE(xml_attr, amlObject.getId());
        }

        // add AMLDatas into Event
        vector<string> dataNames = amlObject.getDataNames();

        for (string name : dataNames)
        {
            AMLData data = amlObject.getData(name);

            pugi::xml_node xml_ie = addInternalElement(xml_event, name);

            setAttributeValue(xml_ie, &data);
        }
        return xml_doc;
    }

    void appendModel(pugi::xml_document* xml_doc)
    {
        assert(nullptr != xml_doc);
        xml_doc->child(CAEX_FILE).append_copy(m_roleClassLib);
        xml_doc->child(CAEX_FILE).append_copy(m_systemUnitClassLib);
    }

    std::string constructModelId()
    {
        std::string suclName(m_systemUnitClassLib.attribute(NAME).value());
        std::string suclVersion(m_systemUnitClassLib.child_value(VERSION));

        std::string modelId = suclName + "_" + suclVersion;

        return modelId;
    }

private:
    pugi::xml_document* m_doc;
    pugi::xml_node m_systemUnitClassLib;
    pugi::xml_node m_roleClassLib;

    void initializeAML(pugi::xml_document* xml_doc)
    {
        pugi::xml_node xml_decl = xml_doc->prepend_child(pugi::node_declaration);
        xml_decl.append_attribute("version") = "1.0";
        xml_decl.append_attribute("encoding") = "utf-8";
        //xml_decl.append_attribute("standalone") = "yes";

        pugi::xml_node xml_caexFile = xml_doc->append_child(CAEX_FILE);
        VERIFY_NON_NULL_THROW_EXCEPTION(xml_caexFile);
        xml_caexFile.append_attribute("FileName") = "";
        xml_caexFile.append_attribute("SchemaVersion") = "2.15";
        xml_caexFile.append_attribute("xsi:noNamespaceSchemaLocation") = "CAEX_ClassModel_V2.15.xsd";
        xml_caexFile.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
    }

    AMLData constructAmlData(pugi::xml_node xml_ie)
    {
        AMLData amlData;

        for (pugi::xml_node xml_attr = xml_ie.child(ATTRIBUTE); xml_attr; xml_attr = xml_attr.next_sibling(ATTRIBUTE))
        {
            std::string key = xml_attr.attribute(NAME).value();

            if (IS_VALUE_TYPE_STRING(xml_attr))
            {
                std::string value(xml_attr.child_value(VALUE));

                amlData.setValue(key, value);
            }
            else if (IS_VALUE_TYPE_STRING_ARRAY(xml_attr))
            {
                vector<string> values;
                pugi::xml_node xml_child;
                size_t sizeOfArray = 0;

                for (xml_child = xml_attr.child(ATTRIBUTE); xml_child; xml_child = xml_child.next_sibling(ATTRIBUTE))
                {
                    sizeOfArray++;
                }

                for (size_t index = 1; index <= sizeOfArray; ++index)
                {
                    xml_child = xml_attr.find_child_by_attribute(NAME, toString(index).c_str());
                    std::string value(xml_child.child_value(VALUE));
                    values.push_back(value);
                }

                amlData.setValue(key, values);
            }
            else if (IS_VALUE_TYPE_MAP(xml_attr))
            {
                AMLData value = constructAmlData(xml_attr);

                amlData.setValue(key, value);
            }
            else
            {
                AML_LOG_V(ERROR, TAG, "Invalid AML : <%s> has value of invalid type", key.c_str());
                throw AMLException(INVALID_AML_SCHEMA);
            }
        }

        return amlData;
    }

    pugi::xml_node addInternalElement(pugi::xml_node xml_parent, const std::string suc_name)
    {
        pugi::xml_node xml_suc = m_systemUnitClassLib.find_child_by_attribute(NAME, suc_name.c_str());
        if (!xml_suc)
        {
            AML_LOG_V(ERROR, TAG, "Invalid Data : <%s> is not present in SystemUnitClassLib", suc_name.c_str());
            throw AMLException(NOT_MATCH_TO_AML_MODEL);
        }

        pugi::xml_node xml_ie = xml_parent.append_copy(xml_suc);

        // reset Name (SystemUnitClass -> InternalElement)
        xml_ie.set_name(INTERNAL_ELEMENT);

        // set RefBaseSystemUnitPath
        std::string refBaseSystemUnitPath;
        refBaseSystemUnitPath.append(m_systemUnitClassLib.attribute(NAME).value());
        refBaseSystemUnitPath.append("/");
        refBaseSystemUnitPath.append(suc_name);
        xml_ie.append_attribute(REF_BASE_SYSTEM_UNIT_PATH) = refBaseSystemUnitPath.c_str();

        // // set SupportedRoleClass
        // xml_ie.append_copy(xml_suc.child(SUPPORTED_ROLE_CLASS));

        return xml_ie;
    }

    void setAttributeValue(pugi::xml_node xml_ie, AMLData* amlData)
    {
        for (pugi::xml_node xml_attr = xml_ie.child(ATTRIBUTE); xml_attr; xml_attr = xml_attr.next_sibling(ATTRIBUTE))
        {
            std::string attributeName(xml_attr.attribute(NAME).value());

            if(NULL != xml_attr.child(DESCRIPTION))
            {
                if(NULL == xml_attr.child(DESCRIPTION).next_sibling())
                    ADD_VALUE(xml_attr, amlData->getValueToStr(attributeName));
            }
            else if (NULL == xml_attr.first_child()) // If <Attribute> does not have any child like <Value> or <RefSemantic>, it has a single string value.
            {
                ADD_VALUE(xml_attr, amlData->getValueToStr(attributeName));
            }
            else if (IS_VALUE_TYPE_STRING_ARRAY(xml_attr))
            {
                addStringArrayValue(xml_attr, amlData->getValueToStrArr(attributeName));
            }
            else if (IS_VALUE_TYPE_MAP(xml_attr))
            {
                AMLData data = amlData->getValueToAMLData(attributeName);
                setAttributeValue(xml_attr, &data);
            }
            else
            {
                AML_LOG_V(ERROR, TAG, "Invalid AML : <%s> has value of invalid type", attributeName.c_str());
                throw AMLException(INVALID_AML_SCHEMA);
            }
        }

        return;
    }

    void addStringArrayValue(pugi::xml_node xml_ie, const std::vector<std::string> valueArray)
    {
        for (std::size_t i = 0, size = valueArray.size(); i != size; ++i)
        {
            pugi::xml_node xml_child_attr = xml_ie.append_child(ATTRIBUTE);

            //xml_child_attr.append_attribute(NAME) = toString(i);
            //xml_child_attr.append_attribute(ATTRIBUTE_DATA_TYPE) = xml_ie.attribute(ATTRIBUTE_DATA_TYPE).value());
            // Copy attributes from parent node to child node
            for (pugi::xml_attribute_iterator attrIter = xml_ie.attributes_begin(), end = xml_ie.attributes_end(); attrIter != end; ++attrIter)
            {
                xml_child_attr.append_attribute(attrIter->name()) = attrIter->value();
            }

            // The names of child attribute are "1", "2", "3"...
            xml_child_attr.attribute(NAME).set_value(i + 1);

            ADD_VALUE(xml_child_attr, valueArray[i]);
        }

        // As AML Document(BPR MLA, V 1.0.0), 'AttributeDataType' of the parent attribute node should be kept empty.
        xml_ie.attribute(ATTRIBUTE_DATA_TYPE).set_value("");
    }
};

Representation::Representation(const std::string amlFilePath) : m_amlModel (new AMLModel(amlFilePath))
{
}

Representation::~Representation(void)
{
    delete m_amlModel;
}

std::string Representation::getRepresentationId() const
{
    return m_amlModel->constructModelId();
}

AMLObject* Representation::getConfigInfo() const
{
    return m_amlModel->constructConfigAmlObject();
}

std::string Representation::DataToAml(const AMLObject& amlObject) const
{
    pugi::xml_document* xml_doc = m_amlModel->constructXmlDoc(amlObject);
    assert(nullptr != xml_doc);

    m_amlModel->appendModel(xml_doc);

    std::ostringstream stream;
    xml_doc->save(stream);

    delete xml_doc;

    return stream.str();
}

AMLObject* Representation::AmlToData(const std::string& xmlStr) const
{
    pugi::xml_document dataXml;
    pugi::xml_parse_result result = dataXml.load_string(xmlStr.c_str());
    if (pugi::status_ok != result.status)
    {
        AML_LOG(ERROR, TAG, "Failed to load string : Invalid XML");
        throw AMLException(INVALID_XML_STR);
    }

    AMLObject *amlObj = m_amlModel->constructAmlObject(&dataXml);
    assert(nullptr != amlObj);
    return amlObj;
}

AMLObject* Representation::ByteToData(const std::string& byte) const
{
#ifdef _DISABLE_PROTOBUF_
    (void)byte;
    AML_LOG(ERROR, TAG, "ByteToData() is not supported. ('disable_protobuf' build option is enabled)");
    throw AMLException(API_NOT_ENABLED);
#else
    datamodel::CAEXFile* caex = new datamodel::CAEXFile();

    if (false == caex->ParseFromString(byte))
    {
        AML_LOG(ERROR, TAG, "Failed to parse from string : Invalid byte");
        delete caex;
        throw AMLException(INVALID_BYTE_STR);
    }

    pugi::xml_document* xml_doc = m_amlModel->constructXmlDoc();
    assert(nullptr != xml_doc);

    // update CAEX attributes
    pugi::xml_node xml_caex = xml_doc->child(CAEX_FILE);
    xml_caex.attribute("FileName")                      = caex->filename().c_str();
    xml_caex.attribute("SchemaVersion")                 = caex->schemaversion().c_str();
    xml_caex.attribute("xsi:noNamespaceSchemaLocation") = caex->xsi().c_str();
    xml_caex.attribute("xmlns:xsi")                     = caex->xmlns().c_str();

    for (datamodel::InstanceHierarchy ih : caex->instancehierarchy())
    {
        pugi::xml_node xml_ih = xml_caex.append_child(INSTANCE_HIERARCHY);
        VERIFY_NON_NULL_THROW_EXCEPTION(xml_ih);

        xml_ih.append_attribute(NAME) = ih.name().c_str();

        extractProtoInternalElement(xml_ih, &ih);
    }

    caex->clear_instancehierarchy();
    delete caex;

    AMLObject* amlObj = m_amlModel->constructAmlObject(xml_doc);
    assert(nullptr != amlObj);

    delete xml_doc;

    return amlObj;
#endif // _DISABLE_PROTOBUF_
}

std::string Representation::DataToByte(const AMLObject& amlObject) const
{
#ifdef _DISABLE_PROTOBUF_
    (void)amlObject;
    AML_LOG(ERROR, TAG, "DataToByte() is not supported. ('disable_protobuf' build option is enabled)");
    throw AMLException(API_NOT_ENABLED);
#else
    // convert AMLObject to XML object
    pugi::xml_document* xml_doc = m_amlModel->constructXmlDoc(amlObject);
    assert(nullptr != xml_doc);

    // convert XML object to AML proto object
    pugi::xml_node xml_caex = xml_doc->child(CAEX_FILE);
    
    datamodel::CAEXFile* caex = new datamodel::CAEXFile();

    caex->set_filename(xml_caex.attribute("FileName").value());
    caex->set_schemaversion(xml_caex.attribute("SchemaVersion").value());
    caex->set_xsi(xml_caex.attribute("xsi:noNamespaceSchemaLocation").value());
    caex->set_xmlns(xml_caex.attribute("xmlns:xsi").value());

    for (pugi::xml_node xml_ih = xml_caex.child(INSTANCE_HIERARCHY); xml_ih; xml_ih = xml_ih.next_sibling(INSTANCE_HIERARCHY))
    {
        datamodel::InstanceHierarchy* ih = caex->add_instancehierarchy();

        ih->set_name    (xml_ih.attribute(NAME).value());
      //ih->set_version (xml_ih.child_value(VERSION)); // @TODO: required?

        extractInternalElement<datamodel::InstanceHierarchy>(ih, xml_ih);
    }

    delete xml_doc;

    std::string binary;
    bool isSuccess = caex->SerializeToString(&binary);

    caex->clear_instancehierarchy();
    delete caex;

    if (false == isSuccess)
    {
        throw AMLException(SERIALIZE_FAIL);
    }
    return binary;
#endif // _DISABLE_PROTOBUF_
}

#ifndef _DISABLE_PROTOBUF_
template <typename T>
static void extractProtoAttribute(pugi::xml_node xmlNode, T* attr)
{
    for (datamodel::Attribute att : attr->attribute())
    {
        pugi::xml_node xml_attr = xmlNode.append_child(ATTRIBUTE);
        VERIFY_NON_NULL_THROW_EXCEPTION(xml_attr);

        xml_attr.append_attribute(NAME) = att.name().c_str();
        xml_attr.append_attribute(ATTRIBUTE_DATA_TYPE) = att.attributedatatype().c_str();

        extractProtoAttribute(xml_attr, &att);

        if (att.has_value())
        {
            xml_attr.append_child(VALUE).text().set(att.value().c_str());
        }
        if (att.has_refsemantic())
        {
            pugi::xml_node xml_refsemantic = xml_attr.append_child(REF_SEMANTIC);
            xml_refsemantic.append_attribute(CORRESPONDING_ATTRIBUTE_PATH) = att.refsemantic().correspondingattributepath().c_str();
        }
    }

    return;
}

template <typename T>
static void extractProtoInternalElement(pugi::xml_node xmlNode, T* ie)
{
    for (datamodel::InternalElement sie : ie->internalelement())
    {
        pugi::xml_node xml_ie = xmlNode.append_child(INTERNAL_ELEMENT);
        VERIFY_NON_NULL_THROW_EXCEPTION(xml_ie);

        xml_ie.append_attribute(NAME) = sie.name().c_str();
        xml_ie.append_attribute(REF_BASE_SYSTEM_UNIT_PATH) = sie.refbasesystemunitpath().c_str();

        extractProtoAttribute(xml_ie, &sie);
        extractProtoInternalElement(xml_ie, &sie);

        if (nullptr != &sie.supportedroleclass() && 
            nullptr != &sie.supportedroleclass().refroleclasspath())
        {
            pugi::xml_node xml_src = xml_ie.append_child(SUPPORTED_ROLE_CLASS);
            VERIFY_NON_NULL_THROW_EXCEPTION(xml_src);

            xml_src.append_attribute(REF_ROLE_CLASS_PATH) = sie.supportedroleclass().refroleclasspath().c_str();
        }
    }

    return;
}

template <typename T>
static void extractAttribute(T* attr, pugi::xml_node xmlNode)
{
    for (pugi::xml_node xmlAttr = xmlNode.child(ATTRIBUTE); xmlAttr; xmlAttr = xmlAttr.next_sibling(ATTRIBUTE))
    {
        datamodel::Attribute* attr_child = attr->add_attribute();

        attr_child->set_name              (xmlAttr.attribute(NAME).value());
        attr_child->set_attributedatatype (xmlAttr.attribute(ATTRIBUTE_DATA_TYPE).value());
        //attr->set_description       (xmlAttr.child_value(DESCRIPTION)); //@TODO: required?

        extractAttribute<datamodel::Attribute>(attr_child, xmlAttr);

        pugi::xml_node xmlValue = xmlAttr.child(VALUE);
        if (NULL != xmlValue)
        {
            attr_child->set_value(xmlValue.text().as_string());
        }

        pugi::xml_node xmlRefSemantic = xmlAttr.child(REF_SEMANTIC);
        if (NULL != xmlRefSemantic)
        {
            datamodel::RefSemantic* refSemantic = new datamodel::RefSemantic();
            refSemantic->set_correspondingattributepath(xmlRefSemantic.attribute(CORRESPONDING_ATTRIBUTE_PATH).value());
            attr_child->set_allocated_refsemantic(refSemantic);
        }
    }

    return;
}

template <typename T>
static void extractInternalElement(T* ie, pugi::xml_node xmlNode)
{
    for (pugi::xml_node xmlIe = xmlNode.child(INTERNAL_ELEMENT); xmlIe; xmlIe = xmlIe.next_sibling(INTERNAL_ELEMENT))    
    {
        datamodel::InternalElement* ie_child = ie->add_internalelement();

        ie_child->set_name                    (xmlIe.attribute(NAME).value());
        ie_child->set_refbasesystemunitpath   (xmlIe.attribute(REF_BASE_SYSTEM_UNIT_PATH).value());
        
        extractAttribute<datamodel::InternalElement>(ie_child, xmlIe);
        extractInternalElement<datamodel::InternalElement>(ie_child, xmlIe);

        pugi::xml_node xmlSrc = xmlIe.child(SUPPORTED_ROLE_CLASS);
        if (NULL != xmlSrc)
        {
            datamodel::SupportedRoleClass* src = new datamodel::SupportedRoleClass();
            src->set_refroleclasspath(xmlSrc.attribute(REF_ROLE_CLASS_PATH).value());

            ie_child->set_allocated_supportedroleclass(src);
        }
    }

    return;
}
#endif // _DISABLE_PROTOBUF_
