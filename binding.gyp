{
  'targets': [
    {
        'target_name': 'alsa_cardinfo',
        'include_dirs': [
            'src'
        ],
        'sources': [
            'src/alsa-cardinfo.c'
        ],
        'link_settings': {
            'libraries': [
            '-lasound'
            ]
        }
    }
  ]
}