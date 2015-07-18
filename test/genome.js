var should = require("should");
var flam3 = require("../lib/index.js");

function checkGenome(g) {
  it("can access gamma property", function() {
    should(g.gamma).equal(4);
    should(g.gamma = 6).equal(6);
    should(g.gamma).equal(6);
  });
}

describe("genome", function() {
  describe("new", function () {
    var g = new flam3.Genome();
    checkGenome(g);
  });

  describe("function", function () {
    var g = flam3.Genome();
    checkGenome(g);
  });
});
