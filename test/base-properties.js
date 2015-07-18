var should = require("should");
var flam3 = require("../lib/index.js");

describe('flam3', function() {
  describe('#threadCount', function () {
    it('should be >= 1', function () {
      should(flam3.threadCount).be.above(0);
    });
  });

  describe('#version', function () {
    it('should be a string', function () {
      should(flam3.version).endWith("v3.1.1");
    });
  });
});
