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
}

NAN_PROPERTY_GETTER(Transform::GetProperty) {
  NanScope();

  Transform* obj = ObjectWrap::Unwrap<Transform>(args.Holder());

  NanUtf8String name(property);
  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Xform_Properties[i].name) == 0) {
      char* field = reinterpret_cast<char*>(obj->xform) + Xform_Properties[i].offset;

      if (Xform_Properties[i].type == DOUBLE) {
        NanReturnValue(NanNew<Number>(*(reinterpret_cast<double*>(field))));
      }
      else {
        NanReturnValue(NanNew<Number>(*(reinterpret_cast<int*>(field))));
      }
    }
  }

  Local<Value> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_SETTER(Transform::SetProperty) {
  NanScope();

  Transform* obj = ObjectWrap::Unwrap<Transform>(args.Holder());

  NanUtf8String name(property);
  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Xform_Properties[i].name) == 0) {
      char* field = reinterpret_cast<char*>(obj->xform) + Xform_Properties[i].offset;

      if (Xform_Properties[i].type == DOUBLE) {
        *(reinterpret_cast<double*>(field)) = value->NumberValue();
      }
      else {
        *(reinterpret_cast<int*>(field)) = value->NumberValue();
      }
      NanReturnValue(value);
    }
  }

  Local<Value> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_QUERY(Transform::QueryProperty) {
  NanScope();

  NanUtf8String name(property);
  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Xform_Properties[i].name) == 0) {
      PropertyAttribute attr = DontDelete;
      NanReturnValue(NanNew<Integer>(attr));
    }
  }

  Local<Integer> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_DELETER(Transform::DeleteProperty) {
  NanScope();

  NanUtf8String name(property);
  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Xform_Properties[i].name) == 0) {
      NanReturnValue(NanNew<Boolean>(false));
    }
  }

  Local<Boolean> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_ENUMERATOR(Transform::EnumerateProperties) {
  NanScope();

  Local<Array> results = NanNew<Array>(XFORM_PROPERTY_COUNT);
  for (int i = 0; i < XFORM_PROPERTY_COUNT; i++) {
    results->Set(i, NanNew<String>(Xform_Properties[i].name));
  }

  NanReturnValue(results);
}

void Transform::Export(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Transform"));

  Local<ObjectTemplate> objtpl = tpl->InstanceTemplate();
  objtpl->SetInternalFieldCount(1);
  objtpl->SetNamedPropertyHandler(GetProperty, SetProperty, QueryProperty,
    DeleteProperty, EnumerateProperties);

  NanAssignPersistent(constructor, tpl->GetFunction());
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
