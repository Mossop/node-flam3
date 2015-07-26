#include "transform.h"
#include "fields.h"
#include "includes/xform_properties.h"

Persistent<Function> Transform::constructor;

property_entry Xform_Properties[] = XFORM_PROPERTIES;

Transform::Transform(Handle<Object> jsObj, flam3_xform* xform) {
  NanScope();

  Wrap(jsObj);

  this->xform = xform;
}

Transform::~Transform() {
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
  exports->Set(NanNew<String>("Transform"), tpl->GetFunction());
}

Transform* Transform::NewInstance(flam3_xform* xform) {
  NanScope();

  Local<Value> jsObj;

  if (xform) {
    Local<Value> argv[] = { NanNew<External>(xform) };
    jsObj = NanNew<Function>(constructor)->NewInstance(1, argv);
  }
  else {
    Local<Value> argv[0];
    jsObj = NanNew<Function>(constructor)->NewInstance(0, argv);
  }

  Transform* transform = ObjectWrap::Unwrap<Transform>(jsObj->ToObject());
  return transform;
}

NAN_METHOD(Transform::New) {
  NanScope();

  flam3_xform* xform = NULL;

  if (args.Length() >= 1) {
    if (!args[0]->IsExternal()) {
      NanThrowTypeError("Argument 0 was of an unexpected type.");
      NanReturnUndefined();
    }

    xform = reinterpret_cast<flam3_xform*>(External::Cast(*args[0])->Value());
  }

  Transform* transform;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Transform(...)`
    transform = new Transform(args.This(), xform);
  }
  else {
    // Invoked as plain function `Transform(...)`, turn into construct call.
    transform = NewInstance(xform);
  }

  NanReturnValue(NanObjectWrapHandle(transform));
}
