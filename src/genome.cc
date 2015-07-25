#include "fields.h"
#include "genome.h"
#include "palette.h"
#include <isaac.h>

int32_t sGenomeCount = 0;

void free_genome(flam3_genome & genome) {
  xmlFreeDoc(genome.edits);
  clear_cp(&genome, flam3_defaults_on);
}

void free_genome(flam3_genome* genome) {
  free_genome(*genome);
  flam3_free(genome);
}

Persistent<Function> Genome::constructor;

genome_property Genome_Properties[] = GENOME_PROPERTIES;

Genome::Genome(Handle<Object> jsObj, flam3_genome* cp) {
  NanScope();

  sGenomeCount++;
  Wrap(jsObj);

  memset(&genome, 0, sizeof(flam3_genome));
  clear_cp(&genome, flam3_defaults_on);

  if (cp) {
    flam3_copy(&genome, cp);
  }

  Palette* plt = Palette::NewInstance(&genome.palette);
  NanAssignPersistent(paletteObj, NanObjectWrapHandle(plt));

  Color* color = new Color(genome.background, 3);
  NanAssignPersistent(backgroundObj, NanObjectWrapHandle(color));

  jsObj->SetAccessor(NanNew<String>("palette"), GetPalette, NULL,
    Handle<Value>(), DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));
  jsObj->SetAccessor(NanNew<String>("background"), GetBackground, NULL,
    Handle<Value>(), DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));
  jsObj->SetAccessor(NanNew<String>("name"), GetName, SetName,
    Handle<Value>(), DEFAULT, static_cast<PropertyAttribute>(DontDelete));
}

Genome::~Genome() {
  NanDisposePersistent(paletteObj);
  free_genome(genome);

  sGenomeCount--;
}

/*void Genome::Init(Handle<Object> jsObj) {
  // double center[2];
  // double rot_center[2];
}*/

NAN_GETTER(Genome::GetPalette) {
  NanScope();

  Genome* genome = ObjectWrap::Unwrap<Genome>(args.Holder());
  NanReturnValue(NanNew<Object>(genome->paletteObj));
}

NAN_GETTER(Genome::GetBackground) {
  NanScope();

  Genome* genome = ObjectWrap::Unwrap<Genome>(args.Holder());
  NanReturnValue(NanNew<Object>(genome->backgroundObj));
}

NAN_GETTER(Genome::GetName) {
  NanScope();

  Genome* genome = ObjectWrap::Unwrap<Genome>(args.Holder());
  NanReturnValue(NanNew<String>(genome->genome.flame_name));
}

NAN_SETTER(Genome::SetName) {
  NanScope();

  NanUtf8String str(value);
  if (strlen(*str) > flam3_name_len) {
    NanThrowTypeError("Name was too long");
    return;
  }

  Genome* genome = ObjectWrap::Unwrap<Genome>(args.Holder());
  strncpy(genome->genome.flame_name, *str, flam3_name_len);
}

NAN_PROPERTY_GETTER(Genome::GetProperty) {
  NanScope();

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());

  NanUtf8String name(property);
  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Genome_Properties[i].name) == 0) {
      char* field = reinterpret_cast<char*>(&obj->genome) + Genome_Properties[i].offset;

      if (Genome_Properties[i].type == DOUBLE) {
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

NAN_PROPERTY_SETTER(Genome::SetProperty) {
  NanScope();

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());

  NanUtf8String name(property);
  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Genome_Properties[i].name) == 0) {
      char* field = reinterpret_cast<char*>(&obj->genome) + Genome_Properties[i].offset;

      if (Genome_Properties[i].type == DOUBLE) {
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

NAN_PROPERTY_QUERY(Genome::QueryProperty) {
  NanScope();

  NanUtf8String name(property);
  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Genome_Properties[i].name) == 0) {
      PropertyAttribute attr = DontDelete;
      NanReturnValue(NanNew<Integer>(attr));
    }
  }

  Local<Integer> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_DELETER(Genome::DeleteProperty) {
  NanScope();

  NanUtf8String name(property);
  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    if (strcmp(*name, Genome_Properties[i].name) == 0) {
      NanReturnValue(NanNew<Boolean>(false));
    }
  }

  Local<Boolean> empty;
  NanReturnValue(empty);
}

