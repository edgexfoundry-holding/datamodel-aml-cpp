###############################################################################
# Copyright 2018 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###############################################################################

################ AML DataModel build script ##################

import os

Import('env')

aml_env = env.Clone()
target_os = aml_env.get('TARGET_OS')
target_arch = aml_env.get('TARGET_ARCH')
disable_protobuf = aml_env.get('DISABLE_PROTOBUF')

if aml_env.get('RELEASE'):
    aml_env.AppendUnique(CCFLAGS=['-Os'])
else:
    aml_env.AppendUnique(CCFLAGS=['-g'])

aml_env.AppendUnique(CPPPATH=[
        './extlibs/pugixml/pugixml-1.8/src',
        './include',
        './include/logger',
        './include/internal'
])

if not disable_protobuf:
    aml_env.AppendUnique(CPPPATH=['./protobuf'])
    aml_env.PrependUnique(LIBS=['protobuf'])
else:
    aml_env.AppendUnique(CPPDEFINES = ['_DISABLE_PROTOBUF_'])

if target_os not in ['windows']:
    aml_env.AppendUnique(
        CXXFLAGS=['-O2', '-g', '-Wall', '-fPIC', '-fmessage-length=0', '-std=c++0x', '-I/usr/local/include'])

if target_os not in ['windows']:
    aml_env.AppendUnique(LINKFLAGS=['-Wl,--no-undefined'])

if target_os in ['linux']:
    aml_env.AppendUnique(LIBS=['pthread'])

if target_os in ['linux']:
    if not env.get('RELEASE'):
        aml_env.PrependUnique(LIBS=['gcov'])
        aml_env.AppendUnique(CXXFLAGS=['--coverage'])

AML_DIR = '.'
aml_env.AppendUnique(aml_src = [aml_env.Glob(os.path.join(AML_DIR, 'extlibs/pugixml/pugixml-1.8/src', '*.cpp')),
                                aml_env.Glob(os.path.join(AML_DIR, 'src', '*.cpp')),
                                aml_env.Glob(os.path.join(AML_DIR, 'src', 'logger', '*.cpp'))])

if not disable_protobuf:
    aml_env.AppendUnique(aml_src = [aml_env.Glob(os.path.join(AML_DIR, 'protobuf', '*.cc'))])

amlshared = aml_env.SharedLibrary('aml', aml_env.get('aml_src'))
amlstatic = aml_env.StaticLibrary('aml', aml_env.get('aml_src'))

# Go to build AML DataModel sample apps
if target_os == 'linux':
       SConscript('samples/SConscript')

# Go to build AML DataModel unit test cases
if target_os == 'linux':
    if target_arch in ['x86', 'x86_64']:
        SConscript('unittests/SConscript')
