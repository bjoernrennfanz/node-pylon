# CMake alternative to the node-gyp build.
# Default: real build against Basler pylon SDK 26.09.
# SDK comes from scripts/download-pylon-sdk.sh (PYLON_SDK_BASE_URL, secret);
# PYLON_ROOT points at the extracted SDK (.pylon-sdk in the repo root).
# Mock only as an explicit opt-in: -DNODE_PYLON_USE_MOCK=ON.
cmake_minimum_required(VERSION 3.18)
project(node_pylon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NODE_PYLON_USE_MOCK "Explicit opt-in: build without the Basler pylon SDK (mock)" OFF)

# Node headers + node-addon-api (expects: npm install in the repo root).
execute_process(
  COMMAND node -p "require('node-addon-api').include.replace('\"','')"
  OUTPUT_VARIABLE NODE_ADDON_API_INCLUDE
  OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
execute_process(
  COMMAND node -e "console.log(process.execPath)"
  OUTPUT_VARIABLE NODE_EXE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(NodeJS QUIET)
if(NOT NODEJS_INCLUDE_DIRS)
  execute_process(
    COMMAND node -p "process.config.variables.nodedir || require('path').dirname(require('path').dirname(process.execPath)) + '/include/node'"
    OUTPUT_VARIABLE NODEJS_INCLUDE_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()

if(NOT NODE_PYLON_USE_MOCK AND NOT DEFINED ENV{PYLON_ROOT} AND NOT DEFINED PYLON_ROOT)
  set(PYLON_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.pylon-sdk" CACHE PATH "Basler pylon SDK root (scripts/download-pylon-sdk.sh)")
endif()

add_library(node_pylon SHARED
  src/addon.cc
  src/mock_nodemap.cc
  src/mock_camera.cc
  react-native/shared/pylon_core.cpp
)

target_include_directories(node_pylon PRIVATE
  ${NODEJS_INCLUDE_DIRS}
  ${NODE_ADDON_API_INCLUDE}
  ${CMAKE_CURRENT_SOURCE_DIR}/react-native/shared
)

if(NODE_PYLON_USE_MOCK)
  target_compile_definitions(node_pylon PRIVATE NODE_PYLON_MOCK=1)
else()
  target_compile_definitions(node_pylon PRIVATE NODE_PYLON_MOCK=0)
  if(NOT DEFINED PYLON_ROOT)
    set(PYLON_ROOT "$ENV{PYLON_ROOT}" CACHE PATH "Basler pylon SDK root")
  endif()
  if(PYLON_ROOT STREQUAL "")
    message(FATAL_ERROR "PYLON_ROOT is not set. Run scripts/download-pylon-sdk.sh (needs PYLON_SDK_BASE_URL) or set PYLON_ROOT.")
  endif()
  if(WIN32)
    target_include_directories(node_pylon PRIVATE ${PYLON_ROOT}/Development/include)
    target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/Development/lib/x64)
    target_link_libraries(node_pylon PRIVATE
      PylonBase_v12 PylonUtility_v12
      GenApi_MD_VC142_v3_5_Basler_pylon_v1 GCBase_MD_VC142_v3_5_Basler_pylon_v1)
    # The pylon C++ headers require C++ exceptions and RTTI.
    target_compile_options(node_pylon PRIVATE /EHsc /GR)
  elseif(APPLE)
    target_include_directories(node_pylon PRIVATE ${PYLON_ROOT}/Frameworks/pylon.framework/Headers)
    target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/Frameworks/pylon.framework/Libraries)
    target_link_libraries(node_pylon PRIVATE
      pylonbase pylonutility
      GenApi_gcc_v3_5_Basler_pylon_v1 GCBase_gcc_v3_5_Basler_pylon_v1)
  else()
    target_include_directories(node_pylon PRIVATE ${PYLON_ROOT}/include)
    target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/lib)
    target_link_libraries(node_pylon PRIVATE
      pylonbase pylonutility
      GenApi_gcc_v3_5_Basler_pylon_v1 GCBase_gcc_v3_5_Basler_pylon_v1)
  endif()
endif()

target_compile_definitions(node_pylon PRIVATE NAPI_VERSION=8 NAPI_DISABLE_CPP_EXCEPTIONS)

set_target_properties(node_pylon PROPERTIES
  PREFIX ""
  SUFFIX ".node"
  OUTPUT_NAME "node_pylon"
)

if(APPLE)
  set_target_properties(node_pylon PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
elseif(UNIX)
  set_target_properties(node_pylon PROPERTIES LINK_FLAGS "-Wl,-rpath,${PYLON_ROOT}/lib")
endif()
