const should = require("should");
const flam3 = require("../lib/index.js");

function checkGenome(createGenome) {
  it("can access gamma property", () => {
    let g = createGenome();

    should(g.gamma).equal(4);
    should(g.gamma = 6).equal(6);
    should(g.gamma).equal(6);
  });

  it("toXMLString works", () => {
    let g = createGenome();
    let str = g.toXMLString();

    should(str).startWith("<flame ");
    should(str).endWith("</flame>\n");

    should(str).match(RegExp('size="' + g.height + ' ' + g.width + '"'));
  });

  it("palette should look sane", () => {
    let g = createGenome();

    should(g.palette).have.length(256);
    should(g.palette[0]).have.properties([ "red", "green", "blue", "alpha" ]);
  });

  it("palette should not be all white", () => {
    let g = createGenome();

    let sum = g.palette.reduce((total, entry) => total + entry.red + entry.green + entry.blue + entry.alpha, 0);

    should(sum).be.lessThan(256 * 4);
  });
}

describe("genome", () => {
  describe("new", () => {
    checkGenome(() => new flam3.Genome());
  });

  describe("function", () => {
    checkGenome(() => flam3.Genome());
  });

  describe("random", () => {
    it("appears to work", () => {
      let genome = flam3.Genome.createRandom();
      should(genome.height).equal(100);
      should(genome.width).equal(100);
    });

    checkGenome(() => flam3.Genome.createRandom());
  });

  describe("loadFromXML", () => {
    it("throws on invalid arguments", () => {
      should.throws(() => flam3.Genome.fromXMLString(), TypeError, "Wrong number of arguments");
      should.throws(() => flam3.Genome.fromXMLString("foo"), TypeError, "Wrong number of arguments");
    });

    it("ignores invalid XML", () => {
      let parsed = flam3.Genome.fromXMLString("foobar", "stdin");
      should(parsed).is.null();
    });

    it("ignores missing flames", () => {
      let parsed = flam3.Genome.fromXMLString("<foobar/>", "stdin");
      should(parsed).be.null();
    });

    it("loads a genome correctly", () => {
      let original = new flam3.Genome();
      original.height = 240;
      original.width = 320;
      original.gamma = 3;

      let results = flam3.Genome.fromXMLString(original.toXMLString(), "stdin");
      should(results).be.Array();
      should(results).have.length(1);

      let parsed = results[0];
      should(parsed.height).equal(240);
      should(parsed.width).equal(320);
      should(parsed.gamma).equal(3);
    });

    checkGenome(() => flam3.Genome.fromXMLString(flam3.Genome.createRandom().toXMLString(), "stdin")[0]);
  });

  if ("gc" in global) {
    describe("gc", () => {
      it("increases and decreases correctly", () => {
        global.gc();
        global.gc();

        let count = flam3.genomeCount;
        let g = new flam3.Genome();
        should(flam3.genomeCount).equal(count + 1);

        global.gc();
        should(flam3.genomeCount).equal(count + 1);

        g = null;
        global.gc();
        should(flam3.genomeCount).equal(count);
      });

      it("palette references hold the genome alive", () => {
        global.gc();
        global.gc();

        let count = flam3.genomeCount;
        let g = flam3.Genome.createRandom();
        should(flam3.genomeCount).equal(count + 1);

        let palette = g.palette;
        g = null;
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count + 1);

        let entry = palette[0];
        palette = null;
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count + 1);

        entry = null;
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);
      });
    });
  }
});
