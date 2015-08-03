import sys
from urllib import urlretrieve

if len(sys.argv) != 3:
    print("Incorrect number of arguments: %s\n" % (" ".join(sys.argv)))
    sys.exit(1)

url = sys.argv[1][len("--url="):]
path = sys.argv[2]

print("Downloading %s to %s\n" % (url, path))
urlretrieve(url, path)
