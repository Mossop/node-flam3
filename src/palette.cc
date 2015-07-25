#include "fields.h"
#include "palette.h"
#include "genome.h"

Color::Color(double colors[], int count) {
  NanScope();

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);

  Local<Object> jsObj = tpl->NewInstance();
  Wrap(jsObj);

  memcpy(this->colors, colors, sizeof(double) * count);

  jsObj->SetAccessor(NanNew<String>("red"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("green"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("blue"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);

  if (count >= 4) {
    jsObj->SetAccessor(NanNew<String>("alpha"), GetProperty, SetProperty,
      Local<Value>(), DEFAULT, DontDelete);
  }
  else {
    this->colors[3] = -1;
  }
}

Color::~Color() {
}

double* Color::GetPropertyPtr(const char* name) {
  if (strcmp("red", name) == 0) {
    return &colors[0];
  }
  else if (strcmp("green", name) == 0) {
    return &colors[1];
  }
  else if (strcmp("blue", name) == 0) {
    return &colors[2];
  }
  else if (strcmp("alpha", name) == 0 && colors[3] >= 0) {
    return &colors[3];
  }
  else {
    return NULL;
  }
}

NAN_GETTER(Color::GetProperty) {
  NanScope();

  NanUtf8String name(property);
  Color* entry = ObjectWrap::Unwrap<Color>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    NanReturnValue(NanNew<Number>(*result));
  }

  NanReturnUndefined();
}

NAN_SETTER(Color::SetProperty) {
  NanScope();

  NanUtf8String name(property);
  Color* entry = ObjectWrap::Unwrap<Color>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    *result = value->NumberValue();
    return;
  }
}

Persistent<Function> Palette::constructor;

Palette::Palette(Handle<Object> jsObj, flam3_palette* palette) {
  NanScope();

  Wrap(jsObj);

  DEFINE_READONLY_PROPERTY(length, NanNew<Number>(256))

  for (int i = 0; i < 256; i++) {
    Color* color = new Color((*palette)[i].color, 4);
    NanAssignPersistent(colors[i], NanObjectWrapHandle(color));
  }
}

Palette::~Palette() {
  NanScope();
}

Color* Palette::GetColor(int index) {
  return ObjectWrap::Unwrap<Color>(NanNew<Object>(colors[index]));
}

NAN_INDEX_GETTER(Palette::GetIndex) {
  NanScope();

  Palette* obj = ObjectWrap::Unwrap<Palette>(args.Holder());
  if (index < 256) {
    Color* color = obj->GetColor(index);
    NanReturnValue(NanObjectWrapHandle(color));
  }

  Local<Value> empty;
  NanReturnValue(empty);
}

NAN_INDEX_SETTER(Palette::SetIndex) {
  NanScope();

  Palette* obj = ObjectWrap::Unwrap<Palette>(args.Holder());
  if (index < 256) {
    Color* color = obj->GetColor(index);
    NanReturnValue(NanObjectWrapHandle(color));
  }

  Local<Value> empty;
  NanReturnValue(empty);
}

NAN_INDEX_QUERY(Palette::QueryIndex) {
  NanScope();

  if (index < 256) {
    NanReturnValue(NanNew<Integer>(static_cast<PropertyAttribute>(ReadOnly | DontDelete)));
  }

  Local<Integer> empty;
  NanReturnValue(empty);
}

NAN_INDEX_DELETER(Palette::DeleteIndex) {
  NanScope();

  if (index < 256) {
    NanReturnValue(NanNew<Boolean>(false));
  }

  Local<Boolean> empty;
  NanReturnValue(empty);
}

NAN_INDEX_ENUMERATOR(Palette::EnumerateIndex) {
  NanScope();

  Local<Array> results = NanNew<Array>(GENOME_PROPERTY_COUNT);
  for (int i = 0; i < 256; i++) {
    results->Set(i, NanNew<Integer>(i));
  }

  NanReturnValue(results);
}

void Palette::ClonePalette(flam3_palette* palette) {
  for (int i = 0; i < 256; i++) {
    flam3_palette_entry* entry = &(*palette)[i];
    Color* color = GetColor(i);

    entry->index = i;

    memcpy(&entry->color, color->colors, sizeof(double) * 4);
  }
}

Palette* Palette::NewInstance(flam3_palette* palette) {
  NanScope();

  Local<Value> argv[] = { NanNew<External>(palette) };
  Local<Value> jsObj = NanNew<Function>(constructor)->NewInstance(1, argv);
  return ObjectWrap::Unwrap<Palette>(jsObj->ToObject());
}

NAN_METHOD(Palette::New) {
  NanScope();

  if (args.Length() < 1) {
    NanThrowTypeError("Palette's cannot be constructed from JavaScript");
    NanReturnUndefined();
  }

  if (!args[0]->IsExternal()) {
    NanThrowTypeError("Palette's cannot be constructed from JavaScript");
    NanReturnUndefined();
  }

  flam3_palette* palette = reinterpret_cast<flam3_palette*>(External::Cast(*args[0])->Value());

  Palette* plt;
  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Palette(...)`
    plt = new Palette(args.This(), palette);
  }
  else {
    // Invoked as plain function `Palette(...)`, turn into construct call.
    plt = NewInstance(palette);
  }

  NanReturnValue(NanObjectWrapHandle(plt));
}

void Palette::Export(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Palette"));

  Local<ObjectTemplate> objtpl = tpl->InstanceTemplate();
  objtpl->SetInternalFieldCount(1);
  objtpl->SetIndexedPropertyHandler(GetIndex, SetIndex, QueryIndex, DeleteIndex,
    EnumerateIndex);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<String>("Palette"), tpl->GetFunction());
}
