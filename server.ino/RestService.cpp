#include "RestService.h"
#include <ArduinoJson.h>

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
      if(server.method() == HTTP_POST)
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
          \"Value\": false \
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
      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": false \
        }");
    });


    server.on("/api/v1/telescope/0/slewtocoordinates", [this]() {
      handleRest();
      if (server.hasArg("plain")) {
        String requestBody = server.arg("plain");
        StaticJsonDocument<200> jsonDoc;
        DeserializationError error = deserializeJson(jsonDoc, requestBody);
        if (error) {
            server.send(400, "application/json", "{\"Error\": \"Invalid JSON format\"}");
            return;
        }
        double rightAscension = jsonDoc["RightAscension"];
        double declination = jsonDoc["Declination"];

        Serial.print("RightAscension: ");
        Serial.println(rightAscension);
        Serial.print("Declination: ");
        Serial.println(declination);

        server.send(200, "application/json", 
          "{ \
              \"ClientTransactionID\": 0, \
              \"ServerTransactionID\": 0, \
              \"ErrorNumber\": 0, \
              \"ErrorMessage\": \"\" \
          }");
    } else {
        server.send(400, "application/json", "{\"Error\": \"No JSON body provided\"}");
    }
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
    Serial.print(message);
    return message;
}


void RestService::handleNotFound() {
    Serial.print("\n\nFile Not Found");
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