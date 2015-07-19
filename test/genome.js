const should = require("should");
const flam3 = require("../lib/index.js");

function checkGenome(createGenome) {
  it("can access gamma property", () => {
    let g = createGenome();

    should(g.gamma).equal(4);
    should(g.gamma = 6).equal(6);
    should(g.gamma).equal(6);
  });
}

describe("genome", () => {
  describe("new", () => {
    checkGenome(() => new flam3.Genome());
  });

  describe("function", () => {
    checkGenome(() => flam3.Genome());
  });

  if ("gc" in global) {
    describe("gc", () => {
      it("increases and decreases correctly", () => {
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
    });
  }
});
