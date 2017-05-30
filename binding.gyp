{
'targets': [
    {
        'target_name': 'genicam',
        'sources': [
            'src/genicam.cc'
        ],
        'include_dirs' : [
            '<!(node -e "require(\'nan\')")'
        ],
        'conditions': [
            ['OS=="win"', {
                'configurations': {
                    'Debug': {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeTypeInfo': 'true',
                            },
                        }
                    },
                    'Release': {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeTypeInfo': 'true',
                            },
                        }
                    },
                },
                'msvs_settings': {
                    'VCCLCompilerTool': {
                        'AdditionalOptions': [
                            '/EHsc'
                        ]
                    }
                },
                'include_dirs' : [
                    '<!(echo %PYLON_DEV_DIR%)/include'
                ]
            }],
            ['OS=="win" and target_arch=="ia32"', {
                'libraries': [
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib'
                ]
            }],
            ['OS=="win" and target_arch=="x64"', {
                'libraries': [
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib'
                ]
            }],
            ['OS=="mac"', {
                'xcode_settings': {
                    'LD_RUNPATH_SEARCH_PATHS': [ '/Library/Frameworks' ],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10',
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                    'GCC_ENABLE_CPP_RTTI': 'YES'
                },
                'mac_framework_dirs': [
                    '/Library/Frameworks'
                ],
                'include_dirs' : [
                    '/Library/Frameworks/pylon.framework/Headers/GenICam',
                ],
                'libraries': [
                    '-framework', 'pylon'
                ]
            }]
        ]
    },
    {
        'target_name': 'pylon',
        'sources': [
            'src/pylon.cc',
            'src/pylon/DeviceInfo.cc',
            'src/pylon/InstantCamera.cc',
            'src/pylon/PylonDevice.cc',
            'src/pylon/TlFactory.cc'
        ],
        'include_dirs' : [
            '<!(node -e "require(\'nan\')")'
        ],
        'dependencies': [
             'genicam',
        ],
        'conditions': [
            ['OS=="win"', {
                'configurations': {
                    'Debug': {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeTypeInfo': 'true',
                            },
                        }
                    },
                    'Release': {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'RuntimeTypeInfo': 'true',
                            },
                        }
                    },
                },
                'msvs_settings': {
                    'RuntimeTypeInfo': 'true',
                    'VCCLCompilerTool': {
                        'AdditionalOptions': [
                            '/EHsc'
                        ]
                    }
                },
                'include_dirs' : [
                    '<!(echo %PYLON_DEV_DIR%)/include',
                    '<!(echo %PYLON_DEV_DIR%)/include/pylon',
                ]
            }],
            ['OS=="win" and target_arch=="ia32"', {
                'libraries': [
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/PylonBase_MD_VC120_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/PylonGUI_MD_VC120_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/Win32/PylonUtility_MD_VC120_v5_0.lib'
                ]
            }],
            ['OS=="win" and target_arch=="x64"', {
                'libraries': [
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/GCBase_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/GenApi_MD_VC120_v3_0_Basler_pylon_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/PylonBase_MD_VC120_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/PylonGUI_MD_VC120_v5_0.lib',
                    '<!(echo %PYLON_DEV_DIR%)/lib/x64/PylonUtility_MD_VC120_v5_0.lib'
                ]
            }],
            ['OS=="mac"', {
                'xcode_settings': {
                    'LD_RUNPATH_SEARCH_PATHS': [ '/Library/Frameworks' ],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10',
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                    'GCC_ENABLE_CPP_RTTI': 'YES'
                },
                'mac_framework_dirs': [
                    '/Library/Frameworks'
                ],
                'include_dirs': [
                    '/Library/Frameworks/pylon.framework/Headers/GenICam'
                ],
                'libraries': [
                    '-framework', 'pylon'
                ]
            }]
        ]
    }]
}
