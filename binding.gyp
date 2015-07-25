{
  "targets": [
    {
      "target_name": "render-tests",
      "type": "none",
      "dependencies": [ "deps/libflam3.gyp:flam3-render" ],
      "actions": [
        {
          "action_name": 'render_tests',
          "inputs": [ "src/render.py", "test/data/test0.flam3", "test/data/test1.flam3" ],
          "outputs": [ "<(PRODUCT_DIR)/tests/test0.png", "<(PRODUCT_DIR)/tests/test1.png" ],
          "action": [ "python", "src/render.py", "<(PRODUCT_DIR)", "test/data", "<(PRODUCT_DIR)/tests" ]
        }
      ],
    }, {
      "target_name": "generate-properties",
      "type": "none",
      "actions": [
        {
          "action_name": "generate",
          "inputs": [ "src/write_header.py", "src/genome_properties" ],
          "outputs": [ "<(SHARED_INTERMEDIATE_DIR)/includes/properties.h" ],
          "action": [ "python", "src/write_header.py", "src/genome_properties", "<(SHARED_INTERMEDIATE_DIR)/includes/properties.h" ]
        }
      ]
    }, {
      "target_name": "flam3_bindings",
      "sources": [
        "src/module.cc",
        "src/fields.cc",
        "src/genome.cc",
        "src/palette.cc"
      ],
      "dependencies": [ "deps/libflam3.gyp:libflam3",
                        "generate-properties",
                        "render-tests" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<(SHARED_INTERMEDIATE_DIR)"
      ],
    }
  ]
}
