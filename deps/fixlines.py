import sys

if len(sys.argv) != 3:
    print("Incorrect number of arguments: %s\n" % (" ".join(sys.argv)))
    sys.exit(1)

path = sys.argv[1]
touch = sys.argv[2]

f = open(path, "r")
lines = f.readlines()
f.close()

lines = [l.rstrip() for l in lines]

f = open(path, "w")
f.write("\n".join(lines))
f.close()

f = open(touch, "w")
f.close()
