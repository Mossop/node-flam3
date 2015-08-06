{
  "targets": [
    {
      "target_name": "flam3_bindings",
      "sources": [
        "src/module.cc",
        "src/fields.cc",
        "src/genome.cc",
        "src/transform.cc"
      ],
      "dependencies": [ "deps/libflam3.gyp:libflam3",
                        "deps/deps.gyp:generate-properties" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<(SHARED_INTERMEDIATE_DIR)"
      ],
      "conditions": [
        [ 'OS != "win"', {
          "dependencies": [ "deps/deps.gyp:render-tests" ]
        }]
      ]
    }
  ]
}
