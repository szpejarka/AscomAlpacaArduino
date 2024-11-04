#ifndef RESTSERVICE_H
#define RESTSERVICE_H

#include <WebServer.h>
#include "MotorsController.h"

class RestService {
  public:
    RestService(const int port,  MotorsController& motorsController);
    void alpacainit();
    void handleClient();

  private:
    WebServer server;
    MotorsController& motorsController; 
    String handleRest();
    void handleNotFound();
    void handleRoot(String action);
};

#endif