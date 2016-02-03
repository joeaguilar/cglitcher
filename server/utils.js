var fs = require('fs');

var totalEffects = "abcdefghijklmnopstuvwxyzABCDWXYZ";
function createOptionsObject(order) {
  // var Emap  = "andefghonpswxyzpABCD";
  var Emap  = "abcdefgjklmswxyzABCD"; 
  var Chmap = "12457";
  var obj = {};
  var number = Math.floor(Math.random() * (Emap.length-1));
  // console.log(number)
  obj.effect = Emap[number].charCodeAt();
  obj.order = ( order === 0) ? 1 : 0;
  obj.channel = parseInt(Chmap[Math.round(Math.random() * Chmap.length-1)], 10);
  obj.threshold = Math.floor(Math.random() * 255) + 1;
  obj.depth  = Math.random() * 2.0 + 0.1;
  obj.xspace = Math.round(Math.random() * 20) + 1;
  obj.yspace = Math.round(Math.random() * 20) + 1;
  obj.stroke = Math.round(Math.random() * 200) + 1;
  obj.weight = Math.round(Math.random() * 200) + 1;
  obj.color = 0xFF1166FF;
  
  return obj;
}


function verifyOptionsObject(object) {
   // object.effect    = (object.effect !== null ) ? 'g' : object.effect;
   object.order     = (object.order !== null ) ? '1' : object.order;
   object.channel   = (object.channel !== null || object.channel > 7 ) ? 0 : object.channel;
   object.threshold = (object.threshold !== null || object.threshold > 100 ) ? 50 : object.threshold;
   object.depth     = (object.depth !== null || object.depth > 2.0 || object.depth < 0.5 ) ? 1.0 : object.depth;
   object.xspace    = (object.xspace < 1 || object.xspace > 20) ? 1 : object.xspace;
   object.yspace    = (object.yspace < 1 || object.yspace > 20) ? 1 : object.yspace;
   return object;
}



function createRandomOptionStack () {
  var stack = [], order = 0;
  var runNumber = parseInt(Math.round(Math.random() * 10), 10) + 1;
  for (var i = 0; i < runNumber; ++i) {
    stack.push(createOptionsObject(order));
    // stack.push(verifyOptionsObject(createOptionsObject(order)));
    order  = (order == 0) ? 1 : 0;
  }
  return stack;
}




function recursiveFileName(base, extension) {
  var filename = base + '/cglitcher'+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2);  
  
  fs.stat(filename + extension, function(err, stat) {
    if (err == null) {
      console.log('File exists!')
      filename = recursiveFileName();
    } else if (err.code == 'ENOENT') {
      return;
    }
  });
  return filename;

}


function displayhelp() {

  process.stdout.write("Options for usage:                                  \n");
  process.stdout.write("scriptname file [order:effect,order:effect]         \n");
  process.stdout.write("oneeffectonce image [2:s,0:W,1:p]                   \n");
  process.stdout.write("The program passes two versions of the image around \n");
  process.stdout.write("it will only print out the image on the first buffer\n");
  process.stdout.write("so in order to acheive the best results, end with   \n");
  process.stdout.write("the order at 0.                                     \n");
  process.stdout.write("Effect Map:                                         \n");
  process.stdout.write('key | effect                                        \n');
  // process.stdout.write("ZOOMS\n")
  process.stdout.write("a: zoomth                                           \n");
  process.stdout.write("b: zoom_b                                           \n");
  process.stdout.write('c: zoom_r                                           \n');
  process.stdout.write('d: crunch                                           \n');
  // SHAPES 
  // process.stdout.write("SHAPES\n")
  process.stdout.write('e: pixelate                                         \n');
  process.stdout.write('f: outtashape                                       \n');
  process.stdout.write('g: outtacircle                                      \n');
  process.stdout.write('h: outtasquare                                      \n');
  process.stdout.write('i: outtatriangle                                    \n');
   // COLORSWAP 
  // process.stdout.write("COLORSWAPS\n")
  process.stdout.write('j: rgb_2_bgr                                        \n');
  process.stdout.write('k: rgb_2_gbr                                        \n');
  process.stdout.write('l: rgb_2_grb                                        \n');
  process.stdout.write('m: rgb_2_brg                                        \n');
  // AUIDO 
  // process.stdout.write("AUDIO\n")
  process.stdout.write('n: wet_reverb                                       \n');
  // DISPLACEMENTS 
  // process.stdout.write("DISPLACEMENTS\n")
  process.stdout.write('o: deplace                                          \n');
  // PIXELSORTS 
  // process.stdout.write("PIXELSORTS\n")
  process.stdout.write('p: pixelsort                                        \n');
  // SMOSHES 
  // process.stdout.write("SMOSHES\n")
  process.stdout.write('s: smosht                                           \n');
  process.stdout.write('t: posht                                            \n');
  process.stdout.write('u: fosht                                            \n');
  process.stdout.write('v: losht                                            \n');
 // GRADIENT BLEND
  // process.stdout.write("GRADIENT BLENDS\n")
  process.stdout.write('w: gradient_blend_add_sample                        \n');
  process.stdout.write('x: gradient_blend_add_source                        \n');
  process.stdout.write('y: gradient_blend_subtract_sample                   \n');
  process.stdout.write('z: gradient_blend_subtract_source                   \n');

  process.stdout.write('w: gradient_blend_add_sample                        \n');
  process.stdout.write('x: gradient_blend_add_source                        \n');
  process.stdout.write('y: gradient_blend_subtract_sample                   \n');
  process.stdout.write('z: gradient_blend_subtract_source                   \n');

  process.stdout.write('A: gradient_blend_add_sample                        \n');
  process.stdout.write('B: gradient_blend_add_source                        \n');
  process.stdout.write('C: gradient_blend_subtract_sample                   \n');
  process.stdout.write('D: gradient_blend_subtract_source                   \n');


  process.stdout.write('W: gradient_blend_add_sample                        \n');
  process.stdout.write('X: gradient_blend_add_source                        \n');
  process.stdout.write('Y: gradient_blend_subtract_sample                   \n');
  process.stdout.write('Z: gradient_blend_subtract_source                   \n');


}

function errorOut() {
  process.stderr.write("Could not understand options                        \n");

}


module.exports = {
  displayhelp: displayhelp,
  totalEffects: totalEffects,
  recursiveFileName: recursiveFileName,
  createRandomOptionStack: createRandomOptionStack,
  verifyOptionsObject: verifyOptionsObject,
  createOptionsObject: createOptionsObject
};




