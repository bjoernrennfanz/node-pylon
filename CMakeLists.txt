# CMake alternative to the node-gyp build.
# Always builds against the real Basler pylon SDK 26.09.
# SDK comes from scripts/download-pylon-sdk.sh (PYLON_SDK_BASE_URL, secret);
# PYLON_ROOT points at the extracted SDK (.pylon-sdk in the repo root).
cmake_minimum_required(VERSION 3.18)
project(node_pylon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

if(NOT DEFINED ENV{PYLON_ROOT} AND NOT DEFINED PYLON_ROOT)
  set(PYLON_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.pylon-sdk" CACHE PATH "Basler pylon SDK root (scripts/download-pylon-sdk.sh)")
endif()

add_library(node_pylon SHARED
  src/addon.cc
  src/buffer_factory.cc
  src/camera_wrap.cc
  src/defect_pixel.cc
  src/device_info.cc
  src/discovery.cc
  src/error_map.cc
  src/feature_persistence.cc
  src/grab_result.cc
  src/image_pipeline.cc
  src/node_map.cc
  src/pylon_image.cc
  src/wait_wrap.cc
)

target_include_directories(node_pylon PRIVATE
  ${NODEJS_INCLUDE_DIRS}
  ${NODE_ADDON_API_INCLUDE}
)

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
  # Physical Versions/A paths (top-level framework symlinks depend on the
  # extractor; the versioned dirs are always real). The framework headers
  # are flat (no pylon/ subdir), so mirror the node-gyp symlink farm here:
  # <build>/pylon-include/pylon -> Headers for #include <pylon/...>,
  # Headers/GenICam for #include <Base/...>/<GenApi/...>.
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pylon-include)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_CURRENT_BINARY_DIR}/pylon-include/pylon
  )
  file(CREATE_LINK
    ${PYLON_ROOT}/Frameworks/pylon.framework/Versions/A/Headers
    ${CMAKE_CURRENT_BINARY_DIR}/pylon-include/pylon SYMBOLIC)
  target_include_directories(node_pylon PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/pylon-include
    ${PYLON_ROOT}/Frameworks/pylon.framework/Versions/A/Headers/GenICam)
  target_link_directories(node_pylon PRIVATE ${PYLON_ROOT}/Frameworks/pylon.framework/Versions/A/Libraries)
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
