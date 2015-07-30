import os
import sys
import re

if (len(sys.argv) != 3):
    print("Not enough arguments passed (%d)" % len(sys.argv))
    sys.exit(1)

source = sys.argv[1]
target = sys.argv[2]

var_names = sorted([
  "linear",
  "sinusoidal",
  "spherical",
  "swirl",
  "horseshoe",
  "polar",
  "handkerchief",
  "heart",
  "disc",
  "spiral",
  "hyperbolic",
  "diamond",
  "ex",
  "julia",
  "bent",
  "waves",
  "fisheye",
  "popcorn",
  "exponential",
  "power",
  "cosine",
  "rings",
  "fan",
  "blob",
  "pdj",
  "fan2",
  "rings2",
  "eyefish",
  "bubble",
  "cylinder",
  "perspective",
  "noise",
  "julian",
  "juliascope",
  "blur",
  "gaussian_blur",
  "radial_blur",
  "pie",
  "ngon",
  "curl",
  "rectangles",
  "arch",
  "tangent",
  "square",
  "rays",
  "blade",
  "secant2",
  "twintrian",
  "cross",
  "disc2",
  "super_shape",
  "flower",
  "conic",
  "parabola",
  "bent2",
  "bipolar",
  "boarders",
  "butterfly",
  "cell",
  "cpow",
  "curve",
  "edisc",
  "elliptic",
  "escher",
  "foci",
  "lazysusan",
  "loonie",
  "pre_blur",
  "modulus",
  "oscilloscope",
  "polar2",
  "popcorn2",
  "scry",
  "separation",
  "split",
  "splits",
  "stripes",
  "wedge",
  "wedge_julia",
  "wedge_sph",
  "whorl",
  "waves2",
  "exp",
  "log",
  "sin",
  "cos",
  "tan",
  "sec",
  "csc",
  "cot",
  "sinh",
  "cosh",
  "tanh",
  "sech",
  "csch",
  "coth",
  "auger",
  "flux",
  "mobius",
], key = lambda v: len(v), reverse = True)

def get_var_name(variable):
    for name in var_names:
        if variable[0:len(name) + 1] == name + "_":
            return (name, variable[len(name) + 1:])
    raise Exception("Unknown variation variable %s" % variable)

variations = dict()

f = open(source, "r")
for line in f.readlines():
    match = re.match("^(\\S+)\\s+(\\S+)(?:\s+(\S+)\s+(\S+))?$", line)
    if match is None:
        continue
    type = match.group(1)
    variable = match.group(2)
    (var, name) = match.group(3, 4)
    if var is None:
        (var, name) = get_var_name(variable)

    if not var in variations:
        variations[var] = []
    variations[var].append((type, name, variable))

f = open(target, "w")

f.write("#define DEFINE_VARIATION_PROPERTIES \\\n");
for var in variations:
    f.write("  if (strcmp(var_name, \"%s\") == 0) { \\\n" % (var));
    for (type, name, variable) in variations[var]:
        f.write("    SetDoubleField(variation, \"%s\", xform->%s); \\\n" % (name, variable))
    f.write("  } \\\n")
f.write("\n")

f.write("#define RESET_VARIATION_PROPERTIES \\\n");
for var in variations:
    for (type, name, variable) in variations[var]:
        f.write("    xform->%s = 0; \\\n" % (variable))
f.write("\n")

f.write("#define GET_VARIATION_PROPERTIES \\\n");
for var in variations:
    f.write("  if (strcmp(var_name, \"%s\") == 0) { \\\n" % (var));
    for (type, name, variable) in variations[var]:
        f.write("    GetDoubleField(variation, \"%s\", xform->%s); \\\n" % (name, variable))
    f.write("  } \\\n")
f.write("\n")
