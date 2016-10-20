var http = require('http');
var server = http.createServer(function(req, res){
  
  console.log("Request Made!")

}).listen(3000); // uses http to create a server for us

console.log("Server Running...")