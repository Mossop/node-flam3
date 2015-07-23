#include "nativefields.h"
#include "genome.h"
#include <isaac.h>

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
  jsObj->Set(NanNew<String>("render"),
    NanNew<FunctionTemplate>(Render)->GetFunction());
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

void init_genome(flam3_genome* genome) {
  genome->time = 0.0;
  genome->interpolation = flam3_interpolation_linear;
  genome->palette_interpolation = flam3_palette_interpolation_hsv;
  genome->hsv_rgb_palette_blend = 0.0;
  genome->background[0] = 0.0;
  genome->background[1] = 0.0;
  genome->background[2] = 0.0;
  genome->center[0] = 0.0;
  genome->center[1] = 0.0;
  genome->rotate = 0.0;
  genome->pixels_per_unit = 64;
  genome->width = 128;
  genome->height = 128;
  genome->spatial_oversample = 1;
  genome->spatial_filter_radius = 0.5;
  genome->spatial_filter_select = 0;
  genome->highlight_power= 1.0;
  genome->zoom = 0.0;
  genome->sample_density = 1;
  genome->nbatches = 1;
  genome->ntemporal_samples = 1;
  genome->estimator = 0.0;
  genome->estimator_minimum = 0.0;
  genome->estimator_curve = 0.6;
}

NAN_METHOD(Genome::Random) {
  NanScope();

  Local<Object> options = NanNew<Object>();
  if (args.Length() >= 1) {
    if (args[0]->IsObject()) {
      options = args[0]->ToObject();
    }
    else if (!args[0]->IsNull()) {
      NanThrowTypeError("Argument 0 must be an object or null");
      NanReturnUndefined();
    }
  }

  flam3_genome genome;
  memset(&genome, 0, sizeof(flam3_genome));
  init_genome(&genome);

  flam3_frame frame;
  flam3_init_frame(&frame);

  int novars[] = {
    VAR_NOISE,
    VAR_BLUR,
    VAR_GAUSSIAN_BLUR,
    VAR_RADIAL_BLUR,
    VAR_NGON,
    VAR_SQUARE,
    VAR_RAYS,
    VAR_CROSS,
    VAR_PRE_BLUR,
    VAR_SEPARATION,
    VAR_SPLIT,
    VAR_SPLITS
  };

  int num_vars = 0;
  int num_novars = sizeof(novars) / sizeof(int);
  int variations[flam3_nvariations - num_novars];

  for (int i = 0; i < flam3_nvariations; i++) {
    variations[num_vars++] = i;

    for (int j = 0; j < num_novars; j++) {
      if (novars[j] == i) {
        num_vars--;
        break;
      }
    }

    if (num_vars == (flam3_nvariations - num_novars))
      break;
  }

  int syms[] = {
    -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8
  };
  int symmetry = syms[random() % 16];
  int spec_xforms = 0;
  flam3_random(&genome, variations, num_vars, symmetry, spec_xforms);

  double bmin[2], bmax[2];
  flam3_estimate_bounding_box(&genome, 0.01, 100000, bmin, bmax, &frame.rc);
  genome.center[0] = (bmin[0] + bmax[0]) / 2.0;
  genome.center[1] = (bmin[1] + bmax[1]) / 2.0;
  genome.rot_center[0] = genome.center[0];
  genome.rot_center[1] = genome.center[1];
  genome.pixels_per_unit = genome.width / (bmax[0] - bmin[0]);

  Genome* obj = new Genome(&genome);
  clear_cp(&genome, flam3_defaults_on);

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

class Renderer : public NanAsyncWorker {
  public:
    Renderer(NanCallback *callback, flam3_genome* genome, Handle<Object> options) : NanAsyncWorker(callback) {
      this->genome = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
      memset(this->genome, 0, sizeof(flam3_genome));
      clear_cp(this->genome, flam3_defaults_on);
      flam3_copy(this->genome, genome);

      /* Force ntemporal_samples to 1 for -render */
      genome->ntemporal_samples = 1;

      flam3_init_frame(&frame);

      Local<Value> seedVal = options->Get(NanNew<String>("seed"));
      if (!seedVal->IsUndefined()) {
        NanUtf8String seedStr(seedVal);
        strncpy((char *)frame.rc.randrsl, *seedStr, RANDSIZ * sizeof(ub4));
        irandinit(&frame.rc, 1);
      }

      frame.genomes = genome;
      frame.ngenomes = 1;
      frame.verbose = 0;
      frame.bits = 33;
      frame.time = 0.0;
      frame.pixel_aspect_ratio = 1.0;
      frame.progress = 0;
      frame.nthreads = flam3_count_nthreads();
      frame.earlyclip = 0;
      frame.sub_batch_size = 10000;
      frame.bytes_per_channel = 1;
    }

    ~Renderer() {
      clear_cp(genome, flam3_defaults_on);
      flam3_free(genome);
    }

    void Execute () {
      int channels = 4;

      image_size = channels * genome->width *
                   genome->height * frame.bytes_per_channel;
      image_data = malloc(image_size);

      stat_struct stats;

      if (flam3_render(&frame, image_data, flam3_field_both, channels, 0, &stats)) {
        free(image_data);

        SetErrorMessage("Error rendering image");
      }
    }

    void HandleOKCallback () {
      NanScope();

      Local<Value> argv[] = {
        NanNull(),
        NanBufferUse(reinterpret_cast<char*>(image_data), image_size)
      };
      callback->Call(2, argv);
    }

  private:
    flam3_genome* genome;
    flam3_frame frame;
    size_t image_size;
    void* image_data;
};

NAN_METHOD(Genome::Render) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowTypeError("Wrong number of arguments");
    NanReturnUndefined();
  }

  Local<Object> options = NanNew<Object>();
  if (args[0]->IsObject()) {
    options = args[0]->ToObject();
  }
  else if (!args[0]->IsNull()) {
    NanThrowTypeError("Argument 0 must be an object or null");
    NanReturnUndefined();
  }

  if (!args[1]->IsFunction()) {
    NanThrowTypeError("Argument 1 be a function");
    NanReturnUndefined();
  }

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());

  NanCallback *callback = new NanCallback(args[1].As<Function>());

  NanAsyncQueueWorker(new Renderer(callback, obj->genome, options));
  NanReturnUndefined();
}
