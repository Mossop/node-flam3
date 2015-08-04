import os
import sys

if (len(sys.argv) != 5):
    print("Not enough arguments passed")
    sys.exit(1)

source = sys.argv[1]
target = sys.argv[2]
struct = sys.argv[3][len("--struct="):]
prefix = sys.argv[4][len("--prefix="):]

f = open(source, "r")
properties = [l.strip().split() for l in f.readlines()]

f = open(target, "w")

f.write("#define %s_PROPERTY_COUNT %d\n" % (prefix, len(properties)))
f.write("#define %s_PROPERTIES { \\\n" % (prefix))
for tokens in properties:
    if len(tokens) < 2:
        continue
    typ = "INT_FIELD"
    if tokens[0] == "double":
        typ = "DOUBLE_FIELD"
    f.write("  { \"%s\", %s, offsetof(%s, %s) }, \\\n" % (tokens[1], typ, struct, tokens[1]))
f.write("}\n")
