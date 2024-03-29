const should = require("should");
const flam3 = require("../lib/index.js");

function checkGenome(g) {
  it("is a genome", () => {
    should(g).be.instanceOf(flam3.Genome);
  });

  it("can access gamma property", () => {
    should(g.gamma).equal(4);
    should(g.gamma = 6).equal(6);
    should(g.gamma).equal(6);

    let reloaded = flam3.Genome.fromXMLString(g.toXMLString(), "stdin")[0];
    should(reloaded.gamma).equal(6);
  });

  it("can access background", () => {
    should(g).has.property("background");
    should(g.background).has.properties(["red", "green", "blue"]);
    should(g.background).not.have.property("alpha");
    should(g.background.red).equal(0);
    should(g.background.green).equal(0);
    should(g.background.blue).equal(0);

    g.background.red = 0.5;
    g.background.blue = 0.6;

    should(g.background.red).equal(0.5);
    should(g.background.green).equal(0);
    should(g.background.blue).equal(0.6);

    let parsed = flam3.Genome.fromXMLString(g.toXMLString(), "stdin")[0];

    should(parsed.background.red).equal(0.5);
    should(parsed.background.green).equal(0);
    should(parsed.background.blue).equal(0.6);

    delete parsed.background.red;
    should(parsed.background).not.have.property("red");
    parsed = flam3.Genome.fromXMLString(parsed.toXMLString(), "stdin")[0];

    should(parsed.background.red).equal(0);
    should(parsed.background.green).equal(0);
    should(parsed.background.blue).equal(0.6);

    delete parsed.background;
    parsed = flam3.Genome.fromXMLString(parsed.toXMLString(), "stdin")[0];

    should(parsed.background.red).equal(0);
    should(parsed.background.green).equal(0);
    should(parsed.background.blue).equal(0);
  });

  it("can access centers", () => {
    should(g.center).has.properties(["x", "y"]);

    g.center.x = 0.5;
    g.center.y = 0.7;

    should(g.center.x).equal(0.5);
    should(g.center.y).equal(0.7);

    let parsed = flam3.Genome.fromXMLString(g.toXMLString(), "stdin")[0];

    should(parsed.center.x).equal(0.5);
    should(parsed.center.y).equal(0.7);
  });

  it("can access name property", () => {
    should(g.name).equal("");
    should(g.name = "foobar").equal("foobar");
    should(g.name).equal("foobar");

    let reloaded = flam3.Genome.fromXMLString(g.toXMLString(), "stdin")[0];
    should(reloaded.name).equal("foobar");

    should.doesNotThrow(() => g.name = "0123456789012345678901234567890123456789012345678901234567890123");
    should(g.name).equal("0123456789012345678901234567890123456789012345678901234567890123");
    should.throws(() => g.name = "01234567890123456789012345678901234567890123456789012345678901234", TypeError, "Name was too long");
    should(g.name).equal("0123456789012345678901234567890123456789012345678901234567890123");
  });

  it("toXMLString works", () => {
    let str = g.toXMLString();

    should(str).startWith("<flame ");
    should(str).endWith("</flame>\n");

    should(str).match(RegExp('size="' + g.height + ' ' + g.width + '"'));
  });

  it("palette should look sane", () => {
    should(g.palette).be.instanceOf(Array);
    should(g.palette).have.length(256);
    should(g.palette[0]).have.properties([ "red", "green", "blue", "alpha" ]);
  });

  it("palette should not be all white", () => {
    let sum = g.palette.reduce((p, c) => p + c.red + c.green + c.blue + c.alpha, 0);
    should(sum).be.lessThan(256 * 4);
  });

  it("palette should match itself", () => {
    should(g.palette).equal(g.palette);
    should(g.palette[0]).equal(g.palette[0]);
  });

  it("can change palette", () => {
    should(delete g.palette).be.true();
    should(g).not.have.property("palette");

    let parsed = flam3.Genome.fromXMLString(g.toXMLString(), "stdin")[0];
    let sum = parsed.palette.reduce((p, c) => p + c.red + c.green + c.blue + c.alpha, 0);
    should(sum).equal(256 * 4);
  });

  it("cannot delete properties", () => {
    g.gamma = 4;
    should.throws(() => delete g.gamma, TypeError, "Cannot delete property");
    should(g.gamma).equal(4);
  });
}

describe("genome", () => {
  describe("new", () => {
    checkGenome(new flam3.Genome());
  });

  describe("function", () => {
    checkGenome(flam3.Genome());
  });

  describe("random", () => {
    it("appears to work", () => {
      let genome = flam3.Genome.createRandom();
      should(genome.height).equal(100);
      should(genome.width).equal(100);
    });

    checkGenome(flam3.Genome.createRandom());
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

    checkGenome(flam3.Genome.fromXMLString(flam3.Genome.createRandom().toXMLString(), "stdin")[0]);
  });

  describe("Palette", () => {
    it("Can't call", () => {
      should.throws(() => flam3.Palette(), TypeError, "cannot be constructed from JavaScript");
      should.throws(() => new flam3.Palette(), TypeError, "cannot be constructed from JavaScript");
      should.throws(() => flam3.Palette(5), TypeError, "cannot be constructed from JavaScript");
      should.throws(() => new flam3.Palette(4), TypeError, "cannot be constructed from JavaScript");
    })
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
        global.gc();
        should(flam3.genomeCount).equal(count + 1);

        g = null;
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);
      });

      it("palette references don't hold the genome alive", () => {
        global.gc();
        global.gc();
        global.gc();

        let count = flam3.genomeCount;
        let g = flam3.Genome.createRandom();
        should(flam3.genomeCount).equal(count + 1);

        let palette = g.palette;
        g = null;
        global.gc();
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);

        let entry = palette[0];
        palette = null;
        global.gc();
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);

        entry = null;
        global.gc();
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);
      });

      it("transform references hold the genome alive", () => {
        global.gc();
        global.gc();
        global.gc();

        let count = flam3.genomeCount;
        let g = flam3.Genome.createRandom();
        should(flam3.genomeCount).equal(count + 1);

        should(g.transformCount).be.greaterThan(0);
        let transform = g.getTransform(0);
        g = null;
        global.gc();
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count + 1);

        transform = null;
        global.gc();
        global.gc();
        global.gc();
        should(flam3.genomeCount).equal(count);
      });
    });
  }
});
