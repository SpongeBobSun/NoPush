/// <reference path="./typings/node/node.d.ts" />

var net = require('net');
var server = net.createServer();
var sockets = new Array();

server.on('connection',function(socket){  
  console.log('on connection');
  sockets.push(socket);
  socket.setEncoding('utf-8');
  socket.on('error',function(err){
    console.log(err);
  });
  socket.on('data',function(data){
    console.log('on data:' + data);
  });
});
// server.listen(22333);

var sender = function(content){
  sockets.forEach(function(value,index,array){
    value.write(content);
  });
};

module.exports.PushServer = server;
module.exports.MsgSender = sender;