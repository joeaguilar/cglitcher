var glitcher = require('bindings')('cglitcher');
var fs = require('fs');
var path = require('path');
var utils = require('./utils');
var extension = ".png";


var pp = path.parse(process.argv[2]);

var queue = [];
var running = false;

fs.readdir(process.argv[2], function(err, files) {
  
  var fl = files.length;
  
  for (var i = 0; i < fl; ++i) {
    var eachFile = pp.dir + "/" + pp.base + "/" + files[i];
    var fmtObj = path.parse(eachFile);
    var curdir = process.argv[2] + fmtObj.name;

    // make sure that every file is a jpg/png/tif before continuing;
    if ( !fmtObj.ext.match(/jpg|jpeg|tif|tiff|png/gi) ) {
    
      continue;
    } 
   queue.push([curdir, eachFile]);
  }
  runTheQueue();
});




function runTheQueue () {
  if (!queue.length) return;
  if (running) setTimeout(runTheQueue, 100);
  else {
  var x = queue.pop();
  running = true;
  runTheGlitchies(x[0], x[1]);
  setTimeout(runTheQueue, 100);
    
  }
}
  


function runTheGlitchies(curdir, eachFile) {
  fs.mkdir(curdir, function(err) { 
      glitchSomeFiles(eachFile, curdir);
      running = false;
  })
}


function glitchSomeFiles(filename,base) {
  var randomFileString = recursiveFileName(base);
  var pngFile = randomFileString + extension;
  var stack = utils.createRandomOptionStack();

  // Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
  glitcher.UseFilename(filename, pngFile, stack, function() {
    
  });
}










