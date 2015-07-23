const fs = require("fs");
const should = require("should");
const flam3 = require("../lib/index.js");
const { PNG } = require("pngjs");
const path = require("path");

let test_dir = path.join(path.dirname(flam3.path), "tests");
let data_dir = path.join(__dirname, "data");

function testImage(root) {
  it(root + " should render correctly", function (done) {
    let xml = fs.readFileSync(path.join(data_dir, root + ".flam3"), { encoding: "UTF8" });
    let genomes = flam3.Genome.fromXMLString(xml, root + ".flam3");
    should(genomes).have.length(1);
    let genome = genomes[0];

    genome.render({ seed: "testseed" }, function(err, data) {
      should(err).be.null();

      let png = new PNG();
      png.on("parsed", (pngdata) => {
        should(genome.height).equal(png.height);
        should(genome.width).equal(png.width);
        should(data.length).equal(pngdata.length);

        let pixel_count = 0;
        let total_difference = 0;
        for (let i = 0; i < data.length; i += 4) {
          let bad = false;
          for (let j = i; j < i + 4; j++) {
            let color_diff = data[j] - pngdata[j];
            if (color_diff != 0) {
              bad = true;
              total_difference += color_diff;
            }
          }

          if (bad)
            pixel_count++;
        }

        pixel_count = 100 * pixel_count / (png.height * png.width);
        should(pixel_count).equal(0);
        total_difference = total_difference / (png.height * png.width);
        should(total_difference).equal(0);

        done();
      });

      fs.createReadStream(path.join(test_dir, root + ".png")).pipe(png);
    });
  });
}

describe("render", () => {
  testImage("test0");
  testImage("test1");
});
