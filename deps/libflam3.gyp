{
  "variables": {
    "flam3_version": "v3.1.1",
    "flam3_source": "https://github.com/scottdraves/flam3/archive/e0801543538451234d7a8a240ba3b417cbda5b21.zip",
    "flam3_archive": "<(INTERMEDIATE_DIR)/flam3.zip",
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
          "inputs": [ "download.py" ],
          "outputs": [ "<(flam3_archive)" ],
          "action": [ "python", "download.py", "--url=<(flam3_source)", "<(flam3_archive)" ]
        }, {
          "action_name": "extract",
          "inputs": [ "<(flam3_archive)" ],
          "outputs": [ "<(flam3_dir)/flam3.c" ],
          "action": [ "python", "unzip.py", "--strip-components=1", "<(flam3_archive)", "<(flam3_dir)" ]
        }
      ],
    }, {
      "target_name": "fix_defines",
      "actions": [
        {
          "action_name": "append",
          "inputs": [ "<(flam3_dir)/config.h" ],
          "outputs": [ "<(flam3_dir)/config.h.fixed" ],
          "action": [ "python", "append.py", "<(flam3_dir)/config.h", "win_defines.h", "<(flam3_dir)/config.h.fixed" ]
        }
      ]
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
        "<(flam3_dir)/isaac.c"
      ],
      "defines": [ "GIT_REV=\"<(flam3_version)\"",
                   "PACKAGE_DATA_DIR=\"\"" ],
      "cflags": [ "<@(flam3_cflags)" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "<@(flam3_cflags)" ],
      },
      "configurations": {
        "Release": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "Optimization": 0
            },
          },
        }
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
      "conditions": [
        [ 'OS == "win"', {
          "dependencies": [
            "fix_defines",
            "pthreads.gyp:download_pthreads",
            "pthreads.gyp:pthreads"
          ]
        }]
      ]
    }, {
      "target_name": "flam3-render",
      "type": "executable",
      "dependencies": [ "libflam3" ],
      "sources": [ "<(flam3_dir)/flam3-render.c",
                   "<(flam3_dir)/jpeg.c",
                   "<(flam3_dir)/png.c",
                   "<(flam3_dir)/docstring.c" ],
      "libraries": [ "-lpng", "-ljpeg" ],
      "cflags": [ "<@(flam3_cflags)" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "<@(flam3_cflags)" ],
      }
    }
  ]
}
