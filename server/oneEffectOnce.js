var glitcher = require('bindings')('cglitcher');
var fs = require('fs');
var path = require('path');
// var filename = process.argv[2];
// var globalFileName  = process.argv[2], globalOrderTracker = 0;
var extension = ".png";

var file = process.argv[2];
var pp = path.parse(file);
  
var randomFileString = recursiveFileName(pp.dir + "/" +pp.name);
var pngFile = randomFileString + extension;
// var tiffFile = randomFileString +'.tif';
var stack = packArray();

// Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
glitcher.UseFilename(file, pngFile, stack, function() {
  console.log("Done with %s!", pngFile, file);
});




// function createObject(order) {
//   // var Emap  = "andefghonpswxyzpABCD";
//   var Emap  = "abcdefgjklmswxyzABCD"; // gif mode
//   var Chmap = "12457";
//   var obj = {};
//   var number = Math.floor(Math.random() * (Emap.length-1));
//   // console.log(number)
//   obj.effect = Emap[number].charCodeAt();
//   obj.order = ( order === 0) ? 1 : 0;
//   obj.channel = parseInt(Chmap[Math.round(Math.random() * Chmap.length-1)], 10);
//   obj.threshold = Math.floor(Math.random() * 255) + 1;
//   obj.depth  = Math.random() * 2.0 + 0.1;
//   obj.xspace = Math.round(Math.random() * 20) + 1;
//   obj.yspace = Math.round(Math.random() * 20) + 1;
//   obj.stroke = Math.round(Math.random() * 200) + 1;
//   obj.weight = Math.round(Math.random() * 200) + 1;
//   obj.color = 0xFF1166FF;
  
//   return obj;
// }


// function verifyObject(object) {
//    // object.effect    = (object.effect !== null ) ? 'g' : object.effect;
//    object.order     = (object.order !== null ) ? '1' : object.order;
//    object.channel   = (object.channel !== null || object.channel > 7 ) ? 0 : object.channel;
//    object.threshold = (object.threshold !== null || object.threshold > 100 ) ? 50 : object.threshold;
//    object.depth     = (object.depth !== null || object.depth > 2.0 || object.depth < 0.5 ) ? 1.0 : object.depth;
//    object.xspace    = (object.xspace < 1 || object.xspace > 20) ? 1 : object.xspace;
//    object.yspace    = (object.yspace < 1 || object.yspace > 20) ? 1 : object.yspace;
//    return object;
// }




// // var dir = path.dirname(process.argv[2]);
// var pp = path.parse(process.argv[2]);

// var queue = [];
// var running = false;

// fs.readdir(process.argv[2], function(err, files) {
//   // if (err) console.log('err', err);
//   var fl = files.length;
//   // console.log(files, fl);
//   for (var i = 0; i < fl; ++i) {
//     var eachFile = pp.dir + "/" + pp.base + "/" + files[i];
//     var fmtObj = path.parse(eachFile);
//     var curdir = process.argv[2] + fmtObj.name;

//     // make sure that every file is a jpg/png/tif before continuing;
//     if ( !fmtObj.ext.match(/jpg|jpeg|tif|tiff|png/gi) ) {
//       // console.log("Skipping: ", fmtObj); 
//       continue;
//     } 
//    queue.push([curdir, eachFile]);
//   }
//   runTheQueue();
// });




// function runTheQueue () {
//   if (!queue.length) return;
//   if (running) setTimeout(runTheQueue, 100);
//   else {
//   var x = queue.pop();
//   running = true;
//   runTheGlitchies(x[0], x[1]);
//   setTimeout(runTheQueue, 100);
    
//   }
// }
  


// function runTheGlitchies(curdir, eachFile) {
//   fs.mkdir(curdir, function(err) { 
//     // if (err) console.log('Folder existed');
//       glitchSomeFiles(eachFile, curdir);
//       running = false;
//   })
// }


// function glitchSomeFiles(filename,base) {
//   var randomFileString = recursiveFileName(base);
//   var pngFile = randomFileString + extension;
//   // var tiffFile = randomFileString +'.tif';
//   var stack = packArray();

//   // /Users/jaguil1/Desktop/glitchme/ZackKaminski.jpg
//   // console.log("EACHFILE::", filename)

//   // Make sure objects are cool
//   // console.log(stack);

//   // Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
//   glitcher.UseFilename(filename, pngFile, stack, function() {
//     console.log("Done with %s!", pngFile, filename);
//   });
// }



function packArray() {
  var stack = [];
  // var effectOrder = ['s','p', 'w', 'A', 'B'];
  // var combineOrder = [3, 2, 0, 1, 0];
  // var effectOrder = ['p'];
  // var combineOrder = [0];

  // var effectOrder = ['s','v', 'z', 'd', 'y', 'C'];
  // var combineOrder = [2, 0, 0, 0 , 0, 0];

  // var combineOrder = [  2,   0, 1, 0  ];
  // var effectOrder =  [ 'p', 'B', 'A', 'C' ];

  var combineOrder = [ 0,1,0 ];
  var effectOrder =  [ 'g', 's','X' ];


  // var combineOrder = [ 1,0,1,0,1,0,1,0 ];
  // var effectOrder =  [ 's','V', 's', 'A', 'p', 'X', 'x','V' ];  
  // var combineOrder = [ 0 ];
  // var effectOrder = [ 'h' ];


  if (effectOrder.length != combineOrder.length) {
    throw new SyntaxError("Impossible number of effects");
  }

  for (var i = 0, l = effectOrder.length; i < l; ++i) {
    stack[i] = {};
    stack[i].effect = effectOrder[i].charCodeAt();
    stack[i].order = combineOrder[i];
    stack[i].channel = 0;
    stack[i].threshold = 100;
    stack[i].depth = 1.0;
    stack[i].xspace = 10;
    stack[i].yspace = 10;
    stack[i].stroke = 5;
    stack[i].weight = 5;
    stack[i].color = 0xFF1166FF;
  }

  // stack.forEach(function(v,i) {
    
  //   v.effect = effectOrder[i].charCodeAt();
  //   v.order = combineOrder[i];
  //   v.channel = 0;
  //   v.threshold = 100;
  //   v.depth = 1.0;
  //   v.xspace = 5;
  //   v.yspace = 5;
  //   v.stroke = 5;
  //   v.weight = 5;
  //   v.color = 0xFF1166FF;
  // });

  return stack;
}



// function packArray () {
//   var stack = [], order = 0;
//   var runNumber = parseInt(Math.round(Math.random() * 10), 10) + 1;
//   for (var i = 0; i < runNumber; ++i) {
//     stack.push(createObject(order));
//     // stack.push(verifyObject(createObject(order)));
//     order  = (order == 0) ? 1 : 0;
//   }
//   return stack;
// }




function recursiveFileName(base) {
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




