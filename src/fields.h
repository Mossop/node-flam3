#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

enum Property_Type {
  INT = 0,
  DOUBLE = 1
};

typedef struct {
  const char* name;
  Property_Type type;
  size_t offset;
} property_entry;

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

class Point : public node::ObjectWrap {
  public:
    Point(double coords[2]);
    ~Point();

    double coords[4];

  private:
    static NAN_GETTER(GetProperty);
    static NAN_SETTER(SetProperty);

    double* GetPropertyPtr(const char* name);
};

#endif
