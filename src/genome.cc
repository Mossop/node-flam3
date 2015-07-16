#include "genome.h"

Persistent<Function> Genome::constructor;

Genome::Genome() {
  memset(&genome, 0, sizeof(flam3_genome));
  clear_cp(&genome, flam3_defaults_on);
}

Genome::~Genome() {
}

void DoublePropertyGetter(Local<String> property, const PropertyCallbackInfo<Value>& args) {
  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());
  double* field = reinterpret_cast<double*>(reinterpret_cast<char*>(&(obj->genome)) + args.Data()->IntegerValue());

  NanReturnValue(NanNew<Number>(*field));
}

void DoublePropertySetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& args) {
  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());
  double* field = reinterpret_cast<double*>(reinterpret_cast<char*>(&(obj->genome)) + args.Data()->IntegerValue());

  *field = value->NumberValue();
}

#define ADD_DOUBLE_PROPERTY(name) \
  tpl->InstanceTemplate()->SetNativeDataProperty(NanNew<String>(#name), \
    DoublePropertyGetter, DoublePropertySetter, NanNew<Number>(offsetof(flam3_genome, name)))

void Genome::Init(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Genome"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  ADD_DOUBLE_PROPERTY(gamma);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<String>("Genome"), tpl->GetFunction());
}

NAN_METHOD(Genome::New) {
  NanScope();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Genome(...)`
    Genome* obj = new Genome();
    obj->Wrap(args.This());
    NanReturnThis();
  } else {
    // Invoked as plain function `Genome(...)`, turn into construct call.
    Local<Value> argv[0];
    NanReturnValue(NanNew<Function>(constructor)->NewInstance(0, argv));
  }
}
