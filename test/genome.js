var should = require("should");
var flam3 = require("../lib/index.js");

function checkGenome(createGenome) {
  it("can access gamma property", function() {
    var g = createGenome();

    should(g.gamma).equal(4);
    should(g.gamma = 6).equal(6);
    should(g.gamma).equal(6);

    g = null;
    global.gc();
  });
}

describe("genome", function() {
  describe("new", function () {
    checkGenome(function() {
      return new flam3.Genome();
    });
  });

  describe("function", function () {
    checkGenome(function() {
      return flam3.Genome();
    });
  });

  describe("gc", function() {
    it("increases and decreases correctly", function() {
      var count = flam3.genomeCount;
      var g = new flam3.Genome();
      should(flam3.genomeCount).equal(count + 1);

      global.gc();
      should(flam3.genomeCount).equal(count + 1);

      g = null;
      global.gc();
      should(flam3.genomeCount).equal(count);
    });
  });
});
