#include <node.h>
#include <nan.h>
#include <dlfcn.h>

extern "C" {
#include <flam3.h>
}

#include "genome.h"
#include "transform.h"
#include "palette.h"

using namespace v8;

extern int32_t sGenomeCount;

NAN_GETTER(GenomeCountGetter) {
  NanScope();

  NanReturnValue(NanNew<Number>(sGenomeCount));
}

NAN_GETTER(ThreadCountGetter) {
  NanScope();

  NanReturnValue(NanNew<Number>(flam3_count_nthreads()));
}

NAN_GETTER(VersionGetter) {
  NanScope();

  NanReturnValue(NanNew<String>(flam3_version()));
}

#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
void Init(Handle<Object> exports, Handle<Value> module) {
#else
void Init(Handle<Object> exports, Handle<Value> module, void* priv) {
#endif
  // Find the library path and set the palettes environment variable
  Dl_info info;
  if (dladdr(&sGenomeCount, &info)) {
    int len = strlen(info.dli_fname);
    while (info.dli_fname[len - 1] != '/') {
      len--;
    }

    char* path = reinterpret_cast<char*>(malloc(len + 18 + 1));
    strncpy(path, info.dli_fname, len);
    strcpy(path + len, "flam3-palettes.xml");
    setenv("flam3_palettes", path, 1);
  }

  flam3_srandom();

  exports->SetAccessor(NanNew<String>("version"), VersionGetter);
  exports->SetAccessor(NanNew<String>("threadCount"), ThreadCountGetter);
  exports->SetAccessor(NanNew<String>("genomeCount"), GenomeCountGetter);

  Genome::Export(exports);
  Transform::Export(exports);
  Palette::Export(exports);
}

NODE_MODULE(flam3_bindings, Init)
