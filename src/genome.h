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
    static void Init(Handle<v8::Object> exports);
    flam3_genome genome;

  private:
    explicit Genome();
    ~Genome();

    static NAN_METHOD(New);
    static Persistent<v8::Function> constructor;
};
