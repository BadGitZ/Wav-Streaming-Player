// WAV Streaming Player Server
// Handles .wav files and sends them to an ESP8266
//Author Stefan Schmolke 2019

var http = require('http');
var url = require('url');
var fs = require('fs');
var formidable = require('formidable');


var uf = require('./urlfunctions');

var count = 0;
var wavFiles = [];
var wavUrl = [];

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
			var newpath = __dirname + "/" + files.filetoupload.name;
			fs.rename(oldpath, newpath, function (err) {
				if (err) throw err;
					res.write('File uploaded and moved!');
					res.end();
			});
		});
	}

	else if (req.url == '/soundfiles') {
		console.log("soundfiles requested");
		fs.readdir(__dirname, function(err, items) {
    	console.log(items);
			var j = 0;
			res.writeHead(200, {'Content-Type': 'text/html'});
			for (var i=0; i<items.length; i++) {
        	if (items[i].endsWith(".wav")) {
						wavFiles[j] = items[i];
						wavUrl[j] = "/" + uf.urlGen(8);
						res.write(wavFiles[j] + " at [" + wavUrl[j] + "]</br>");
						j++;
					}
    	}
			console.log(wavFiles);
			console.log(wavUrl);
			res.write("</br>Soundfiles Test");
			res.end();


		});
	}

	else if (req.url == "/main") {
		fs.readFile("main.html", function(err, data) {

    res.writeHead(200, {'Content-Type': 'text/html'});
    var n = data.indexOf("|a|");
    data[n+1] = count+48;
    res.write(data);
    res.write("</br>");
		console.log("test.html requested");
		res.write('<form action="fileupload" method="post" enctype="multipart/form-data">');
		res.write('<input type="file" name="filetoupload"></br></br>');
		res.write('<input type="submit">');
		res.write('</form>');

    return res.end();
  });

}

else {
	console.log("404 Error ocurred");
	res.writeHead(404, {'Content-Type': 'text/html'});
	return res.end("404 Not Found");
}
}).listen(8080);
