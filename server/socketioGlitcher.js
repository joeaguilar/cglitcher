var glitcher = require('bindings')('cglitcher');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require('fs');

fs.mkdir(__dirname +'/glitched', function(e) {
  console.log('already exists');
})

app.get('/', function (req, res) {
  res.sendFile(__dirname + '/index.html');
});



io.on('connection', function(socket) {
  io.emit('connected', 'username');
  io.emit('not glitched');


  // watch a directory to place images on html
  /*
    fs.watch(__dirname + '/glitches', function(event, filename) {
      // TODO: read the file and pass it along

      io.emit('changed', function() {
        TODO: emit the changed event and pass the file upstream
      })
    })
  */

  socket.on('chat message', function(msg) {
    io.emit('chat message', msg);
  });
  
  socket.on('disconnect', function() {
    // console.log('User disconnected')
    io.emit('chat message', 'user disconnected');

  });
  
 socket.on('glitch', function(params) {
  var base64Data = params.png.replace(/^data:image\/png;base64/, "");
  var filename = __dirname + "/glitched/original"+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2) + ".png";
  var outfilename = __dirname + "/glitched/glitched"+ new Date().toISOString().slice(17,19) + (Math.random()).toString().slice(2) + ".png";
  fs.writeFile(filename, base64Data, 'base64', function(err) {
    if (err) {
      socket.emit('error', {error: error});
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

//   socket.on('glitch', function(params) {
//     if (!params.options || params.options.length < 1) {
//       console.log("No bufffer!");
//       io.emit('chat message', "Glitched!");
//       io.emit('not glitched');
//       return;
//     }
//     console.log("This is where you glitch it: ", params.options);

//     console.log(Object.keys(params));

//     params.options.forEach(function(v) {
//       console.log("Each effect: ", v.effect );
//     })
//     var length = params.options[0].length;
//     var socketArray = params.data;
//     var imgBuffer = new Buffer(length);
//     for (var i = 0; i < length; ++i) {
//       imgBuffer[i] = socketArray[i];
//     }
//     socketArray = null;
//     params.data = null;
//     console.log("Buffered image", imgBuffer);
//     console.log('pos 100: %d, length: %d ', imgBuffer[100], imgBuffer.length);
 
//     var sampleData = [ 
//       imgBuffer[24],
//       imgBuffer[240],
//       imgBuffer[2400],
//       imgBuffer[24000]
//     ]


//     try {
//       glitcher.UseBuffer(params.options, imgBuffer, function(buf) {
//         console.log('done!', buf);
//         // imgArray = new Uint8Array(buf);

//         if (imgBuffer[24] == buf[24]) console.log("First is the same: ", buf[24]);
//         if (imgBuffer[240] == buf[240]) console.log("Second is the same: ", buf[240]);
//         if (imgBuffer[2400] == buf[2400]) console.log("Third is the same: ", buf[2400]);
//         if (imgBuffer[24000] == buf[24000]) console.log("Fouth is the same: ", buf[24000]);


//         io.emit('glitched', { imgArray: buf });
//         io.emit('chat message', "Glitched!");
//       })
//     } catch(e) {
//       console.log("Error: ", e.message);
//     }
//   })


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
  io.emit('not glitched', "server restarted!")
  process.stdout.write("Listening on :1337");
})