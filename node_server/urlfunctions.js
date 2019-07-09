var http = require('http');
var url = require('url');

exports.urlGen = function(size) {
  var result           = '';
  var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  var charactersLength = characters.length;
  for ( var i = 0; i < size; i++ ) {
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
  }
  return result;
}

exports.errorResponse = function(res) {
  console.log("404 Error ocurred");
	res.writeHead(404, {'Content-Type': 'text/html'});
	return res.end("404 Not Found");
}
