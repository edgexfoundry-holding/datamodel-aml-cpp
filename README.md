# DataModel AML library (cpp)
datamodel-aml-cpp is a library which provides the way to present raw data(key/value based) to AutomationML(AML) standard format.
 - Transform raw data to AML data(XML).
 - Serialization / Deserialization AML data using protobuf.


## Prerequisites ##
- SCons
  - Version : 2.3.0 or above
  - [How to install](http://scons.org/doc/2.3.0/HTML/scons-user/c95.html)

- Protobuf
  - Version : 3.4.0(mandatory)
  - Protobuf will be installed by build option (See 'How to build')
  - Refer to the links below for manual installation.
    - [Where to download](https://github.com/google/protobuf/releases/tag/v3.4.0)
    - [How to install](https://github.com/google/protobuf/blob/master/src/README.md)

## How to build ##
1. Goto: ~/datamodel-aml-cpp/
2. Run the script:

   ```
   ./build.sh <options>         : Native build for x86_64
   ./build_32.sh <options>      : Native build for x86
   ./build_arm.sh <options>     : Native build for armhf [Raspberry pi board])
   ./build_arm64.sh <options>   : Native build for arm64 [Raspberry pi board])
   ```
**Notes** </br>
(a) For getting help about script option: **$ ./build.sh --help** </br>
(b) If you build for the first time, set <i>install_prerequisites</i> option true. (e.g. $./build.sh --install_prerequisites=true)<br> Then it will install the required libraries. In this case, script needs sudo permission for installing the libraries. In future need for sudo will be removed by installing those libraries in aml library.


## How to run ##

### Prerequisites ###
 Built datamodel-aml-cpp library

### Sample ###
1. Goto: ~/datamodel-aml-cpp/out/linux/{ARCH}/{MODE}/samples/
2. export LD_LIBRARY_PATH=../
3. Run the sample:
    ```
     ./sample
    ```

## Usage guide for datamodel-aml-cpp library (for microservices)

1. The microservice which wants to use aml APIs has to link following libraries:</br></br>
   **(A) If microservice wants to link aml dynamically following are the libraries it needs to link:**</br>
        - aml.so</br>
   **(B) If microservice wants to link aml statically following are the libraries it needs to link:**</br>
        - aml.a</br>
2. Reference aml library APIs : [docs/docs/html/index.html](docs/docs/html/index.html)


</br></br>
