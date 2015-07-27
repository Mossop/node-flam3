#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

#include "genome.h"

using namespace v8;
using namespace node;

class Transform : public node::ObjectWrap {
  public:
    Transform(Handle<Object> jsObj, Genome* genome, flam3_xform* xform);

    static Transform* NewInstance(Genome* genome, flam3_xform* cp);
    static void Export(Handle<v8::Object> exports);

  private:
    ~Transform();

    static NAN_PROPERTY_GETTER(GetProperty);
    static NAN_PROPERTY_SETTER(SetProperty);
    static NAN_PROPERTY_QUERY(QueryProperty);
    static NAN_PROPERTY_DELETER(DeleteProperty);
    static NAN_PROPERTY_ENUMERATOR(EnumerateProperties);

    static NAN_METHOD(New);

    static Persistent<Function> constructor;

    flam3_xform* xform;
};

#endif
