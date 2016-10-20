var express = require('express'); // import the express module
var app = express(); // sets up our express app with the express method

app.get('/', function(request, response){
  response.send('Hello, Jon!')
});
app.listen(3000, function(){
  console.log(`App is running on port 3000`);
});
