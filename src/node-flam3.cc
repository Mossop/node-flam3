#include <node.h>

extern "C" {
#include "flam3.h"
}

using namespace v8;

void Init(Handle<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  Local<Number> threadCount = Number::New(isolate, flam3_count_nthreads());
  Local<String> version = String::NewFromUtf8(isolate, flam3_version());
  PropertyAttribute constant_attributes = static_cast<PropertyAttribute>(ReadOnly | DontDelete);
  exports->ForceSet(String::NewFromUtf8(isolate, "version"), version, constant_attributes);
  exports->ForceSet(String::NewFromUtf8(isolate, "threadCount"), threadCount, constant_attributes);
}

NODE_MODULE(flam3, Init)
