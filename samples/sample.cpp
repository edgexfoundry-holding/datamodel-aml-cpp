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

#include <fstream>
#include <iostream>
#include <string>

#include "Representation.h"
#include "AMLInterface.h"
#include "AMLException.h"

using namespace std;
using namespace AML;

// helper methods
void printAMLData(AMLData amlData, int depth);
void printAMLObject(AMLObject amlObj);
void saveStringToFile(string str, string filePath);

/*
    Raw Data1 (name : "Model")
    {
        "a": "Model_107.113.97.248",
        "b": "SR-P7-970"
    }

    Raw Data2 (name : "Sample")
    {
        "info": {
            "id": "f437da3b",
            "axis": {
                "x": "20",
                "y": "110"
                "z": "80"
            }
        },
        "appendix": [
            "935",
            "52303",
            "1442"
        ]
    }
*/

int main() {
    try{
        // construct Representation object
        Representation* rep = new Representation("sample_data_model.aml");
        cout << "RepresentationId is " << rep->getRepresentationId() << endl;
        cout << "-------------------------------------------------------------" << endl;

        AMLObject* config_amlObj = rep->getConfigInfo();
        printAMLObject(*config_amlObj);
        delete config_amlObj;
        cout << "-------------------------------------------------------------" << endl;

        // create AMLObject
        string deviceId = "SAMPLE001";
        string timeStamp = "123456789";

        AMLObject amlObj(deviceId, timeStamp);

        // create "Model" data
        AMLData model;
        model.setValue("a", "Model_107.113.97.248");
        model.setValue("b", "SR-P7-970");

        // create "Sample" data
        AMLData axis;
        axis.setValue("x", "20");
        axis.setValue("y", "110");
        axis.setValue("z", "80");

        AMLData info;
        info.setValue("id", "f437da3b");
        info.setValue("axis", axis);

        vector<string> appendix;
        appendix.push_back("52303");
        appendix.push_back("935");
        appendix.push_back("1442");

        AMLData sample;
        sample.setValue("info", info);
        sample.setValue("appendix", appendix);


        // Add Datas to AMLObject
        amlObj.addData("Model", model);
        amlObj.addData("Sample", sample);

        printAMLObject(amlObj);

        // Convert AMLObject to AMLstring(XML)
        string aml_string = rep->DataToAml(amlObj);
        cout << aml_string << endl;
        cout << "-------------------------------------------------------------" << endl;
//        saveStringToFile(aml_string, "aml_result.aml");

        // Convert AMLstring(XML) to AMLObject
        AMLObject* data_from_aml = rep->AmlToData(aml_string);
        printAMLObject(*data_from_aml);
        cout << "-------------------------------------------------------------" << endl;

#ifndef _DISABLE_PROTOBUF_
        // Convert AMLObject to Byte(string)
        string byte_string = rep->DataToByte(*data_from_aml);
        cout << byte_string << endl;
        cout << "-------------------------------------------------------------" << endl;
//        saveStringToFile(byte_string, "binary_result");

        // Convert Byte(string) to AMLObject
        AMLObject* data_from_byte = rep->ByteToData(byte_string);
        printAMLObject(*data_from_byte);
        cout << "-------------------------------------------------------------" << endl;

        delete data_from_byte;
#endif // _DISABLE_PROTOBUF_

        delete data_from_aml;
        delete rep;
    }
    catch (const AMLException& e)
    {
        cout << e.what();
    }
}

void printAMLData(AMLData amlData, int depth)
{
    string indent;
    for (int i = 0; i < depth; i++) indent += "    ";

    cout << indent << "{" << endl;

    vector<string> keys = amlData.getKeys();
    for (string key : keys)
    {
        cout << indent << "    \"" << key << "\" : ";

        AMLValueType type = amlData.getValueType(key);
        if (AMLValueType::String == type)
        {
            string valStr = amlData.getValueToStr(key);
            cout << valStr;
        }
        else if (AMLValueType::StringArray == type)
        {
            vector<string> valStrArr = amlData.getValueToStrArr(key);
            cout << "[";
            for (string val : valStrArr)
            {
                cout << val;
                if (val != valStrArr.back()) cout << ", ";
            }
            cout << "]";
        }
        else if (AMLValueType::AMLData == type)
        {
            AMLData valAMLData = amlData.getValueToAMLData(key);
            cout << endl;
            printAMLData(valAMLData, depth + 1);
        }

        if (key != keys.back()) cout << ",";
        cout << endl;
    }
    cout << indent << "}";
}

void printAMLObject(AMLObject amlObj)
{
    cout << "{" << endl;
    cout << "    \"device\" : " << amlObj.getDeviceId() << "," << endl;
    cout << "    \"timestamp\" : " << amlObj.getTimeStamp() << "," << endl;
    cout << "    \"id\" : " << amlObj.getId() << "," << endl;

    vector<string> dataNames = amlObj.getDataNames();

    for (string n : dataNames)
    {
        AMLData data = amlObj.getData(n);

        cout << "    \"" << n << "\" : " << endl;
        printAMLData(data, 1);
        if (n != dataNames.back()) cout << "," << endl;
    }

    cout << "\n}" << endl;
}

void saveStringToFile(string str, string filePath)
{
    std::ofstream out(filePath);
    out << str;
    out.close();
}