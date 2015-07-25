#ifndef PALETTE_H
#define PALETTE_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

#include "fields.h"

using namespace v8;
using namespace node;

class Palette : public node::ObjectWrap {
  public:
    static Palette* NewInstance(flam3_palette* palette);

    static void Export(Handle<v8::Object> exports);

    void ClonePalette(flam3_palette* palette);

  private:
    Palette(Handle<Object> jsObj, flam3_palette* palette);
    ~Palette();

    static NAN_INDEX_GETTER(GetIndex);
    static NAN_INDEX_SETTER(SetIndex);
    static NAN_INDEX_QUERY(QueryIndex);
    static NAN_INDEX_DELETER(DeleteIndex);
    static NAN_INDEX_ENUMERATOR(EnumerateIndex);

    static NAN_METHOD(New);

    Color* GetColor(int index);

    static Persistent<Function> constructor;

    Persistent<Object> colors[256];
};

#endif
