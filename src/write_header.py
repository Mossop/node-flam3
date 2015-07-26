import os
import sys

if (len(sys.argv) != 5):
    print("Not enough arguments passed")
    sys.exit(1)

source = sys.argv[1]
target = sys.argv[2]
struct = sys.argv[3]
prefix = sys.argv[4]

f = open(source, "r")
properties = [l.strip().split() for l in f.readlines()]

f = open(target, "w")

f.write("#define %s_PROPERTY_COUNT %d\n" % (prefix, len(properties)))
f.write("#define %s_PROPERTIES { \\\n" % (prefix))
for tokens in properties:
    typ = "INT"
    if tokens[0] == "double":
        typ = "DOUBLE"
    f.write("  { \"%s\", %s, offsetof(%s, %s) }, \\\n" % (tokens[1], typ, struct, tokens[1]))
f.write("}\n")
