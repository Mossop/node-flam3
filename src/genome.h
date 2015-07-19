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

  private:
    Genome(Local<Object> jsObj);
    Genome(flam3_genome* g);
    Genome(flam3_genome* g, Local<Object> jsObj);
    ~Genome();

    void Init(flam3_genome* g, Local<Object> jsObj);

    static NAN_METHOD(New);
    static NAN_METHOD(Random);

    static Persistent<v8::Function> constructor;

    flam3_genome* genome;
};
