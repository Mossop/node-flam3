{
  "variables": {
    "flam3_version": "v3.1.1",
    "flam3_archive": "https://github.com/scottdraves/flam3/archive/<(flam3_version).tar.gz",
    "flam3_dir": "<(SHARED_INTERMEDIATE_DIR)/flam3",
    "flam3_sources": [
      "<(flam3_dir)/flam3.c",
      "<(flam3_dir)/filters.c",
      "<(flam3_dir)/parser.c",
      "<(flam3_dir)/variations.c",
      "<(flam3_dir)/interpolation.c",
      "<(flam3_dir)/palettes.c",
      "<(flam3_dir)/jpeg.c",
      "<(flam3_dir)/png.c",
      "<(flam3_dir)/isaac.c"
    ]
  },
  "targets": [
    {
      "target_name": "libflam3",
      "type": "static_library",
      "dependencies": [ "../node_modules/libxmljs/vendor/libxml/libxml.gyp:libxml" ],
      "actions": [
        {
          "action_name": 'download_flam3',
          "inputs": [],
          "outputs": [ "<(INTERMEDIATE_DIR)/flam3.tar.gz" ],
          "action": ["curl", "-L", "-o", "<(INTERMEDIATE_DIR)/flam3.tar.gz", "<(flam3_archive)" ]
        }, {
          "action_name": 'extract_flam3',
          "inputs": [ "<(INTERMEDIATE_DIR)/flam3.tar.gz" ],
          "outputs": [ "<@(flam3_sources)" ],
          "action": ["tar", "-xmv", "--strip-components", "1", "-C", "<(flam3_dir)", "-f", "<(INTERMEDIATE_DIR)/flam3.tar.gz" ]
        }
      ],
      "sources": [ "<@(flam3_sources)" ],
      "defines": [ "GIT_REV=\"<(flam3_version)\"",
                   "PACKAGE_DATA_DIR=\"\"" ],
      "cflags": [ "-g",
                  "-O3",
                  "-std=gnu99",
                  "-ffast-math" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "-g",
                          "-O3",
                          "-std=gnu99",
                          "-ffast-math" ],
      },
      "direct_dependent_settings": {
        "include_dirs": [ "<(flam3_dir)",
                          "../node_modules/libxmljs/vendor/libxml/include" ],
      }
    }
  ]
}
