#include "transform.h"
#include "fields.h"
#include "includes/xform_properties.h"

Persistent<Function> Transform::constructor;

property_entry Xform_Properties[] = XFORM_PROPERTIES;

Transform::Transform(Handle<Object> jsObj) {
  NanScope();

  Wrap(jsObj);
}

Transform::~Transform() {
}

void Transform::Init(Genome* genome, flam3_xform* xform) {
  NanScope();

  this->xform = xform;
  // Keep the genome alive as long as the transform is alive
  NanObjectWrapHandle(this)->SetHiddenValue(NanNew<String>("flam3::Genome"), NanObjectWrapHandle(genome));

  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    property_entry entry = Xform_Properties[i];
    char* field = reinterpret_cast<char*>(xform) + entry.offset;

    if (entry.type == DOUBLE) {
      DEFINE_DOUBLE_PROPERTY(entry.name, field);
    }
    else {
      DEFINE_INT_PROPERTY(entry.name, field);
    }
  }

  for (int i = 0; i < flam3_nvariations; i++) {
    Local<Object> variation = NanNew<Object>();
    SetDoubleField(variation, "strength", xform->var[i]);
    NanObjectWrapHandle(this)->Set(NanNew<String>(flam3_variation_names[i]), variation);
  }
}

void Transform::Export(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Transform"));

  Local<ObjectTemplate> objtpl = tpl->InstanceTemplate();
  objtpl->SetInternalFieldCount(1);

  NanAssignPersistent(constructor, tpl->GetFunction());
}

void Transform::CloneTransform(flam3_xform* xform) {
  for (int i = 0; i < flam3_nvariations; i++) {
    xform->var[i] = 0;

    Local<Value> varn = NanObjectWrapHandle(this)->Get(NanNew<String>(flam3_variation_names[i]));
    if (varn.IsEmpty()) {
      continue;
    }

    Local<Object> variation = varn->ToObject();
    GetDoubleField(variation, "strength", xform->var[i]);
  }
}

Transform* Transform::NewInstance(Genome* genome, flam3_xform* xform) {
  assert(xform);
  NanScope();

  Local<Value> argv[] = { NanNew<External>(genome), NanNew<External>(xform) };
  Local<Value> jsObj = NanNew<Function>(constructor)->NewInstance(2, argv);
  Transform* transform = ObjectWrap::Unwrap<Transform>(jsObj->ToObject());
  transform->Init(genome, xform);
  return transform;
}

NAN_METHOD(Transform::New) {
  NanScope();

  assert(args.IsConstructCall());
  Transform* transform = new Transform(args.This());
  NanReturnValue(NanObjectWrapHandle(transform));
}
