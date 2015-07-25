#ifndef PALETTE_H
#define PALETTE_H

#include <node.h>
#include <node_object_wrap.h>
#include <nan.h>

extern "C" {
#include <flam3.h>
}

using namespace v8;
using namespace node;

class Genome;
class Palette;

class PaletteEntry : public node::ObjectWrap {
  public:
    PaletteEntry(Palette* palette, int index);
    ~PaletteEntry();

  private:
    static NAN_GETTER(GetProperty);
    static NAN_SETTER(SetProperty);

    flam3_palette_entry* GetPaletteEntry();
    double* GetPropertyPtr(const char* name);

    Persistent<Object> paletteObj;
    int index;
};

class Palette : public node::ObjectWrap {
  friend class PaletteEntry;

  public:
    Palette(Handle<Object> jsObj, Genome* genome);
    ~Palette();

    static Palette* NewInstance(Genome* genome);

    static void Export(Handle<v8::Object> exports);

    flam3_palette* palette;

  private:
    static NAN_INDEX_GETTER(GetIndex);
    static NAN_INDEX_SETTER(SetIndex);
    static NAN_INDEX_QUERY(QueryIndex);
    static NAN_INDEX_DELETER(DeleteIndex);
    static NAN_INDEX_ENUMERATOR(EnumerateIndex);

    static NAN_METHOD(New);

    PaletteEntry* GetEntry(int index);

    static Persistent<Function> constructor;

    Persistent<Object> genomeObj;

    PaletteEntry* entries[256];
};

#endif
