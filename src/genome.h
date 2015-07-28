#ifndef GENOME_H
#define GENOME_H

#include <v8-util.h>
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
    Genome(Handle<Object> jsObj, flam3_genome* cp);

    static void Export(Handle<v8::Object> exports);

    void CloneGenome(flam3_genome* cp);

  private:
    ~Genome();

    static NAN_METHOD(Random);
    static NAN_METHOD(Parse);

    static NAN_GETTER(GetName);
    static NAN_SETTER(SetName);

    static NAN_PROPERTY_GETTER(GetProperty);
    static NAN_PROPERTY_SETTER(SetProperty);
    static NAN_PROPERTY_QUERY(QueryProperty);
    static NAN_PROPERTY_DELETER(DeleteProperty);
    static NAN_PROPERTY_ENUMERATOR(EnumerateProperties);

    static NAN_METHOD(New);
    static Genome* NewInstance(flam3_genome* cp);

    static NAN_METHOD(ToXMLString);
    static NAN_METHOD(Render);

    static NAN_METHOD(GetTransform);
    static NAN_GETTER(GetTransformCount);

    static Persistent<Function> constructor;

    Persistent<Object> paletteObj;
    flam3_genome genome;
};

#endif
