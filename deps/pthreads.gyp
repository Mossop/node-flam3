{
  "variables": {
    "pthreads_source": "ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.zip",
    "pthreads_archive": "<(INTERMEDIATE_DIR)/pthreads.zip",
    "pthreads_dir": "<(SHARED_INTERMEDIATE_DIR)/pthreads"
  },
  "targets": [
    {
      "target_name": "pthreads",
      "type": "none",
      "actions": [
        {
          "action_name": "download",
          "inputs": [ "download.py" ],
          "outputs": [ "<(pthreads_archive)" ],
          "action": [ "python", "download.py", "--url=<(pthreads_source)", "<(pthreads_archive)" ]
        }, {
          "action_name": "extract",
          "inputs": [ "<(pthreads_archive)" ],
          "outputs": [ "<(pthreads_dir)/Pre-built.2/include/pthread.h" ],
          "action": [ "python", "unzip.py", "--strip-components=0", "<(pthreads_archive)", "<(pthreads_dir)" ]
        }, {
          "action_name": "fixlines",
          "inputs": [ "<(pthreads_dir)/Pre-built.2/include/pthread.h" ],
          "outputs": [ "<(pthreads_dir)/Pre-built.2/include/dummy.h" ],
          "action": [ "python", "fixlines.py", "<(pthreads_dir)/Pre-built.2/include/pthread.h" ]
        }
      ],
      "direct_dependent_settings": {
        "defines": [ "PTW32_STATIC_LIB=1" ],
        "include_dirs": [ "<(pthreads_dir)/Pre-built.2/include" ],
        "library_dirs": [ "<(pthreads_dir)/Pre-built.2/lib/x86" ],
        "libraries": [
          "-lpthreadVC2.lib"
        ],
      },
    }
  ]
}
