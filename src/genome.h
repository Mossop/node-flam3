#ifndef GENOME_H
#define GENOME_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>
#include "includes/properties.h"

extern "C" {
#include <flam3.h>
}

using namespace v8;
using namespace node;

class Palette;

enum Property_Type {
  INT = 0,
  DOUBLE = 1
};

typedef struct {
  const char* name;
  Property_Type type;
  size_t offset;
} genome_property;

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

    static Persistent<Function> constructor;

    Persistent<Object> paletteObj;
    Persistent<Object> backgroundObj;
    Persistent<Object> centerObj;
    Persistent<Object> rotationalCenterObj;
    flam3_genome genome;
};

#endif
