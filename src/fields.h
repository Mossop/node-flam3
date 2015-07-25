#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <nan.h>

NAN_GETTER(ValuePropertyGetter);

#define DEFINE_READONLY_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    ValuePropertyGetter, NULL, value, DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));

#endif
