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
    static Transform* NewInstance(Genome* genome, flam3_xform* cp);
    static void Export(Handle<v8::Object> exports);

  private:
    Transform(Handle<Object> jsObj);
    ~Transform();
    void Init(Genome* genome, flam3_xform* xform);

    static NAN_METHOD(New);

    static Persistent<Function> constructor;

    flam3_xform* xform;
};

#endif
