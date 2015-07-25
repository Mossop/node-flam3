#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

#include "fields.h"

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

NAN_GETTER(ValuePropertyGetter) {
  NanScope();

  NanReturnValue(args.Data());
}

Color::Color(double colors[], int count) {
  NanScope();

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);

  Local<Object> jsObj = tpl->NewInstance();
  Wrap(jsObj);

  memcpy(this->colors, colors, sizeof(double) * count);

  jsObj->SetAccessor(NanNew<String>("red"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("green"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("blue"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);

  if (count >= 4) {
    jsObj->SetAccessor(NanNew<String>("alpha"), GetProperty, SetProperty,
      Local<Value>(), DEFAULT, DontDelete);
  }
  else {
    this->colors[3] = -1;
  }
}

Color::~Color() {
}

double* Color::GetPropertyPtr(const char* name) {
  if (strcmp("red", name) == 0) {
    return &colors[0];
  }
  else if (strcmp("green", name) == 0) {
    return &colors[1];
  }
  else if (strcmp("blue", name) == 0) {
    return &colors[2];
  }
  else if (strcmp("alpha", name) == 0 && colors[3] >= 0) {
    return &colors[3];
  }
  else {
    return NULL;
  }
}

NAN_GETTER(Color::GetProperty) {
  NanScope();

  NanUtf8String name(property);
  Color* entry = ObjectWrap::Unwrap<Color>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    NanReturnValue(NanNew<Number>(*result));
  }

  NanReturnUndefined();
}

NAN_SETTER(Color::SetProperty) {
  NanScope();

  NanUtf8String name(property);
  Color* entry = ObjectWrap::Unwrap<Color>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    *result = value->NumberValue();
    return;
  }
}

Point::Point(double coords[2]) {
  NanScope();

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);

  Local<Object> jsObj = tpl->NewInstance();
  Wrap(jsObj);

  memcpy(this->coords, coords, sizeof(double) * 2);

  jsObj->SetAccessor(NanNew<String>("x"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("y"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
}

Point::~Point() {
}

double* Point::GetPropertyPtr(const char* name) {
  if (strcmp("x", name) == 0) {
    return &coords[0];
  }
  else if (strcmp("y", name) == 0) {
    return &coords[1];
  }
  else {
    return NULL;
  }
}

NAN_GETTER(Point::GetProperty) {
  NanScope();

  NanUtf8String name(property);
  Point* entry = ObjectWrap::Unwrap<Point>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    NanReturnValue(NanNew<Number>(*result));
  }

  NanReturnUndefined();
}

NAN_SETTER(Point::SetProperty) {
  NanScope();

  NanUtf8String name(property);
  Point* entry = ObjectWrap::Unwrap<Point>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    *result = value->NumberValue();
    return;
  }
}
