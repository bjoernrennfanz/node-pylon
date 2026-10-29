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
      # N-API ABI-Stabilitaet: kein V8-Direktzugriff, keine Exceptions ueber die Grenze.
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
        # Real-SDK-Build (Default): gegen Basler pylon SDK 26.09.
        # SDK-Bezug via scripts/download-pylon-sdk.sh (PYLON_SDK_BASE_URL).
        # Mock nur als explizites Opt-in:
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
        # node_pylon_use_mock=0 -> Real-SDK (Default, ZAU-161). Mock nur opt-in.
        "node_pylon_use_mock%": 0,
        # Ueber env PYLON_ROOT setzbar (CI-Export aus download-Skript);
        # Default: .pylon-sdk im Repo-Root (legt scripts/download-pylon-sdk.sh an).
        "PYLON_ROOT%": "<!(node -p \"process.env.PYLON_ROOT || require('path').join(process.cwd(), '.pylon-sdk')\")"
      }
    }
  ]
}
