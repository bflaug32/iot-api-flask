 if( APIclient.connect(APIserver, 80) && (( (millis() - APIcallTimer ) > APIcallWaitTime ) || firstRunFlag)){  
    
    APIcallTimer = millis();  //reset APIcallTimer
    
    delay(500);

    //Send request to resource
    APIclient.print("GET ");
    APIclient.print(APIresource);
    APIclient.println(" HTTP/1.1");
    APIclient.print("Host: ");
    APIclient.println(APIserver);
    APIclient.println("Connection: close");
    APIclient.println();

    delay(100);

    //API Data Sucessfully Recieved
    Serial.println("API Request Sent");

    //Set Time limit for API.client to wait for find() to be successful
    APIclient.setTimeout(APIcallWaitTime);

    //if find() is successful, return 'true'
    //if find() is unsuccessful within setTimeout() ms, return false
    bool validResponse = APIclient.find("\r\n\r\n");

    if(!validResponse){
      Serial.println("API Response Invalid or Timed Out");
    }

    //if APIclient got a valid response, continue with parsing response
    if(validResponse){
          
          //API Data Sucessfully Recieved
          Serial.println("Valid API Data Received");
          
              APIclient.readBytes(APIresponse, API_RESPONSE_ARRAY_SIZE);
              Serial.print("APIresponse= ");
              Serial.println(APIresponse);
      
              if(int(APIresponse[0]) != 91){   //"91" is ASCII code for open bracket "["
                  Serial.println("Wrong start char detected");
                  int index = 0;
                  bool beginFound = false;
                  while(!beginFound){
                      if(int(APIresponse[index]) == 91){ // check for the "[" 
                      beginFound = true;
                      Serial.print("'[' found at ");
                      Serial.println(index);
                      }
                      index++;
                  }
              
                  int eol = sizeof(APIresponse);
                  Serial.print("Length = ");
                  Serial.println(eol);
                  
                  //restructure by shifting the correct data
                  Serial.println("Restructure API Response...");
                  for(int c=0; c<(eol-index); c++){
                      APIresponse[c] = APIresponse[((c+index)-1)];
                      Serial.print(APIresponse[c]);
                  }
              
                  Serial.println("Restructuring Complete.");      
              }
    }
    //End of API Response Processing
        
    APIclient.stop(); // disconnect from server      
}
