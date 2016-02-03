var glitcher = require('bindings')('cglitcher');
var fs = require('fs');
var http = require('http');
// var filename = process.argv[2];
var globalFileName  = process.argv[2], globalOrderTracker = 0;
var extension = ".png";




var server = http.createServer(function (req, res) {
  var url = require('url').parse(req.url);
  if (url.pathname == "/canvas") {
    // res.setHeader('Content-Type', 'text/html')
    // res.writeHead(200, {'Content-Type': 'text/plain'});
    // res.end('okay');
    

  }
})


function createObject(order) {
  var Emap  = "andefghonpswxyzpABCD";
  var Chmap = "12457";
  var obj = {};
  var number = Math.floor(Math.random() * (Emap.length-1));
  console.log(number)
  obj.effect = Emap[number].charCodeAt();
  obj.order = ( order === 0) ? 1 : 0;
  obj.channel = parseInt(Chmap[Math.round(Math.random() * Chmap.length-1)], 10);
  obj.threshold = Math.floor(Math.random() * 255);
  obj.depth  = Math.random() * 2.0;
  obj.xspace = Math.round(Math.random() * 20);
  obj.yspace = Math.round(Math.random() * 20);
  obj.stroke = Math.round(Math.random() * 200);
  obj.weight = Math.round(Math.random() * 200);
  obj.color = 0xFF1166FF;
  
  return obj;
}


function verifyOptions(obj) {
  obj.effect = (obj.effect !== null ) ? 'g' : obj.effect;
  obj.order  = (obj.order !== null ) ? '1' : obj.order;
  obj.channel = ( !(obj.channel < 8) || !(obj.channel > 0) ) ? 0 : obj.channel;
  obj.threshold = ( !(obj.threshold > 0) || !(obj.threshold < 256) ) ? 50 : obj.threshold;     
  obj.depth = ( !(obj.depth > 0) || !(obj.depth < 256) ) ? 1.0 : obj.depth;     
  obj.xspace = ( !(obj.xspace > 0) || !(obj.xspace < 256) ) ? 50 : obj.xspace;     
  obj.yspace = ( !(obj.yspace > 0) || !(obj.yspace < 256) ) ? 50 : obj.yspace;     
  obj.weight = ( !(obj.weight > 0) || !(obj.weight < 256) ) ? 50 : obj.weight;     
  obj.stroke = ( !(obj.stroke > 0) || !(obj.stroke < 256) ) ? 50 : obj.stroke;     
  if (isLSB()) {
    // LSB architecture
    obj.color =  ( !(obj.color > 0xFF000000) || !(obj.color < 0xFFFFFFFF) ) ? 0xFFFFFFFF : obj.color;
  } else {
    // MSB architecture
    obj.color =  ( !(obj.color > 0x000000FF) || !(obj.color < 0xFFFFFFFF) ) ? 0xFFFFFFFF : obj.color;     
  }
}



function packArray () {
  var stack = [], order = 0;
  var runNumber = parseInt(Math.round(Math.random() * 10), 10) + 1;
  for (var i = 0; i < runNumber; ++i) {
    
    stack.push(createObject(order));
    order  = (order == 0) ? 1 : 0;
  }
  return stack;
}

function writeSomeFiles() {
  var globalFileName = recursiveFileName();
  var pngFile = randomFileString +extension;
  var tiffFile = randomFileString +'.tif';
  var stack = packArray();
  // Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
  glitcher.UseFilename(filename, pngFile, stack, function() {
    console.log("Done with %s.jpeg!", randomFileString);
  });
  
    glitcher.UseFilename(filename, tiffFile, stack, function() {
    console.log("Done with %s.tif!", randomFileString);
  });
}



function writeTheSameFile() {
  if (globalCount > 4) return;
  globalCount = globalCount + 1;
  filename = globalFileName;
  globalFileName = recursiveFileName() + extension;
  // var pngFile = globalFileName +'.png';
  globalOrderTracker = ( globalOrderTracker === 0) ? 1 : 0;
  // Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
  glitcher.UseFilename(filename, globalFileName, [createObject(globalOrderTracker), createObject(globalOrderTracker) ], writeTheSameFile);
}
// Recursive FTW!
var globalCount = 0;
 
writeTheSameFile();

// for (var x = 0; x < 50; ++x) {
//   setTimeout(writeSomeFiles, 1000);
// }


function recursiveFileName() {
  var filename = './cglitcher'+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2);  
  
  fs.stat(filename +".png", function(err, stat) {
    if (err == null) {
      console.log('File exists!')
      filename = recursiveFileName();
    } else if (err.code == 'ENOENT') {
      return;
    }
  });
  return filename;

}


/*
  Effect map
  case 'a' : zoomth(pixels, clone, opts);
  case 'b' : zoom_b(pixels, clone, opts);  
  case 'c' : zoom_r(pixels, clone, opts);
  case 'd' : crunch(pixels, clone, opts);
  // SHAPES 
  case 'e' : pixelate(pixels, clone, opts);
  case 'f' : outtashape(pixels, clone, opts);
  case 'g' : outtacircle(pixels, clone, opts);
  case 'h' : outtasquare(pixels, clone, opts);
  case 'i' : outtatriangle(pixels, clone, opts);
   // COLORSWAP 
  case 'j' : rgb_2_bgr(pixels, clone, opts);
  case 'k' : rgb_2_gbr(pixels, clone, opts);
  case 'l' : rgb_2_grb(pixels, clone, opts);
  case 'm' : rgb_2_brg(pixels, clone, opts);
  // AUIDO 
  case 'n' : wet_reverb(pixels, clone, opts);
  // DISPLACEMENTS 
  case 'o' : deplace(pixels, clone, opts);        
  // PIXELSORTS 
  case 'p' : pixelsort(pixels, clone, opts); 
  // SMOSHES 
  case 's' : smosht(pixels, clone, opts); 
  case 't' : posht(pixels, clone, opts); 
  case 'u' : fosht(pixels, clone, opts);  
  case 'v' : losht(pixels, clone, opts);
 // GRADIENT BLEND
  case 'w' : gradient_blend_add_sample(pixels, clone, opts);
  case 'x' : gradient_blend_add_source(pixels, clone, opts);
  case 'y' : gradient_blend_subtract_sample(pixels, clone, opts);
  case 'z' : gradient_blend_subtract_source(pixels, clone, opts);
*/




