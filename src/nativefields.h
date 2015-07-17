#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

using namespace v8;

NAN_GETTER(DoublePropertyGetter) {
  NanScope();

  Local<External> ptr = Local<External>::Cast(args.Data());
  double* field = reinterpret_cast<double*>(ptr->Value());

  NanReturnValue(NanNew<Number>(*field));
}

NAN_SETTER(DoublePropertySetter) {
  NanScope();

  Local<External> ptr = Local<External>::Cast(args.Data());
  double* field = reinterpret_cast<double*>(ptr->Value());

  *field = value->NumberValue();
}

NAN_GETTER(IntPropertyGetter) {
  NanScope();

  Local<External> ptr = Local<External>::Cast(args.Data());
  int* field = reinterpret_cast<int*>(ptr->Value());

  NanReturnValue(NanNew<Number>(*field));
}

NAN_SETTER(IntPropertySetter) {
  NanScope();

  Local<External> ptr = Local<External>::Cast(args.Data());
  int* field = reinterpret_cast<int*>(ptr->Value());

  *field = value->IntegerValue();
}

#define DEFINE_DBL_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    DoublePropertyGetter, DoublePropertySetter, NanNew<External>(&(field)));

#define DEFINE_STRUCT_DBL_PROPERTY(strct, field) DEFINE_DBL_PROPERTY(field, strct->field)

#define DEFINE_INT_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    IntPropertyGetter, IntPropertySetter, NanNew<External>(&(field)));

#define DEFINE_STRUCT_INT_PROPERTY(strct, field) DEFINE_INT_PROPERTY(field, strct->field)
