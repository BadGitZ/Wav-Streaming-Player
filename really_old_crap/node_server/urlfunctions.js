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

exports.simplePow = function(a, b) {
  var c = a;
  for (var i = 0; i < b; i++) {
    a *= c;
  }
  return a;
}
