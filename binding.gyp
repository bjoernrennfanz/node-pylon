{
  "targets": [
    {
      "target_name": "node_pylon",
      "sources": [
        "src/addon.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
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
        "CLANG_CXX_LANGUAGE_STANDARD": "c++17",
        "GCC_ENABLE_CPP_EXCEPTIONS": "NO"
      },
      "conditions": [
        # Mock-Build (Default 1): kompiliert ohne Basler-pylon-SDK.
        # Echter Build: npm run rebuild -- --define=node_pylon_use_mock=0
        #   mit PYLON_ROOT bzw. pylon-Include/Lib-Pfaden (siehe README, ZAU-150).
        [ "node_pylon_use_mock==1", {
          "defines": [ "NODE_PYLON_MOCK=1" ]
        }, {
          "defines": [ "NODE_PYLON_MOCK=0" ],
          "conditions": [
            [ "OS==\"win\"", {
              "include_dirs": [ "<(PYLON_ROOT)/include" ],
              "msvs_settings": {
                "VCLinkerTool": {
                  "AdditionalLibraryDirectories": [ "<(PYLON_ROOT)/lib/x64" ]
                }
              }
            }],
            [ "OS==\"linux\"", {
              "include_dirs": [ "<(PYLON_ROOT)/include" ],
              "libraries": [ "-L<(PYLON_ROOT)/lib", "-lpylonbase", "-lGenApi" ]
            }],
            [ "OS==\"mac\"", {
              "include_dirs": [ "<(PYLON_ROOT)/include" ],
              "libraries": [ "-L<(PYLON_ROOT)/lib", "-lpylonbase", "-lGenApi" ]
            }]
          ]
        }]
      ],
      "variables": {
        # node-gyp: node_pylon_use_mock=1 -> Mock ohne Kamera/SDK.
        "node_pylon_use_mock%": 1,
        # Ueber env PYLON_ROOT oder --define=pylon_root=... setzbar.
        "PYLON_ROOT%": "<!(node -p \"process.env.PYLON_ROOT || '/opt/pylon'\")"
      }
    }
  ]
}
