var glitcher = require('bindings')('cglitcher');
var app = require('express')();
var http = require('http').Server(app);
var path = require('path');
var io = require('socket.io')(http);
var fs = require('fs');

fs.mkdir(__dirname +'/../test/glitched', function(e) {
  console.log('Directory exists at: ' + __dirname +'../test/glitched');
})

app.get('/', function (req, res) {
  res.sendFile(path.resolve(__dirname + '/../views/index.html'));

});



io.on('connection', function(socket) {
  io.emit('connected', 'username');
  io.emit('reset');


  // watch a directory to place images on html
  /*
    fs.watch(__dirname + '/glitches', function(event, filename) {
      // TODO: read the file and pass it along

      io.emit('changed', function() {
        TODO: emit the changed event and pass the file upstream
      })
    })
  */

  socket.on('message', function(msg) {
    io.emit('message', msg);
  });
  
  socket.on('disconnect', function() {
    // console.log('User disconnected')
    io.emit('message', 'user disconnected');

  });
  socket.on('error', function(err) {
    io.emit('error', err);
  })
  
 socket.on('glitch', function(params) {
  var base64Data = params.png.replace(/^data:image\/png;base64/, "");
  var filename = __dirname + "/../test/glitched/original"+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2) + ".png";
  var outfilename = __dirname + "/../test/glitched/glitched"+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2) + ".png";
  fs.writeFile(filename, base64Data, 'base64', function(err) {
    if (err) {
      socket.emit('error', {error: err});
      console.log(err);
      return;
    };
    console.log('Saved!');

    glitcher.UseFilename(filename, outfilename, params.options, function() {
      // console.log("Done with %s!", pngFile, filename);
      fs.readFile(outfilename, function(err, buffer) {
        if (err) {
          socket.emit('error', {error: error});
          return;
        }
        socket.emit('glitched', {buffer: buffer });
      });
    });
  });
 })


// TODO: Use raw data buffer, currently this is too much data to send over


});

app.post('/', function (req, res) {
  var imageBuffer;
  req.on('data', function (data) {
    // push data to buffer;

  })

  req.on('end', function (req, res) {
    // run data through glitcher
    // send binary data back

    res.send(imageBuffer)
  })

  req.on('error', function (err) {
    process.stderr.write('Error: ', err.message);
  })
});


http.listen(1337, function() {
  io.emit('reset', "server restarted!")
  process.stdout.write("Listening on :1337\n");
})