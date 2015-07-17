{
  "targets": [
    {
      "target_name": "flam3_bindings",
      "sources": [ "src/module.cc",
                   "src/genome.cc" ],
      "dependencies": [ "libflam3.gyp:libflam3" ],
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
    }
  ]
}
