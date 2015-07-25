#ifndef FIELDS_H
#define FIELDS_H

#include <node.h>
#include <nan.h>

NAN_GETTER(DoublePropertyGetter);
NAN_SETTER(DoublePropertySetter);
NAN_GETTER(IntPropertyGetter);
NAN_SETTER(IntPropertySetter);
NAN_GETTER(ValuePropertyGetter);

#define DEFINE_DBL_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    DoublePropertyGetter, DoublePropertySetter, NanNew<External>(&(field)), DEFAULT, DontDelete);

#define DEFINE_STRUCT_DBL_PROPERTY(strct, field) DEFINE_DBL_PROPERTY(field, strct->field)

#define DEFINE_INT_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    IntPropertyGetter, IntPropertySetter, NanNew<External>(&(field)), DEFAULT, DontDelete);

#define DEFINE_STRUCT_INT_PROPERTY(strct, field) DEFINE_INT_PROPERTY(field, strct->field)

#define DEFINE_READONLY_PROPERTY(name, value) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    ValuePropertyGetter, NULL, value, DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));

#endif
