/*
 * index.js
 * 
 * Copyright 2019 Badgitz <Badgitz@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Initialization Stuff------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

var osc = require("osc");
var fs = require('fs');

var hostAddress = "192.168.43.208";
//var hostAddress = "192.168.43.15"
var hostPort = 8888;

var wavFiles = [];
sampleRate = 0;

console.log("Server starting");

simplePow = function(a, b) {
    var c = a;
    for (var i = 0; i < b; i++) {
        a *= c;
    }
    return a;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/getFileList---------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

getFileList = function() {

    fs.readdir(__dirname, function(err, items) {
        var j = 0;
        //console.log(items);

        var fileMessage = {address: "/files", args: [{type: 0, value: 0}]}
        fileMessage.args[0].type = "i";
        fileMessage.args[0].value = wavFiles.length;
        for (var i = 1; i < wavFiles.length; i++) {
            fileMessage.args.push({type: "s", value: wavFiles[i]})
        }
            udpPort.send(fileMessage, hostAddress, hostPort);
    });
}

fs.readdir(__dirname, function(err, items) {
    var j = 0;
    //console.log(items);
		for (var i=0; i<items.length; i++) {
				if (items[i].endsWith(".wav")) {
                    wavFiles[j] = items[i];
					j++;
				}
		}
});

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Get File------------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

getFileData = function(filename) {
    console.log("CHECK");
    fs.readFile(filename, function(err, data) {
        console.log("File Requested " + filename);
        var sampleRate = data[0x18] + data[0x19]*256 + data[0x1A]*simplePow(256, 2) + data[0x1B]*simplePow(256, 3);
		var bps = data[0x22];
        var numChannels = data[0x16];
        
        console.log("SR: " + sampleRate + ", BPS " + bps + ", NC " + numChannels);

        udpPort.send({
        address: "/info",
        args: [
            {
                type: "i",
                value: sampleRate
            },
            {
                type: "i",
                value: bps
            },
            {
                type: "i",
                value: numChannels
            }
        ]
    }, hostAddress, hostPort);
    console.log("File Info Send");
    
    var bytesToSend = "";
    for (var i = 0x2C; i < data.length; i+= 512) {
        bytesToSend = "";
        for (var j = i; (j < i + 512) && (data[j] != undefined); j++) {
            if (data[j] < 16) {
                bytesToSend += "0";
            }
            //var byte = data[j];
            bytesToSend += data[j].toString(16);
        }
        udpPort.send({address: "/data", args: [{type : "s", value: bytesToSend}]}, hostAddress, hostPort);
        console.log("File Data Send");
    }
    });
   // udpPort.send({address: "/end", args: [{type : "i", value: 1}]}, hostAddress, hostPort);
   
}


// Create an osc.js UDP Port listening on port 9999.
var udpPort = new osc.UDPPort({
    localAddress: "192.168.43.126",
    localPort: 9999,
    metadata: true
});

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Waiting for OSC messages--------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Listen for incoming OSC messages.
udpPort.on("message", function (oscMessage, info) {
    console.log("An OSC message just arrived. ", oscMessage);
    if (oscMessage.address == "/test") {
        console.log(oscMessage.address);
        console.log("Type: " + oscMessage.args[0].type);
        console.log("value: " + oscMessage.args[0].value)

    }
    else if (oscMessage.address == "/getFile") {
        console.log(oscMessage.address);
        console.log("value: " + oscMessage.args[0].value);
        console.log(wavFiles);
        var ind = wavFiles.indexOf(oscMessage.args[0].value);
            console.log("index " + ind);
        if (ind != -1)
         getFileData(oscMessage.args[0].value);
    }
    else if (oscMessage.address == "/getList") {
        getFileList();
    }
});

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Server Startup------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
 
// Open the socket.
udpPort.open();
 
 
// When the port is ready, send an OSC message
udpPort.on("ready", function () {
    console.log("sending hello message");
    udpPort.send({
        address: "/hello",
        args: [
            {
                type: "s",
                value: "hello"
            },
            {
                type: "i",
                value: 0xff10ff
            }
        ]
    }, hostAddress, hostPort);
});
