#include "RestService.h"
#include <iostream>
#include <string>

RestService::RestService(const int port, MotorsController& motorsCtrl) :
    server(port), motorsController(motorsCtrl) {}

const int led = 2;

void RestService::alpacainit() {

  server.on("/api/v1/telescope/0/interfaceversion", [this]() {
    handleRest();
    server.send(200, "application/json", 
      "{ \
        \"ClientTransactionID\": 0, \
        \"ServerTransactionID\": 0, \
        \"ErrorNumber\": 0, \
        \"ErrorMessage\": \"\", \
        \"Value\": 0 \
      }");
  });


  server.on("/api/v1/telescope/0/connected", [this]() {
      handleRest();
      if(server.method() == HTTP_PUT)
      {
        server.send(200, "application/json", 
          "{ \
            \"ClientTransactionID\": 0, \
            \"ServerTransactionID\": 0, \
            \"ErrorNumber\": 0, \
            \"ErrorMessage\": \"\", \
          }");
      }
      else {
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
      }
    });


  server.on("/api/v1/telescope/0/cansync", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
    });  


  server.on("/api/v1/telescope/0/cansettracking", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
    });


  server.on("/api/v1/telescope/0/canslew", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
    });       


  server.on("/api/v1/telescope/0/canslewasync", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
    });      


  server.on("/api/v1/telescope/0/canmoveaxis", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
    });      


  server.on("/api/v1/telescope/0/declination", [this]() {
      handleRest();
      float decValue = motorsController.getDEC();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": " + String(decValue, 6) + 
        "}");
    });    


  server.on("/api/v1/telescope/0/rightascension", [this]() {
      handleRest();
      float raValue = motorsController.getRA();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": " + String(raValue, 6) +
        "}");
  
    }); 


  server.on("/api/v1/telescope/0/tracking", [this]() {
      handleRest();
      bool tracking = false;
      if(server.method() == HTTP_PUT){
          for (uint8_t i = 0; i < server.args(); i++){
              if(server.argName(i) == "Tracking"){
                  tracking = (server.arg(i) == "True");
              }
          }
          if(tracking) { motorsController.startTracking(); }
          else { motorsController.stopTracking(); }
          server.send(200, "application/json", 
            "{ \
                \"ClientTransactionID\": 0, \
                \"ServerTransactionID\": 0, \
                \"ErrorNumber\": 0, \
                \"ErrorMessage\": \"\" \
            }");
      }
      else {
          bool isTracking = motorsController.isTracking();
          String value = "";
          if(isTracking){ value = "true"; }
          else { value = "false"; }
          server.send(200, "application/json", 
            "{ \
              \"ClientTransactionID\": 0, \
              \"ServerTransactionID\": 0, \
              \"ErrorNumber\": 0, \
              \"ErrorMessage\": \"\", \
              \"Value\":" + value + 
            "}");
      }
    });


    server.on("/api/v1/telescope/0/slewtocoordinatesasync", [this]() {
      handleRest();

      double rightAscension;// = jsonDoc["RightAscension"];
      double declination;// = jsonDoc["Declination"];

      for (uint8_t i = 0; i < server.args(); i++) {
          if(server.argName(i) == "RightAscension"){
              rightAscension = server.arg(i).toDouble();
          }
          else if(server.argName(i) == "Declination"){
              declination = server.arg(i).toDouble();
          }
      }
      motorsController.slewToCoordinates(declination, rightAscension);

      server.send(200, "application/json", 
        "{ \
            \"ClientTransactionID\": 0, \
            \"ServerTransactionID\": 0, \
            \"ErrorNumber\": 0, \
            \"ErrorMessage\": \"\" \
        }");
    });

    server.on("/api/v1/telescope/0/slewing", [this]() {
      handleRest();
      bool isSlewing = motorsController.isSlewing();
      String value = "";
      if(isSlewing){ value = "true"; }
      else { value = "false"; }
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\":" + value + 
        "}");
    });


    server.on("/api/v1/telescope/0/abortslew", [this]() {
      handleRest();
      motorsController.abortSlewing();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": true \
        }");
      Serial.println("Aborted slewing");
    });

    server.on("/api/v1/telescope/0/trackingrates", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": [0] \
        }");
    });

    server.on("/api/v1/telescope/0/trackingrate", [this]() {
      handleRest();
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": 0 \
        }");
    });      




    server.onNotFound([this]() { handleNotFound(); });
    server.begin();
    Serial.println("HTTP server started");
}


void RestService::handleClient(){
  server.handleClient();
}


String RestService::handleRest() {
    digitalWrite(led, 1);
    String message = "\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    //Serial.print(message);
    return message;
}


void RestService::handleNotFound() {
    //Serial.print("\n\nFile Not Found");
    String message = handleRest();
    handleRoot(message);
    digitalWrite(led, 0);
}

void RestService::handleRoot(String action) {
  digitalWrite(led, 1);
  server.send(200, "text/html", "<html><body><form action='form'><p>Action=<b>"+action+"</b>"
  +"<p><label for='motor'>Motor</label><input type='radio' name='motor' id='motor' value='0'><input type='radio' name='motor' id='motor' value='1'>"
  +"<p><a href='/start'>Start</a>"
  +"<p><a href='/stop'>STOP</a>"
  +"<p><a href='/starth'>START_HI_SPEED</a>"
  +"<p><a href='/dir'>Change dir</a>"
  +"<p><label for='time'>Time (s)</label><input type='text' name='time' id='time'>"
  +"<input type='submit' value='Start timed'>"
  +"</form></body></html>");
}