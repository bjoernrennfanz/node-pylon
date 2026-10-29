{
  "targets": [
    {
      "target_name": "node_pylon",
      "sources": [
        "src/addon.cc",
        "src/mock_nodemap.cc",
        "src/mock_camera.cc",
        "react-native/shared/pylon_core.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "react-native/shared"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      # N-API ABI stability: no direct V8 access, no exceptions across the boundary.
      "defines": [
        "NAPI_VERSION=8",
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ],
      "cflags_cc": [
        "-std=c++17"
      ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "AdditionalOptions": [ "/std:c++17" ]
        }
      },
      "xcode_settings": {
        "CLANG_CXX_LANGUAGE_STANDARD": "c++17"
      },
      "conditions": [
        # Real-SDK build (default) against Basler pylon SDK 26.09.
        # SDK comes from scripts/download-pylon-sdk.sh (PYLON_SDK_BASE_URL).
        # Mock only as an explicit opt-in:
        #   npm run rebuild:mock  (node-gyp ... --define=node_pylon_use_mock=1)
        [ "node_pylon_use_mock==1", {
          "defines": [ "NODE_PYLON_MOCK=1" ]
        }, {
          "defines": [ "NODE_PYLON_MOCK=0" ],
          "conditions": [
            [ "OS==\"win\"", {
              "include_dirs": [ "<(PYLON_ROOT)/Development/include" ],
              "libraries": [
                "<(PYLON_ROOT)/Development/lib/x64/PylonBase_v12.lib",
                "<(PYLON_ROOT)/Development/lib/x64/PylonUtility_v12.lib",
                "<(PYLON_ROOT)/Development/lib/x64/GenApi_MD_VC142_v3_5_Basler_pylon_v1.lib",
                "<(PYLON_ROOT)/Development/lib/x64/GCBase_MD_VC142_v3_5_Basler_pylon_v1.lib"
              ]
            }],
            [ "OS==\"linux\"", {
              "include_dirs": [ "<(PYLON_ROOT)/include" ],
              "libraries": [
                "-L<(PYLON_ROOT)/lib",
                "-lpylonbase",
                "-lpylonutility",
                "-lGenApi_gcc_v3_5_Basler_pylon_v1",
                "-lGCBase_gcc_v3_5_Basler_pylon_v1"
              ],
              "ldflags": [
                "-Wl,-E",
                "-Wl,-rpath,<(PYLON_ROOT)/lib"
              ]
            }],
            [ "OS==\"mac\"", {
              "include_dirs": [ "<(PYLON_ROOT)/Frameworks/pylon.framework/Headers" ],
              "libraries": [
                "-L<(PYLON_ROOT)/Frameworks/pylon.framework/Libraries",
                "-lpylonbase",
                "-lpylonutility",
                "-lGenApi_gcc_v3_5_Basler_pylon_v1",
                "-lGCBase_gcc_v3_5_Basler_pylon_v1"
              ],
              "xcode_settings": {
                "OTHER_LDFLAGS": [
                  "-Wl,-rpath,<(PYLON_ROOT)/Frameworks/pylon.framework/Libraries"
                ]
              }
            }]
          ]
        }]
      ],
      "variables": {
        # node_pylon_use_mock=0 -> real SDK (default). Mock only opt-in.
        "node_pylon_use_mock%": 0,
        # Settable via env PYLON_ROOT (CI export from the download script);
        # default: .pylon-sdk in the repo root (created by scripts/download-pylon-sdk.sh).
        "PYLON_ROOT%": "<!(node -p \"process.env.PYLON_ROOT || require('path').join(process.cwd(), '.pylon-sdk')\")"
      }
    }
  ]
}
