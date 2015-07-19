const should = require("should");
const flam3 = require("../lib/index.js");

describe("flam3", () => {
  describe("#threadCount", () => {
    it("should be >= 1", () => {
      should(flam3.threadCount).be.above(0);
    });
  });

  describe("#version", () => {
    it("should be a string", () => {
      should(flam3.version).endWith("v3.1.1");
    });
  });

  describe("#genomeCount", () => {
    it("should be 0", () => {
      should(flam3.genomeCount).equal(0);
    });
  });
});
