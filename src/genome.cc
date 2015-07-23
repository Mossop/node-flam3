#include "nativefields.h"
#include "genome.h"

int32_t sGenomeCount = 0;

PaletteEntry::PaletteEntry(Genome* genome, flam3_palette_entry* entry) {
  this->entry = entry;
  NanAssignPersistent(genomeObj, NanObjectWrapHandle(genome));

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);
  Wrap(tpl->NewInstance());

  DEFINE_DBL_PROPERTY(red, entry->color[0])
  DEFINE_DBL_PROPERTY(green, entry->color[1])
  DEFINE_DBL_PROPERTY(blue, entry->color[2])
  DEFINE_DBL_PROPERTY(alpha, entry->color[3])
}

PaletteEntry::~PaletteEntry() {
  NanDisposePersistent(genomeObj);
}

Persistent<Function> Genome::constructor;

Genome::Genome(Handle<Object> jsObj) {
  genome = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
  memset(genome, 0, sizeof(flam3_genome));
  clear_cp(genome, flam3_defaults_on);

  Init(jsObj);
}

Genome::Genome(flam3_genome* genome) {
  this->genome = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
  memset(this->genome, 0, sizeof(flam3_genome));
  clear_cp(this->genome, flam3_defaults_on);
  flam3_copy(this->genome, genome);

  Local<ObjectTemplate> tpl = NanNew<ObjectTemplate>();
  tpl->SetInternalFieldCount(1);

  Init(tpl->NewInstance());
}

Genome::~Genome() {
  clear_cp(genome, flam3_defaults_on);
  flam3_free(genome);
  sGenomeCount--;
}

void Genome::Init(Handle<Object> jsObj) {
  sGenomeCount++;
  Wrap(jsObj);

  // char flame_name[flam3_name_len+1]; /* 64 chars plus a null */
  DEFINE_STRUCT_DBL_PROPERTY(genome, time)
  DEFINE_STRUCT_INT_PROPERTY(genome, interpolation)
  DEFINE_STRUCT_INT_PROPERTY(genome, interpolation_type)
  DEFINE_STRUCT_INT_PROPERTY(genome, palette_interpolation)
  DEFINE_STRUCT_DBL_PROPERTY(genome, hsv_rgb_palette_blend)
   
  DEFINE_STRUCT_INT_PROPERTY(genome, genome_index)
  // char parent_fname[flam3_parent_fn_len];   /* base filename where parent was located */
  DEFINE_STRUCT_INT_PROPERTY(genome, symmetry)
  DEFINE_STRUCT_INT_PROPERTY(genome, palette_index)
  DEFINE_STRUCT_DBL_PROPERTY(genome, brightness)
  DEFINE_STRUCT_DBL_PROPERTY(genome, contrast)
  DEFINE_STRUCT_DBL_PROPERTY(genome, gamma)
  DEFINE_STRUCT_DBL_PROPERTY(genome, highlight_power)
  DEFINE_STRUCT_INT_PROPERTY(genome, width)
  DEFINE_STRUCT_INT_PROPERTY(genome, height)
  DEFINE_STRUCT_INT_PROPERTY(genome, spatial_oversample)
  // double center[2];             /* of camera */
  // double rot_center[2];         /* really the center */
  DEFINE_STRUCT_DBL_PROPERTY(genome, rotate)
  DEFINE_STRUCT_DBL_PROPERTY(genome, vibrancy)
  DEFINE_STRUCT_DBL_PROPERTY(genome, hue_rotation)
  // double background[3];
  DEFINE_STRUCT_DBL_PROPERTY(genome, zoom)
  DEFINE_STRUCT_DBL_PROPERTY(genome, pixels_per_unit)
  DEFINE_STRUCT_DBL_PROPERTY(genome, spatial_filter_radius)
  DEFINE_STRUCT_INT_PROPERTY(genome, spatial_filter_select)
  DEFINE_STRUCT_DBL_PROPERTY(genome, sample_density)
  DEFINE_STRUCT_INT_PROPERTY(genome, nbatches)
  DEFINE_STRUCT_INT_PROPERTY(genome, ntemporal_samples)
  DEFINE_STRUCT_DBL_PROPERTY(genome, estimator)
  DEFINE_STRUCT_DBL_PROPERTY(genome, estimator_curve)
  DEFINE_STRUCT_DBL_PROPERTY(genome, estimator_minimum)
  DEFINE_STRUCT_DBL_PROPERTY(genome, gam_lin_thresh)
  DEFINE_STRUCT_INT_PROPERTY(genome, palette_index0)
  DEFINE_STRUCT_DBL_PROPERTY(genome, hue_rotation0)
  DEFINE_STRUCT_INT_PROPERTY(genome, palette_index1)
  DEFINE_STRUCT_DBL_PROPERTY(genome, hue_rotation1)
  DEFINE_STRUCT_DBL_PROPERTY(genome, palette_blend)
  DEFINE_STRUCT_INT_PROPERTY(genome, temporal_filter_type)
  DEFINE_STRUCT_DBL_PROPERTY(genome, temporal_filter_width)
  DEFINE_STRUCT_DBL_PROPERTY(genome, temporal_filter_exp)
  DEFINE_STRUCT_INT_PROPERTY(genome, palette_mode)

  jsObj->Set(NanNew<String>("toXMLString"),
    NanNew<FunctionTemplate>(ToXMLString)->GetFunction());
  jsObj->SetAccessor(NanNew<String>("palette"), BuildPalette);
}

