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

# ==================== Entry point for scons ======================

# Load common build config
SConscript('build_common/SConscript')

Import('env')

# Get the build directory prefix
build_dir = env.get('BUILD_DIR')

# Build AML DataModel library
SConscript(build_dir + 'SConscript')

## Delete temp file for unittest
#if env.get('TEST') == '1':
#	Command("delete_tmp_file", "", Delete("TEST_DataModel.aml"))
