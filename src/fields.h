#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

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
