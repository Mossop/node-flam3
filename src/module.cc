#include <node.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

#include "genome.h"

using namespace v8;

void ThreadCountGetter(Local<String> property, const PropertyCallbackInfo<Value>& args) {
  NanReturnValue(NanNew<Number>(flam3_count_nthreads()));
}

void VersionGetter(Local<String> property, const PropertyCallbackInfo<Value>& args) {
  NanReturnValue(NanNew<String>(flam3_version()));
}

void Init(Handle<Object> exports, Handle<Value> module, void* priv) {
  exports->SetAccessor(NanNew<String>("version"), VersionGetter);
  exports->SetAccessor(NanNew<String>("threadCount"), ThreadCountGetter);

  Genome::Export(exports);
}

NODE_MODULE(flam3_bindings, Init)
