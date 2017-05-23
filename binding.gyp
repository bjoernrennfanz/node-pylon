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
                'include_dirs' : [
                    '<!(echo %PYLON_DEV_DIR%)/include',
                ],
            }]
        ]
    },
    {
        'target_name': 'pylon',
        'sources': [
            'src/pylon.cc'
        ],
        'include_dirs' : [
            '<!(node -e "require(\'nan\')")'
        ],
        'dependencies': [
             'genicam',
        ],
        'conditions': [
            ['OS=="win"', {
                'include_dirs' : [
                    '<!(echo %PYLON_DEV_DIR%)/include/pylon',
                ],
            }]
        ]
    }]
}