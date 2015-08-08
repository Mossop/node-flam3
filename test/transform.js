const fs = require("fs");
const should = require("should");
const flam3 = require("../lib/index.js");
const path = require("path");

let data_dir = path.join(__dirname, "data");

describe("transform", () => {
  let xml = fs.readFileSync(path.join(data_dir, "test0.flam3"), { encoding: "UTF8" });
  let genomes = flam3.Genome.fromXMLString(xml, "test0.flam3");

  it("should load correctly from xml", () => {
    should(genomes).have.length(1);
    let genome = genomes[0];

    should(genome.transformCount).equal(4);

    for (let i = 0; i < genome.transformCount; i++) {
      let xform = genome.getTransform(i);
      should(xform.density).equal(0.25);
    }
  });

  it("can access variation fields", () => {
    let genome = genomes[0];
    let transform = genome.getTransform(0);

    should(transform.spherical.strength).equal(1);
    should(transform.linear.strength).equal(0);

    transform.spherical.strength = 0.2;
    transform.linear.strength = 0.7;

    genome = flam3.Genome.fromXMLString(genome.toXMLString(), "stdin")[0];
    transform = genome.getTransform(0);
    should(transform.spherical.strength).equal(0.2);
    should(transform.linear.strength).equal(0.7);
  });

  it("variation properties work", () => {
    let genome = genomes[0];
    let transform = genome.getTransform(0);

    should(transform.blob.strength).equal(0);
    should(transform.blob.low).equal(0);
    should(transform.blob.high).equal(1);
    should(transform.blob.waves).equal(1);

    transform.blob = { strength: 1, low: 0.2, high: 0.3, waves: 0.7 };

    should(transform.blob.strength).equal(1);
    should(transform.blob.low).equal(0.2);
    should(transform.blob.high).equal(0.3);
    should(transform.blob.waves).equal(0.7);

    genome = flam3.Genome.fromXMLString(genome.toXMLString(), "stdin")[0];
    transform = genome.getTransform(0);

    should(transform.blob.strength).equal(1);
    should(transform.blob.low).equal(0.2);
    should(transform.blob.high).equal(0.3);
    should(transform.blob.waves).equal(0.7);

    should(transform.wedge_julia).have.properties([ "angle", "count", "power", "dist" ]);
  });

  it("can access coefficients", () => {
    let genome = genomes[0];
    let transform = genome.getTransform(0);

    should(transform.coefficients).eql([
      [ -0.681206, -0.0779465 ],
      [ 0.20769, 0.755065 ],
      [ -0.0416126, -0.262334 ]
    ]);

    should(transform.post).eql([
      [ 1, 0 ],
      [ 0, 1 ],
      [ 0, 0 ]
    ]);

    transform = genome.getTransform(1);

    should(transform.coefficients).eql([
      [ 0.953766, 0.48396 ],
      [ 0.43268, -0.0542476 ],
      [ 0.642503, -0.995898 ]
    ]);

    should(transform.post).eql([
      [ 1, 0 ],
      [ 0, 1 ],
      [ 0, 0 ]
    ]);
  });
});
