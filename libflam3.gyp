{
  "variables": {
    "flam3-version": "3.1.1",
    "flam3-archive": "https://github.com/scottdraves/flam3/archive/v<(flam3-version).tar.gz"
  },
  "targets": [
    {
      "target_name": "libflam3",
      "type": "static_library",
      "dependencies": [ "action_before_build" ],
      "sources": [ "<(SHARED_INTERMEDIATE_DIR)/flam3/flam3.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/filters.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/parser.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/variations.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/interpolation.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/palettes.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/jpeg.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/png.c",
                   "<(SHARED_INTERMEDIATE_DIR)/flam3/isaac.c" ],
      "defines": [ "GIT_REV=\"<(flam3-version)\"",
                   "PACKAGE_DATA_DIR=\"\"" ],
      "cflags": [ "-g",
                  "-O3",
                  "-std=gnu99",
                  "-ffast-math",
                  "<!@(xml2-config --cflags)" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "-g",
                          "-O3",
                          "-std=gnu99",
                          "-ffast-math",
                          "<!@(xml2-config --cflags)" ],
      },
      "direct_dependent_settings": {
        "cflags": [ "<!@(xml2-config --cflags)" ],
        "include_dirs": [ "<(SHARED_INTERMEDIATE_DIR)/flam3" ],
        "libraries": [ "<!@(xml2-config --libs)" ],
        "xcode_settings": {
          "OTHER_CFLAGS": [ "<!@(xml2-config --cflags)" ],
        }
      }
    }, {
      "target_name": "action_before_build",
      "type": "none",
      "hard_dependency": 1,
      "actions": [
        {
          "action_name": 'download_flam3',
          "inputs": [],
          "outputs": [
            "<(SHARED_INTERMEDIATE_DIR)/flam3.zip"
          ],
          "action": ["curl", "-L", "-o", "<(SHARED_INTERMEDIATE_DIR)/flam3.tar.gz", "<(flam3-archive)" ]
        }, {
          "action_name": 'extract_flam3',
          "inputs": [
            "<(SHARED_INTERMEDIATE_DIR)/flam3.tar.gz"
          ],
          "outputs": [
            "<(SHARED_INTERMEDIATE_DIR)/flam3/flam3.c"
          ],
          "action": ["tar", "-xv", "--strip-components", "1", "-C", "<(SHARED_INTERMEDIATE_DIR)/flam3", "-f", "<(SHARED_INTERMEDIATE_DIR)/flam3.tar.gz" ]
        }
      ]
    }
  ]
}
