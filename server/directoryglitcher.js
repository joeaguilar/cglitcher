var glitcher = require('bindings')('cglitcher');
var fs = require('fs');
var path = require('path');
var utils = require('./utils');
var extension = ".png";
var pp = path.parse(process.argv[2]);
var flag = process.argv[3] || "";


var combineOrder = [];
var effectOrder =  [];
var queue = [];
var running = false;

if (flag == "-help") {
  utils.displayhelp();
} else {
  parseEffectFlags(flag);
  fs.readdir(process.argv[2], function(err, files) {

    var fl = files.length;

    for (var i = 0; i < fl; ++i) {
      var eachFile = pp.dir + "/" + pp.base + "/" + files[i];
      var fmtObj = path.parse(eachFile);
      var curdir = process.argv[2] + fmtObj.name;

      // make sure that every file is a jpg/png/tif before continuing;
      if ( !fmtObj.ext.match(/jpg|jpeg|tif|tiff|png/gi) ) {

        // Skip files that aren't images
        continue;
      } 
     queue.push([curdir, eachFile]);
    }
    runTheQueue();
  });
  
}


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
  var randomFileString = utils.recursiveFileName(base, extension);
  var pngFile = randomFileString + extension;
  var stack = createOptionStack(combineOrder, effectOrder);

  // Signature: filename(string), outfile(string),  options(array[objects]), callback(function)
  glitcher.UseFilename(filename, pngFile, stack, function() {
    console.log("Done with %s!", pngFile, filename);
  });
}



function createOptionStack(combineOrder, effectOrder) {
  var stack = [];

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
    stack[i].xspace = 5;
    stack[i].yspace = 5;
    stack[i].stroke = 5;
    stack[i].weight = 5;
    stack[i].color = 0xFF1166FF;
  }


  return stack;
}


function parseEffectOrder(str) {
  var both;
  if ( !str.match(":") ) {
    utils.errorOut();
    return;
  }
  b = str.split(':');
  if ( (b[0] > 4) || (b[0] < 0) || isNaN(parseInt(b[0])) ) {
    utils.errorOut();
    return;
  }
  b[1] += "";
  if ( (b[1].length > 1) || (b[1].length < 1) || (utils.totalEffects.indexOf(b[1]) == -1) ) {
    utils.errorOut();
    return;
  }

  combineOrder.push(b[0]);
  effectOrder.push(b[1]);

}


function parseEffectFlags(flag) {
  var lines, effects;
  if (flag.length <= 0 || !(flag[0] == "[" && flag[flag.length-1] == "]") ) {
    utils.errorOut();
    throw new SyntaxError("Options not formatted correctly");
    return;
  }
  var flag = flag.replace(/\[|\]/gi, "")
  lines = flag.split(",");
  lines.forEach(function(v,i) {
    parseEffectOrder(v);
  })
}










