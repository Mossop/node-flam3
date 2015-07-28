#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

#include "fields.h"

using namespace v8;

void SetIntField(Handle<Object> obj, const char * property, int & value) {
  NanScope();

  obj->Set(NanNew<String>(property), NanNew<Number>(value));
}

void GetIntField(Handle<Object> obj, const char * property, int & value) {
  NanScope();

  if (!obj->Has(NanNew<String>(property))) {
    return;
  }

  Local<Value> prop = obj->Get(NanNew<String>(property));
  if (!prop->IsInt32()) {
    return;
  }

  value = prop->Int32Value();
}

void SetDoubleField(Handle<Object> obj, const char * property, double & value) {
  NanScope();

  obj->Set(NanNew<String>(property), NanNew<Number>(value));
}

void GetDoubleField(Handle<Object> obj, const char * property, double & value) {
  NanScope();

  if (!obj->Has(NanNew<String>(property))) {
    return;
  }

  Local<Value> prop = obj->Get(NanNew<String>(property));
  if (!prop->IsNumber()) {
    return;
  }

  value = prop->NumberValue();
}

void SetPointValue(point_t point, double x, double y) {
  point[0] = x;
  point[1] = y;
}

void SetPointField(Handle<Object> obj, const char * property, point_t point) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "x", point[0]);
  SetDoubleField(prop, "y", point[1]);

  obj->Set(NanNew<String>(property), prop);
}

void GetPointField(Handle<Object> obj, const char * property, point_t point) {
  NanScope();

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsObject()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  GetDoubleField(prop, "x", point[0]);
  GetDoubleField(prop, "y", point[1]);
}

void SetColorValue(rgb_t color, double red, double green, double blue) {
  color[0] = red;
  color[1] = green;
  color[2] = blue;
}

void SetColorValue(rgba_t color, double red, double green, double blue, double alpha) {
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = alpha;
}

void SetColorField(Handle<Object> obj, const char * property, rgb_t color) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "red", color[0]);
  SetDoubleField(prop, "green", color[1]);
  SetDoubleField(prop, "blue", color[2]);

  obj->Set(NanNew<String>(property), prop);
}

void GetColorField(Handle<Object> obj, const char * property, rgb_t color) {
  NanScope();

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsObject()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  GetDoubleField(prop, "red", color[0]);
  GetDoubleField(prop, "green", color[1]);
  GetDoubleField(prop, "blue", color[2]);
}

void SetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "red", color[0]);
  SetDoubleField(prop, "green", color[1]);
  SetDoubleField(prop, "blue", color[2]);
  SetDoubleField(prop, "alpha", color[3]);

  obj->Set(NanNew<String>(property), prop);
}

void GetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color) {
  NanScope();

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsObject()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  GetDoubleField(prop, "red", color[0]);
  GetDoubleField(prop, "green", color[1]);
  GetDoubleField(prop, "blue", color[2]);
  GetDoubleField(prop, "alpha", color[3]);
}

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
