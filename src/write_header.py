import os
import sys

if (len(sys.argv) != 3):
    sys.exit(1)

source = sys.argv[1]
target = sys.argv[2]

f = open(source, "r")
properties = [l.strip().split() for l in f.readlines()]

f = open(target, "w")

f.write("#define GENOME_PROPERTY_COUNT %d\n" % (len(properties)))
f.write("#define GENOME_PROPERTIES { \\\n")
for tokens in properties:
    typ = "INT"
    if tokens[0] == "double":
        typ = "DOUBLE"
    f.write("  { \"%s\", %s, offsetof(flam3_genome, %s) }, \\\n" % (tokens[1], typ, tokens[1]))
f.write("}\n")
