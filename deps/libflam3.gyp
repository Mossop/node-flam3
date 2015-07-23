{
  "variables": {
    "flam3_version": "v3.1.1",
    "flam3_archive": "https://github.com/scottdraves/flam3/archive/<(flam3_version).tar.gz",
    "flam3_dir": "<(SHARED_INTERMEDIATE_DIR)/flam3",
    "flam3_cflags": [
      "-g",
      "-O3",
      "-std=gnu99",
      "-ffast-math"
    ]
  },
  "targets": [
    {
      "target_name": "download_flam3",
      "type": "none",
      "actions": [
        {
          "action_name": "download",
          "inputs": [],
          "outputs": [ "<(INTERMEDIATE_DIR)/flam3.tar.gz" ],
          "action": [ "curl", "-L", "-o", "<(INTERMEDIATE_DIR)/flam3.tar.gz", "<(flam3_archive)" ]
        }, {
          "action_name": "extract",
          "inputs": [ "<(INTERMEDIATE_DIR)/flam3.tar.gz" ],
          "outputs": [ "<(flam3_dir)/flam3.c" ],
          "action": [ "tar", "-xmv", "--strip-components", "1", "-C", "<(flam3_dir)", "-f", "<(INTERMEDIATE_DIR)/flam3.tar.gz" ]
        }
      ],
    }, {
      "target_name": "libflam3",
      "type": "static_library",
      "dependencies": [ "download_flam3",
                        "../node_modules/libxmljs/vendor/libxml/libxml.gyp:libxml" ],
      "sources": [
        "<(flam3_dir)/flam3.c",
        "<(flam3_dir)/filters.c",
        "<(flam3_dir)/parser.c",
        "<(flam3_dir)/variations.c",
        "<(flam3_dir)/interpolation.c",
        "<(flam3_dir)/palettes.c",
        "<(flam3_dir)/jpeg.c",
        "<(flam3_dir)/png.c",
        "<(flam3_dir)/isaac.c"
      ],
      "defines": [ "GIT_REV=\"<(flam3_version)\"",
                   "PACKAGE_DATA_DIR=\"\"" ],
      "cflags": [ "<@(flam3_cflags)" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "<@(flam3_cflags)" ],
      },
      "direct_dependent_settings": {
        "include_dirs": [ "<(flam3_dir)",
                          "../node_modules/libxmljs/vendor/libxml/include" ],
      },
      "copies": [
        {
          "destination": "<(PRODUCT_DIR)",
          "files": [ "<(SHARED_INTERMEDIATE_DIR)/flam3/flam3-palettes.xml" ]
        }
      ],
    }, {
      "target_name": "flam3-render",
      "type": "executable",
      "dependencies": [ "libflam3" ],
      "sources": [ "<(flam3_dir)/flam3-render.c",
                   "<(flam3_dir)/docstring.c" ],
      "libraries": [ "-lpng", "-ljpeg" ],
      "cflags": [ "<@(flam3_cflags)" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "<@(flam3_cflags)" ],
      }
    }
  ]
}
