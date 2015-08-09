import sys
import os
from zipfile import ZipFile
from shutil import copyfileobj

if len(sys.argv) != 4:
    print("Incorrect number of arguments: %s\n" % (" ".join(sys.argv)))
    sys.exit(1)

strip = int(sys.argv[2][len("--strip-components="):])
archive = sys.argv[1]
path = sys.argv[3]

print("Extracting %s to %s\n" % (archive, path))

def get_extractions(zip, strip):
    for name in sorted(zip.namelist()):
        if name[-1:] == "/":
            continue
        parts = name.split("/")[strip:]
        yield (name, "/".join(parts))

def extract(zip, entry, target):
    parent = os.path.dirname(target)
    if not os.path.exists(parent):
        os.makedirs(parent)
    zfile = zip.open(entry, "r")
    rfile = open(target, "wb")
    copyfileobj(zfile, rfile)
    rfile.close()
    zfile.close()


zip = ZipFile(archive, "r")
for (entry, target) in get_extractions(zip, strip):
    target = os.path.join(path, target)
    extract(zip, entry, target)
zip.close()
