#include <node.h>

extern "C" {
#include "flam3.h"
}

using namespace v8;

void ThreadCount(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<Number> threads = Number::New(isolate, flam3_count_nthreads());
  args.GetReturnValue().Set(threads);
}

void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "threadCount", ThreadCount);
}

NODE_MODULE(flam3, init)
