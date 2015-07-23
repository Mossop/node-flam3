const fs = require("fs");
const should = require("should");
const flam3 = require("../lib/index.js");
const { PNG } = require("pngjs");
const pngdiff = require("png-diff");
const path = require("path");

let test_dir = path.join(path.dirname(flam3.path), "tests");
let data_dir = path.join(__dirname, "data");

describe("render", () => {
  it("test0 should render correctly", function (done) {
    let xml = fs.readFileSync(path.join(data_dir, "test0.flam3"), { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test0.flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    genome.render({ seed: "testseed" }, function(err, data) {
      should(err).be.null();

      let png = new PNG({ width: genome.width, height: genome.height });
      png.data = data;

      pngdiff.outputDiffStream(png.pack(), path.join(test_dir, "test0.png"), function(err, stream, diffState) {
        should(err).be.null();
        should(diffState).equal(0);
        done();
      });
    });
  });

  it("test1 should render correctly", function (done) {
    let xml = fs.readFileSync(path.join(data_dir, "test1.flam3"), { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test1.flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    genome.render({ seed: "testseed" }, function(err, data) {
      should(err).be.null();

      let png = new PNG({ width: genome.width, height: genome.height });
      png.data = data;

      pngdiff.outputDiffStream(png.pack(), path.join(test_dir, "test1.png"), function(err, stream, diffState) {
        should(err).be.null();
        should(diffState).equal(0);
        done();
      });
    });
  });
});