NAN_PROPERTY_ENUMERATOR(Genome::EnumerateProperties) {
  NanScope();

  Local<Array> results = NanNew<Array>(GENOME_PROPERTY_COUNT);
  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    results->Set(i, NanNew<String>(Genome_Properties[i].name));
  }

  NanReturnValue(results);
}

void Genome::CloneGenome(flam3_genome* cp) {
  memset(cp, 0, sizeof(flam3_genome));
  clear_cp(cp, flam3_defaults_on);
  flam3_copy(cp, &genome);

  Color* color = ObjectWrap::Unwrap<Color>(NanNew<Object>(backgroundObj));
  memcpy(cp->background, color->colors, sizeof(double) * 3);

  Palette* palette = ObjectWrap::Unwrap<Palette>(NanNew<Object>(paletteObj));
  palette->ClonePalette(&genome.palette);
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

  NODE_SET_PROTOTYPE_METHOD(tpl, "toXMLString", ToXMLString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "render", Render);

  Local<ObjectTemplate> objtpl = tpl->InstanceTemplate();
  objtpl->SetInternalFieldCount(1);
  objtpl->SetNamedPropertyHandler(GetProperty, SetProperty, QueryProperty,
    DeleteProperty, EnumerateProperties);

  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew<String>("Genome"), tpl->GetFunction());
}

Genome* Genome::NewInstance(flam3_genome* cp) {
  NanScope();

  Local<Value> jsObj;

  if (cp) {
    Local<Value> argv[] = { NanNew<External>(cp) };
    jsObj = NanNew<Function>(constructor)->NewInstance(1, argv);
  }
  else {
    Local<Value> argv[0];
    jsObj = NanNew<Function>(constructor)->NewInstance(0, argv);
  }

  Genome* genome = ObjectWrap::Unwrap<Genome>(jsObj->ToObject());
  return genome;
}

NAN_METHOD(Genome::New) {
  NanScope();

  flam3_genome* cp = NULL;

  if (args.Length() >= 1) {
    if (!args[0]->IsExternal()) {
      NanThrowTypeError("Argument 0 was of an unexpected type.");
      NanReturnUndefined();
    }

    cp = reinterpret_cast<flam3_genome*>(External::Cast(*args[0])->Value());
  }

  Genome* genome;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Genome(...)`
    genome = new Genome(args.This(), cp);
  }
  else {
    // Invoked as plain function `Genome(...)`, turn into construct call.
    genome = NewInstance(cp);
  }

  NanReturnValue(NanObjectWrapHandle(genome));
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

  Genome* obj = Genome::NewInstance(&genome);
  free_genome(genome);

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
    results->Set(i, NanObjectWrapHandle(Genome::NewInstance(genome)));
    // We want to clear the contents of the genome freeing the array below will
    // clear the genome structure
    free_genome(*genome);
  }

  flam3_free(genomes);
  NanReturnValue(results);
}

NAN_METHOD(Genome::ToXMLString) {
  NanScope();

  Genome* obj = ObjectWrap::Unwrap<Genome>(args.Holder());
  flam3_genome genome;
  obj->CloneGenome(&genome);
  char* str = flam3_print_to_string(&genome);
  free_genome(genome);
  Local<String> xml = NanNew<String>(str);
  flam3_free(str);

  NanReturnValue(xml);
}

class Renderer : public NanAsyncWorker {
  public:
    Renderer(NanCallback *callback, Genome* gnm, Handle<Object> options) : NanAsyncWorker(callback) {
      genome = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
      gnm->CloneGenome(genome);

      /* Force ntemporal_samples to 1 for -render */
      genome->ntemporal_samples = 1;

      flam3_init_frame(&frame);

      Local<Value> seedVal = options->Get(NanNew<String>("seed"));
      if (!seedVal->IsUndefined()) {
        NanUtf8String seedStr(seedVal);
        strncpy((char *)frame.rc.randrsl, *seedStr, RANDSIZ * sizeof(ub4));
        irandinit(&frame.rc, 1);
        srand(atoi(*seedStr));
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
      free_genome(genome);
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

  NanAsyncQueueWorker(new Renderer(callback, obj, options));
  NanReturnUndefined();
}