void Genome::Export(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Genome"));
  tpl->Set(NanNew<String>("createRandom"),
    NanNew<FunctionTemplate>(Genome::Random)->GetFunction());
  tpl->Set(NanNew<String>("fromXMLString"),
    NanNew<FunctionTemplate>(Genome::Parse)->GetFunction());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<String>("Genome"), tpl->GetFunction());
}

NAN_METHOD(Genome::New) {
  NanScope();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Genome(...)`
    Genome* obj = new Genome(args.This());
    NanReturnValue(NanObjectWrapHandle(obj));
  } else {
    // Invoked as plain function `Genome(...)`, turn into construct call.
    Local<Value> argv[0];
    NanReturnValue(NanNew<Function>(constructor)->NewInstance(0, argv));
  }
}

NAN_METHOD(Genome::Random) {
  NanScope();

  flam3_genome* genome = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
  memset(genome, 0, sizeof(flam3_genome));

  int variations[] = { flam3_variation_random };
  flam3_random(genome, variations, 1, 0, 0);

  Genome* obj = new Genome(genome);
  NanReturnValue(NanObjectWrapHandle(obj));
}

NAN_METHOD(Genome::Parse) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowTypeError("Wrong number of arguments");
    NanReturnUndefined();
  }

  NanUtf8String xmlstr(args[0]);
  NanUtf8String xmlfilename(args[1]);
  int count;
  flam3_genome* genomes = flam3_parse_xml2(*xmlstr, *xmlfilename, flam3_defaults_on, &count);

  if (!genomes) {
    NanReturnNull();
  }

  Local<Array> results = NanNew<Array>(count);
  for (int i = 0; i < count; i++) {
    flam3_genome* genome = genomes + i;
    results->Set(i, NanObjectWrapHandle(new Genome(genome)));
    xmlFreeDoc(genome->edits);
    clear_cp(genome, 0);
  }

  flam3_free(genomes);
  NanReturnValue(results);
}

NAN_GETTER(Genome::BuildPalette) {
  NanScope();

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());

  Local<Array> palette = NanNew<Array>(0);
  for (int i = 0; i < 256; i++) {
    flam3_palette_entry *entry = &(obj->genome->palette[i]);
    palette->Set(i, NanObjectWrapHandle(new PaletteEntry(obj, entry)));
  }

  NanReturnValue(palette);
}

NAN_METHOD(Genome::ToXMLString) {
  NanScope();

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());
  char* str = flam3_print_to_string(obj->genome);
  Local<String> xml = NanNew<String>(str);
  flam3_free(str);

  NanReturnValue(xml);
}
