#include <node.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

#include "genome.h"

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
  exports->SetAccessor(NanNew<String>("version"), VersionGetter);
  exports->SetAccessor(NanNew<String>("threadCount"), ThreadCountGetter);
  exports->SetAccessor(NanNew<String>("genomeCount"), GenomeCountGetter);

  Genome::Export(exports);
}

NODE_MODULE(flam3_bindings, Init)
