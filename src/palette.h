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

class Color : public node::ObjectWrap {
  public:
    Color(double colors[], int count);
    ~Color();

    double red;
    double green;
    double blue;
    double alpha;

  private:
    static NAN_GETTER(GetProperty);
    static NAN_SETTER(SetProperty);

    double* GetPropertyPtr(const char* name);
};

class Palette : public node::ObjectWrap {
  friend class PaletteEntry;

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
