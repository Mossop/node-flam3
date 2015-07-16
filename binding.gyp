{
  "variables": {
    "flam3-version": "<!(cd flam3 && git describe --tags --dirty)"
  },
  "targets": [
    {
      "target_name": "flam3",
      "sources": [ "src/node-flam3.cc",
                   "src/genome.cc" ],
      "dependencies": [ "libflam3" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
    }, {
      "target_name": "libflam3",
      "type": "static_library",
      "sources": [ "flam3/flam3.c",
                   "flam3/filters.c",
                   "flam3/parser.c",
                   "flam3/variations.c",
                   "flam3/interpolation.c",
                   "flam3/palettes.c",
                   "flam3/jpeg.c",
                   "flam3/png.c",
                   "flam3/isaac.c" ],
      "include_dirs": [ "/usr/include/libxml2" ],
      "defines": [ "GIT_REV=\"<(flam3-version)\"",
                   "PACKAGE_DATA_DIR=\"\"" ],
      "xcode_settings": {
        "OTHER_CFLAGS": [ "-g",
                          "-O3",
                          "-std=gnu99",
                          "-ffast-math" ],
      },
      "direct_dependent_settings": {
        "include_dirs": [ "flam3",
                          "/usr/include/libxml2" ],
        "libraries": [ "-lxml2" ]
      }
    }
  ]
}
