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
    Genome(Handle<Object> jsObj);
    Genome(flam3_genome* genome);

    static void Export(Handle<v8::Object> exports);

  private:
    ~Genome();

    void Init(Handle<Object> jsObj);

    static NAN_METHOD(New);
    static NAN_METHOD(Random);
    static NAN_METHOD(Parse);

    static NAN_GETTER(BuildPalette);
    static NAN_METHOD(ToXMLString);

    static Persistent<Function> constructor;

    flam3_genome* genome;
};

class PaletteEntry : public node::ObjectWrap {
  public:
    PaletteEntry(Genome* genome, flam3_palette_entry* entry);

  private:
    ~PaletteEntry();

    Persistent<Object> genomeObj;
    flam3_palette_entry* entry;
};
