#include "genome.h"

Persistent<Function> Genome::constructor;

void DoublePropertyGetter(Local<String> property, const PropertyCallbackInfo<Value>& args) {
  Local<External> ptr = Local<External>::Cast(args.Data());
  double* field = reinterpret_cast<double*>(ptr->Value());

  NanReturnValue(NanNew<Number>(*field));
}

void DoublePropertySetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& args) {
  Local<External> ptr = Local<External>::Cast(args.Data());
  double* field = reinterpret_cast<double*>(ptr->Value());

  *field = value->NumberValue();
}

void IntPropertyGetter(Local<String> property, const PropertyCallbackInfo<Value>& args) {
  Local<External> ptr = Local<External>::Cast(args.Data());
  int* field = reinterpret_cast<int*>(ptr->Value());

  NanReturnValue(NanNew<Number>(*field));
}

void IntPropertySetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& args) {
  Local<External> ptr = Local<External>::Cast(args.Data());
  int* field = reinterpret_cast<int*>(ptr->Value());

  *field = value->IntegerValue();
}

#define DEFINE_DBL_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    DoublePropertyGetter, DoublePropertySetter, NanNew<External>(&(field)));

#define DEFINE_STRUCT_DBL_PROPERTY(strct, field) DEFINE_DBL_PROPERTY(field, strct->field)

#define DEFINE_INT_PROPERTY(name, field) \
  NanObjectWrapHandle(this)->SetAccessor(NanNew<String>(#name), \
    IntPropertyGetter, IntPropertySetter, NanNew<External>(&(field)));

#define DEFINE_STRUCT_INT_PROPERTY(strct, field) DEFINE_INT_PROPERTY(field, strct->field)

Genome::Genome(Local<Object> jsObj) {
  flam3_genome* g = reinterpret_cast<flam3_genome*>(flam3_malloc(sizeof(flam3_genome)));
  memset(g, 0, sizeof(flam3_genome));
  clear_cp(g, flam3_defaults_on);

  Init(g, jsObj);
}

Genome::Genome(flam3_genome* g) {
  Init(g, NanNew<Object>());
}

Genome::~Genome() {
  clear_cp(genome, flam3_defaults_on);
  flam3_free(genome);
}

void Genome::Init(flam3_genome* g, Local<Object> jsObj) {
  genome = g;
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
  // flam3_palette palette;
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
}

void Genome::Export(Handle<Object> exports) {
  NanScope();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
  tpl->SetClassName(NanNew<String>("Genome"));
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