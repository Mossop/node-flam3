const fs = require("fs");
const should = require("should");
const flam3 = require("../lib/index.js");
const path = require("path");

let data_dir = path.join(__dirname, "data");

describe("transform", () => {
  it("should load correctly from xml", () => {
    let xml = fs.readFileSync(path.join(data_dir, "test0.flam3"), { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test0.flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    should(genome.transformCount).equal(4);

    for (let i = 0; i < genome.transformCount; i++) {
      let xform = genome.getTransform(i);
      should(xform.density).equal(0.25);
    }
  });

  it("can access variation fields", () => {
    let xml = fs.readFileSync(path.join(data_dir, "test0.flam3"), { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, "test0.flam3");
    should(genomes).have.length(1);
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
});
