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
    SetColorValue(palette[i].color, 1, 1, 1, 1);
  }

  Local<Value> p = obj->Get(NanNew<String>(property));
  if (p.IsEmpty() || !p->IsArray()) {
    return;
  }
  Local<Object> prop = p->ToObject();

  char index[4];
  for (int i = 0; i < 256; i++) {
    sprintf(index, "%d", i);
    GetColorAlphaField(prop, index, palette[i].color);
  }
}

NAN_GETTER(ValuePropertyGetter) {
  NanScope();

  NanReturnValue(args.Data());
}
