#ifndef GENOME_H
#define GENOME_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

using namespace v8;
using namespace node;

class Genome : public node::ObjectWrap {
  public:
    static void Export(Handle<v8::Object> exports);

    void CloneGenome(flam3_genome* cp);

  private:
    Genome(Handle<Object> jsObj, flam3_genome* cp);
    ~Genome();

    static NAN_METHOD(Random);
    static NAN_METHOD(Parse);

    static NAN_GETTER(GetName);
    static NAN_SETTER(SetName);

    static NAN_METHOD(New);
    static Genome* NewInstance(flam3_genome* cp);

    static NAN_METHOD(ToXMLString);
    static NAN_METHOD(Render);

    static NAN_METHOD(GetTransform);
    static NAN_GETTER(GetTransformCount);

    static Persistent<Function> constructor;

    flam3_genome genome;
};

#endif
