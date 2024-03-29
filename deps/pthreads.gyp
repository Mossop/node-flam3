{
  "variables": {
    "pthreads_source": "ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.zip",
    "pthreads_archive": "<(INTERMEDIATE_DIR)/pthreads.zip",
    "pthreads_dir": "<(SHARED_INTERMEDIATE_DIR)/pthreads"
  },
  "targets": [
    {
      "target_name": "download_pthreads",
      "link_dependency": 1,
      "type": "none",
      "actions": [
        {
          "action_name": "download",
          "inputs": [ "download.py" ],
          "outputs": [ "<(pthreads_archive)" ],
          "action": [ "python", "<@(_inputs)", "--url=<(pthreads_source)", "<@(_outputs)" ]
        }, {
          "action_name": "extract",
          "inputs": [ "unzip.py", "<(pthreads_archive)" ],
          "outputs": [ "<(pthreads_dir)/Pre-built.2/include/pthread.h" ],
          "action": [ "python", "<@(_inputs)", "--strip-components=0", "<(pthreads_dir)" ]
        }, {
          "action_name": "fixlines",
          "inputs": [ "fixlines.py", "<(pthreads_dir)/Pre-built.2/include/pthread.h" ],
          "outputs": [ "<(pthreads_dir)/Pre-built.2/include/pthread.h.fixed" ],
          "action": [ "python", "<@(_inputs)", "<@(_outputs)" ]
        }
      ],
    }, {
      "target_name": "pthreads",
      "type": "none",
      "copies": [
        {
          "destination": "<(PRODUCT_DIR)",
          "files": [ "<(SHARED_INTERMEDIATE_DIR)/pthreads/Pre-built.2/dll/<(host_arch)/pthreadVC2.dll" ]
        }
      ],
      "direct_dependent_settings": {
        "include_dirs": [ "<(pthreads_dir)/Pre-built.2/include" ],
      },
      "all_dependent_settings": {
        "libraries": [ "<(pthreads_dir)/Pre-built.2/lib/<(host_arch)/pthreadVC2.lib" ]
      },
    }
  ]
}
