{
  "targets": [
    {
      "target_name": "render-tests",
      "type": "none",
      "dependencies": [ "libflam3.gyp:flam3-render" ],
      "actions": [
        {
          "action_name": 'render_tests',
          "inputs": [ "render.py", "../test/data/test0.flam3", "../test/data/test1.flam3" ],
          "outputs": [ "<(PRODUCT_DIR)/tests/test0.png", "<(PRODUCT_DIR)/tests/test1.png" ],
          "action": [ "python", "render.py", "<(PRODUCT_DIR)", "../test/data", "<(PRODUCT_DIR)/tests" ]
        }
      ],
    }, {
      "target_name": "generate-properties",
      "type": "none",
      "actions": [
        {
          "action_name": "generate-genome-properties",
          "inputs": [ "../src/write_header.py", "../src/genome_properties" ],
          "outputs": [ "<(SHARED_INTERMEDIATE_DIR)/includes/genome_properties.h" ],
          "action": [ "python", "<@(_inputs)", "<@(_outputs)", "--struct=flam3_genome", "--prefix=GENOME" ]
        }, {
          "action_name": "generate-xform-properties",
          "inputs": [ "../src/write_header.py", "../src/xform_properties" ],
          "outputs": [ "<(SHARED_INTERMEDIATE_DIR)/includes/xform_properties.h" ],
          "action": [ "python", "<@(_inputs)", "<@(_outputs)", "--struct=flam3_xform", "--prefix=XFORM" ]
        }, {
          "action_name": "generate-transform-properties",
          "inputs": [ "../src/write_variations.py", "../src/variation_properties" ],
          "outputs": [ "<(SHARED_INTERMEDIATE_DIR)/includes/variation_properties.h" ],
          "action": [ "python", "<@(_inputs)", "<@(_outputs)" ]
        }
      ]
    }
  ]
}
