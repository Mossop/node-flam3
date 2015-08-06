import os
import sys
import subprocess

if (len(sys.argv) != 4):
    sys.exit(1)

def render(flame, output):
    env = {
        "isaac_seed": "testseed",
        "seed": "testseed",
        "in": flame,
        "flam3_palettes": os.path.join(sys.argv[1], "flam3-palettes.xml"),
        "out": output
    }

    subprocess.check_call([os.path.join(sys.argv[1], "flam3-render")], env = env)

bindir = sys.argv[1]
sourcedir = sys.argv[2]
outputdir = sys.argv[3]

try:
    os.mkdir(testdir)
except:
    pass

render(os.path.join(sourcedir, "test0.flam3"), os.path.join(outputdir, "test0.png"))
render(os.path.join(sourcedir, "test1.flam3"), os.path.join(outputdir, "test1.png"))
