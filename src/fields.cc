#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

#include "fields.h"

using namespace v8;

rgba_t WHITE = { 1, 1, 1, 1 };

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

void SetPointField(Handle<Object> obj, const char * property, point_t point) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "x", point[0]);
  SetDoubleField(prop, "y", point[1]);

  obj->Set(NanNew<String>(property), prop);
}

void GetPointField(Handle<Object> obj, const char * property, point_t point, point_t def) {
  NanScope();

  memcpy(point, def, sizeof(point_t));

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsObject()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  GetDoubleField(prop, "x", point[0]);
  GetDoubleField(prop, "y", point[1]);
}

void SetColorField(Handle<Object> obj, const char * property, rgb_t color) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "red", color[0]);
  SetDoubleField(prop, "green", color[1]);
  SetDoubleField(prop, "blue", color[2]);

  obj->Set(NanNew<String>(property), prop);
}

void GetColorField(Handle<Object> obj, const char * property, rgb_t color, rgb_t def) {
  NanScope();

  memcpy(color, def, sizeof(rgb_t));

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsObject()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  GetDoubleField(prop, "red", color[0]);
  GetDoubleField(prop, "green", color[1]);
  GetDoubleField(prop, "blue", color[2]);
}

void SetColorAlphaField(Handle<Object> obj, const char * property, rgb_t color) {
  NanScope();

  Local<Object> prop = NanNew<Object>();
  SetDoubleField(prop, "red", color[0]);
  SetDoubleField(prop, "green", color[1]);
  SetDoubleField(prop, "blue", color[2]);
  SetDoubleField(prop, "alpha", color[3]);

  obj->Set(NanNew<String>(property), prop);
}

void GetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color, rgba_t def) {
  NanScope();

  memcpy(color, def, sizeof(rgba_t));

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

void SetPaletteField(Handle<Object> obj, const char * property, flam3_palette palette) {
  NanScope();

  Local<Array> prop = NanNew<Array>(256);
  char index[4];
  for (int i = 0; i < 256; i++) {
    sprintf(index, "%d", i);
    SetColorAlphaField(prop, index, palette[i].color);
  }

  obj->Set(NanNew<String>(property), prop);
}

void GetPaletteField(Handle<Object> obj, const char * property, flam3_palette palette) {
  NanScope();

  // Initialize to all white
  for (int i = 0; i < 256; i++) {
    memcpy(palette[i].color, WHITE, sizeof(rgba_t));
  }

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsArray()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  char index[4];
  for (int i = 0; i < 256; i++) {
    sprintf(index, "%d", i);
    GetColorAlphaField(prop, index, palette[i].color, WHITE);
  }
}

NAN_GETTER(ValuePropertyGetter) {
  NanScope();

  NanReturnValue(args.Data());
}

NAN_GETTER(IntPropertyGetter) {
  NanScope();

  int* field = reinterpret_cast<int*>(Local<External>::Cast(args.Data())->Value());
  NanReturnValue(NanNew<Number>(*field));
}

NAN_SETTER(IntPropertySetter) {
  NanScope();

  int* field = reinterpret_cast<int*>(Local<External>::Cast(args.Data())->Value());
  *field = value->Int32Value();
  NanReturnValue(NanNew<Number>(*field));
}

NAN_GETTER(DoublePropertyGetter) {
  NanScope();

  double* field = reinterpret_cast<double*>(Local<External>::Cast(args.Data())->Value());
  NanReturnValue(NanNew<Number>(*field));
}

NAN_SETTER(DoublePropertySetter) {
  NanScope();

  double* field = reinterpret_cast<double*>(Local<External>::Cast(args.Data())->Value());
  *field = value->NumberValue();
  NanReturnValue(NanNew<Number>(*field));
}
