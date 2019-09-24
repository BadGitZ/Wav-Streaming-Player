// WAV Streaming Player Server
// Handles .wav files and sends them to an ESP8266
//Author Stefan Schmolke 2019

var http = require('http');
var url = require('url');
var fs = require('fs');
var formidable = require('formidable');

var uf = require('./urlfunctions');

var count = 0;
fileCounter = 10000;
var wavFiles = [];
var wavUrl = [];


//---------------------------------Pre Server Stuff-----------------------------
console.log("Server starting");
//Create URLs for files
fs.readdir(__dirname, function(err, items) {
	var j = 0;
		for (var i=0; i<items.length; i++) {
				if (items[i].endsWith(".wav")) {
					wavFiles[j] = items[i];
					wavUrl[j] = "/" + uf.urlGen(8);
					j++;
				}
			}
	});

//---------------------------------Server Stuff---------------------------------
http.createServer(function (req, res) {
	count++;
	console.log("Request Count: %d", count);

	var q = url.parse(req.url, true);
	var filename = "." + q.pathname;


//-----------------------------File Upload--------------------------------------
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
//-----------------------------Get List Of Files and URLs-----------------------
	else if (req.url == '/soundfiles') {
		console.log("soundfiles requested");
		res.writeHead(200, {'Content-Type': 'text/html'});
		res.write("" + wavFiles.length);
		for (var j=0; j<wavFiles.length; j++) {
			res.write("</br>" + wavFiles[j] + "  [" + wavUrl[j] + "]");
		}
		console.log(wavFiles);
		console.log(wavUrl);
		res.end();
	}
//-----------------------------Main Page For User Input And Output--------------
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
//-----------------------------Get Info For Specfic File------------------------
else if (req.url.startsWith("/info")) {

	//Getting The Wanted filename
	var currentSelection = req.url.replace("/info","");
	var ind = wavUrl.indexOf(currentSelection);
	//If File Is Found In List Of URLs
	if (ind != -1) {
		fs.readFile(wavFiles[ind], function(err, data) {
			//Gathering Of File Descriptive Information
			sampleRate = data[0x18] + data[0x19]*256 + data[0x1A]*uf.simplePow(256, 2) + data[0x1B]*uf.simplePow(256, 3);
			bps = data[0x22];
			numChannels = data[0x16];
			//Displaying The Data
			res.writeHead(200, {'Content-Type': 'text/html'});
			res.write(wavFiles[ind]);
			res.write("</br> SR: " + sampleRate);
			res.write("</br> BPS: " + bps);
			res.write("</br> NC: " + numChannels);
			res.end();
		});
	}
	//If Info With Wrong Url Is Requested
	else {
		res.writeHead(404, {'Content-Type': 'text/html'});
		console.log("Error URL not matchable");
		res.write("<h1>These Are Not The Droids You Are Looking For!</h1>");
		res.write("</br></br><p>File not found on Server...</p>")
		res.end();
	}
}

//-----------------------------Get Data For Specfic File------------------------
else if (req.url.startsWith("/data")) {
	//Getting The Wanted filename

	var currentSelection = req.url.replace("/data","");
	var ind = wavUrl.indexOf(currentSelection);
	//If File Is Found In List Of URLs
	if (ind != -1) {
		fs.readFile(wavFiles[ind], function(err, data) {
				res.writeHead(200, {'Content-Type': 'text/html'});

				for (var i = 0x2C+fileCounter-10000; i < (0x2C+fileCounter); i++) {
					res.write(String.fromCharCode(data[i]));
							//String.fromCharCode
				}
				res.end();
		});
		fileCounter += 10000;
		if (fileCounter >= 50000)
			fileCounter = 10000;
	}

	else {
		res.writeHead(404, {'Content-Type': 'text/html'});
		console.log("Error URL not matchable");
		res.write("<h1>These Are Not The Droids You Are Looking For!</h1>");
		res.write("</br></br><p>File not found on Server...</p>")
		res.end();
	}
}

//-----------------------------If No Known URL Is Requested---------------------
else {
	console.log("404 Error ocurred");
	res.writeHead(404, {'Content-Type': 'text/html'});
	return res.end("404 Not Found");
}
}).listen(8080);
