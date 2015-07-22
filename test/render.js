const fs = require("fs");
const should = require("should");
const flam3 = require("../lib/index.js");
const { PNG } = require('pngjs');
const pngdiff = require('png-diff');

describe("render", () => {
  it("test0 should render correctly", function (done) {
    let xml = fs.readFileSync(__dirname + "/data/test0.flam3", { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test0.flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    genome.render({ seed: "testseed" }, function(err, data) {
      should(err).be.null();

      let png = new PNG({ width: genome.width, height: genome.height });
      png.data = data;

      pngdiff.outputDiffStream(png.pack(), __dirname + "/data/test0.png", function(err, stream, diffState) {
        should(err).be.null();
        should(diffState).equal(0);
        done();
      });
    });
  });

  it("test1 should render correctly", function (done) {
    let xml = fs.readFileSync(__dirname + "/data/test1.flam3", { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test1.flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    genome.render({ seed: "testseed" }, function(err, data) {
      should(err).be.null();

      let png = new PNG({ width: genome.width, height: genome.height });
      png.data = data;

      pngdiff.outputDiffStream(png.pack(), __dirname + "/data/test1.png", function(err, stream, diffState) {
        should(err).be.null();
        should(diffState).equal(0);
        done();
      });
    });
  });
});
