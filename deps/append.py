import sys

if len(sys.argv) != 4:
    print("Incorrect number of arguments: %s\n" % (" ".join(sys.argv)))
    sys.exit(1)

path = sys.argv[1]
extra = sys.argv[2]
touch = sys.argv[3]

f = open(extra)
lines = f.readlines()
f.close()

f = open(path, "a")
for line in lines:
    f.write(line)
f.close()

f = open(touch, "w")
f.close()
