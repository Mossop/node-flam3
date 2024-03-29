#include "fields.h"
#include "genome.h"
#include "transform.h"
#include <isaac.h>
#include "includes/genome_properties.h"

#ifdef _WIN32
#define random()  (rand() ^ (rand()<<15))
#endif

int32_t sGenomeCount = 0;

rgb_t BLACK = { 0, 0 };
point_t ORIGIN = { 0, 0 };

void free_genome(flam3_genome & genome) {
  xmlFreeDoc(genome.edits);
  clear_cp(&genome, flam3_defaults_on);
}

void free_genome(flam3_genome* genome) {
  free_genome(*genome);
  flam3_free(genome);
}

Persistent<Function> Genome::constructor;

property_entry Genome_Properties[] = GENOME_PROPERTIES;

Genome::Genome(Handle<Object> jsObj, flam3_genome* cp) {
  NanScope();

  sGenomeCount++;
  Wrap(jsObj);

  memset(&genome, 0, sizeof(flam3_genome));
  clear_cp(&genome, flam3_defaults_on);

  if (cp) {
    flam3_copy(&genome, cp);
  }

  for (int i = 0; i < GENOME_PROPERTY_COUNT; i++) {
    property_entry entry = Genome_Properties[i];
    char* field = reinterpret_cast<char*>(&genome) + entry.offset;

    if (entry.type == DOUBLE_FIELD) {
      DEFINE_DOUBLE_PROPERTY(entry.name, field);
    }
    else {
      DEFINE_INT_PROPERTY(entry.name, field);
    }
  }

  // Define child objects
  SetPaletteField(jsObj, "palette", genome.palette);
  SetColorField(jsObj, "background", genome.background);
  SetPointField(jsObj, "center", genome.center);

  Local<Array> transforms = NanNew<Array>(genome.num_xforms);
  for (int i = 0; i < genome.num_xforms; i++) {
    transforms->Set(i, NanObjectWrapHandle(Transform::NewInstance(this, &genome.xform[i])));
  }
  jsObj->SetHiddenValue(NanNew<String>("flam3::transforms"), transforms);

  jsObj->SetAccessor(NanNew<String>("name"), GetName, SetName,
    Handle<Value>(), DEFAULT, static_cast<PropertyAttribute>(DontDelete));
}

Genome::~Genome() {
  free_genome(genome);

  sGenomeCount--;
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

void Genome::CloneGenome(flam3_genome* cp) {
  memset(cp, 0, sizeof(flam3_genome));
  clear_cp(cp, flam3_defaults_on);
  flam3_copy(cp, &genome);

  Local<Object> jsObj = NanObjectWrapHandle(this);

  GetPaletteField(jsObj, "palette", cp->palette);
  GetColorField(jsObj, "background", cp->background, BLACK);
  GetPointField(jsObj, "center", cp->center, ORIGIN);

  Local<Array> transforms = Local<Array>::Cast(jsObj->GetHiddenValue(NanNew<String>("flam3::transforms")));
  assert(transforms->Length() == (uint32_t)cp->num_xforms);
  for (int i = 0; i < cp->num_xforms; i++) {
    Transform* transform = ObjectWrap::Unwrap<Transform>(transforms->Get(i)->ToObject());
    transform->CloneTransform(&cp->xform[i]);
  }
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
  NODE_SET_PROTOTYPE_METHOD(tpl, "getTransform", GetTransform);

  Local<ObjectTemplate> objtpl = tpl->InstanceTemplate();
  objtpl->SetInternalFieldCount(1);
  objtpl->SetAccessor(NanNew<String>("transformCount"), GetTransformCount, NULL,
    Local<Value>(), DEFAULT, static_cast<PropertyAttribute>(ReadOnly | DontDelete));

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
    jsObj = NanNew<Function>(constructor)->NewInstance(0, NULL);
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
  int* variations = reinterpret_cast<int*>(malloc(sizeof(int) * (flam3_nvariations - num_novars)));

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
  free(variations);

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

NAN_GETTER(Genome::GetTransformCount) {
  NanScope();

  Local<Array> transforms = Local<Array>::Cast(args.Holder()->GetHiddenValue(NanNew<String>("flam3::transforms")));
  NanReturnValue(NanNew<Number>(transforms->Length()));
}

NAN_METHOD(Genome::GetTransform) {
  NanScope();

  if (args.Length() < 1 || !args[0]->IsUint32()) {
    NanThrowTypeError("Must pass an index");
    NanReturnUndefined();
  }

  uint32_t index = args[0]->Uint32Value();
  Local<Array> transforms = Local<Array>::Cast(args.Holder()->GetHiddenValue(NanNew<String>("flam3::transforms")));

  if (index >= transforms->Length()) {
    NanThrowTypeError("Must pass an index between 0 and transformCount");
    NanReturnUndefined();
  }

  NanReturnValue(transforms->Get(index));
}
