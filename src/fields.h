#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

using namespace v8;

enum Property_Type {
  INT = 0,
  DOUBLE = 1
};

typedef struct {
  const char* name;
  Property_Type type;
  size_t offset;
} property_entry;

typedef double point_t[2];
typedef double rgb_t[3];
typedef double rgba_t[4];

void SetIntField(Handle<Object> obj, const char * property, int & value);
void GetIntField(Handle<Object> obj, const char * property, int & value);

void SetDoubleField(Handle<Object> obj, const char * property, double & value);
void GetDoubleField(Handle<Object> obj, const char * property, double & value);

void SetPointField(Handle<Object> obj, const char * property, point_t point);
void GetPointField(Handle<Object> obj, const char * property, point_t point, point_t def);

void SetColorField(Handle<Object> obj, const char * property, rgb_t color);
void GetColorField(Handle<Object> obj, const char * property, rgb_t color, rgb_t def);

void SetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color);
void GetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color, rgba_t def);

void SetPaletteField(Handle<Object> obj, const char * property, flam3_palette palette);
void GetPaletteField(Handle<Object> obj, const char * property, flam3_palette palette);

NAN_GETTER(ValuePropertyGetter);
NAN_GETTER(IntPropertyGetter);
NAN_SETTER(IntPropertySetter);
NAN_GETTER(DoublePropertyGetter);
NAN_SETTER(DoublePropertySetter);

#define DEFINE_READONLY_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(name), \
    ValuePropertyGetter, NULL, value, DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));

#define DEFINE_INT_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(name), \
    IntPropertyGetter, IntPropertySetter, NanNew<External>(value), DEFAULT, DontDelete);

#define DEFINE_DOUBLE_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(name), \
    DoublePropertyGetter, DoublePropertySetter, NanNew<External>(value), DEFAULT, DontDelete);
#endif
