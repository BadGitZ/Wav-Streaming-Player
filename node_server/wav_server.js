// WAV Streaming Player Server
// Handles .wav files and sends them to an ESP8266
//Author Stefan Schmolke 2019

var http = require('http');
var url = require('url');
var fs = require('fs');
var formidable = require('formidable');
var path = require('path');

var count = 0;

console.log("Server starting");

http.createServer(function (req, res) {
	count++;
	console.log("Request Count: %d", count);

	var q = url.parse(req.url, true);
	var filename = "." + q.pathname;

	if (req.url == '/fileupload') {
		console.log("Fileupload requested");
		var form = new formidable.IncomingForm();
		form.parse(req, function (err, fields, files) {
			var oldpath = files.filetoupload.path;
			var newpath = '/home/badgitz/' + files.filetoupload.name;
			fs.rename(oldpath, newpath, function (err) {
				if (err) throw err;
					res.write('File uploaded and moved!');
					res.end();
			});
		});
	}

	else {
		fs.readFile(filename, function(err, data) {
		if (err) {
			console.log("404 Error ocurred");
			res.writeHead(404, {'Content-Type': 'text/html'});
			return res.end("404 Not Found");
		}
    res.writeHead(200, {'Content-Type': 'text/html'});
    var n = data.indexOf("|a|");
    data[n+1] = count+48;
    res.write(data);
    res.write("</br>");

    if (req.url == "/test.html") {
		console.log("test.html requested");
		res.write('<form action="fileupload" method="post" enctype="multipart/form-data">');
		res.write('<input type="file" name="filetoupload"></br></br>');
		res.write('<input type="submit">');
		res.write('</form>');
	}

    return res.end();
  });

}
}).listen(8080);
