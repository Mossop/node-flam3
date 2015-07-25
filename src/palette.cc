#include "fields.h"
#include "palette.h"
#include "genome.h"

PaletteEntry::PaletteEntry(Palette* palette, int index) {
  NanScope();

  this->index = index;
  NanAssignPersistent(paletteObj, NanObjectWrapHandle(palette));

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);

  Local<Object> jsObj = tpl->NewInstance();
  Wrap(jsObj);

  jsObj->SetAccessor(NanNew<String>("red"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("green"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("blue"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
  jsObj->SetAccessor(NanNew<String>("alpha"), GetProperty, SetProperty,
    Local<Value>(), DEFAULT, DontDelete);
}

PaletteEntry::~PaletteEntry() {
  NanScope();

  Palette* palette = ObjectWrap::Unwrap<Palette>(NanNew<Object>(paletteObj));
  palette->entries[index] = NULL;
  NanDisposePersistent(paletteObj);
}

flam3_palette_entry* PaletteEntry::GetPaletteEntry() {
  NanScope();

  Local<Object> palette = NanNew<Object>(paletteObj);
  return &(*(ObjectWrap::Unwrap<Palette>(palette)->palette))[index];
}

double* PaletteEntry::GetPropertyPtr(const char* name) {
  flam3_palette_entry* entry = GetPaletteEntry();

  if (strcmp("red", name) == 0) {
    return &entry->color[0];
  }
  else if (strcmp("green", name) == 0) {
    return &entry->color[1];
  }
  else if (strcmp("blue", name) == 0) {
    return &entry->color[2];
  }
  else if (strcmp("alpha", name) == 0) {
    return &entry->color[3];
  }
  else {
    return NULL;
  }
}

NAN_GETTER(PaletteEntry::GetProperty) {
  NanScope();

  NanUtf8String name(property);
  PaletteEntry* entry = ObjectWrap::Unwrap<PaletteEntry>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    NanReturnValue(NanNew<Number>(*result));
  }

  NanReturnUndefined();
}

NAN_SETTER(PaletteEntry::SetProperty) {
  NanScope();

  NanUtf8String name(property);
  PaletteEntry* entry = ObjectWrap::Unwrap<PaletteEntry>(args.Holder());

  double* result = entry->GetPropertyPtr(*name);
  if (result) {
    *result = value->NumberValue();
    return;
  }
}

Persistent<Function> Palette::constructor;

Palette::Palette(Handle<Object> jsObj, Genome* genome) {
  NanScope();

  Wrap(jsObj);

  DEFINE_READONLY_PROPERTY(length, NanNew<Number>(256))

  memset(&entries, 0, sizeof(entries));
  NanAssignPersistent(genomeObj, NanObjectWrapHandle(genome));
  palette = &genome->genome.palette;
}

Palette::~Palette() {
  NanScope();

  Genome* genome = ObjectWrap::Unwrap<Genome>(NanNew<Object>(genomeObj));
  genome->palette = NULL;
  NanDisposePersistent(genomeObj);
}

PaletteEntry* Palette::GetEntry(int index) {
  if (entries[index]) {
    return entries[index];
  }

  return entries[index] = new PaletteEntry(this, index);
}

NAN_INDEX_GETTER(Palette::GetIndex) {
  NanScope();

  Palette* obj = ObjectWrap::Unwrap<Palette>(args.Holder());
  if (index < 256) {
    PaletteEntry* entry = obj->GetEntry(index);
    NanReturnValue(NanObjectWrapHandle(entry));
  }

  Local<Value> empty;
  NanReturnValue(empty);
}

NAN_INDEX_SETTER(Palette::SetIndex) {
  NanScope();

  Palette* obj = ObjectWrap::Unwrap<Palette>(args.Holder());
  if (index < 256) {
    PaletteEntry* entry = obj->GetEntry(index);
    NanReturnValue(NanObjectWrapHandle(entry));
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

Palette* Palette::NewInstance(Genome* genome) {
  NanScope();

  Local<Value> argv[] = { NanNew<External>(genome) };
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

  Genome* genome = reinterpret_cast<Genome*>(External::Cast(*args[0])->Value());

  Palette* palette;
  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Palette(...)`
    palette = new Palette(args.This(), genome);
  }
  else {
    // Invoked as plain function `Palette(...)`, turn into construct call.
    palette = NewInstance(genome);
  }

  NanReturnValue(NanObjectWrapHandle(palette));
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
