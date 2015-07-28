#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

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

void SetPointValue(point_t point, double x, double y);
void SetPointField(Handle<Object> obj, const char * property, point_t point);
void GetPointField(Handle<Object> obj, const char * property, point_t point);

void SetColorValue(rgb_t color, double red, double green, double blue);
void SetColorValue(rgba_t color, double red, double green, double blue, double alpha);
void SetColorField(Handle<Object> obj, const char * property, rgb_t color);
void GetColorField(Handle<Object> obj, const char * property, rgb_t color);

void SetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color);
void GetColorAlphaField(Handle<Object> obj, const char * property, rgba_t color);

NAN_GETTER(ValuePropertyGetter);

#define DEFINE_READONLY_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    ValuePropertyGetter, NULL, value, DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));

class Color : public node::ObjectWrap {
  public:
    Color(double colors[], int count);
    ~Color();

    double colors[4];

  private:
    static NAN_GETTER(GetProperty);
    static NAN_SETTER(SetProperty);

    double* GetPropertyPtr(const char* name);
};

#endif
