# CMake-Alternative zum node-gyp-Build (ZAU-149 Scaffold).
# Echter pylon-Build folgt in ZAU-150; Mock-Build ist Default ohne SDK.
cmake_minimum_required(VERSION 3.18)
project(node_pylon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NODE_PYLON_USE_MOCK "Build ohne Basler-pylon-SDK (Mock)" ON)

# Node-Header + node-addon-api (erwartet: npm install im Repo-Root).
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
  set(PYLON_ROOT "$ENV{PYLON_ROOT}" CACHE PATH "Basler pylon SDK root")
  target_include_directories(node_pylon PRIVATE ${PYLON_ROOT}/include)
  if(WIN32)
    target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/lib/x64)
  else()
    target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/lib)
    target_link_libraries(node_pylon PRIVATE pylonbase GenApi)
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
