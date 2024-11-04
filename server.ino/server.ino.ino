#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include "RestService.h"
#include "MotorsController.h"
#include <NTPClient.h>



const char* ssid = "rock-1";
const char* password = "grudka1972";
// Set your Static IP address
IPAddress local_IP(192, 168, 55, 200);
// Set your Gateway IP address
IPAddress gateway(192, 168, 55, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
RestService* restService;

const int led = 2;

// Motor 1
const int MOTOR_STEP = 22;
const int MOTOR_DIR = 23;
const int MOTOR_EN = 21;

// Motor 2 32,33,25
// const int MOTOR_STEP = 33;
// const int MOTOR_DIR = 32;
// const int MOTOR_EN = 25;

const int STEP_PER_1MHZ = 23373;


hw_timer_t *timer = NULL;
hw_timer_t *stopper = NULL;
int step_tick = 1;
int speed = 12800;
int dir = 0;
void IRAM_ATTR timerInterrupt() {
 digitalWrite(MOTOR_STEP, step_tick);
 step_tick = (step_tick + 1) % 2;
}
void IRAM_ATTR stopperInterrupt() {
   timerStop(timer);
   digitalWrite(led, 0);  
}

  void handleStop()
  {
      digitalWrite(MOTOR_EN, 1); 
      Serial.printf("Stop movement");  
      timerStop(timer);
      digitalWrite(led, 0);  
  }
  void handleStart(int Tus)
  {
      digitalWrite(MOTOR_EN, 0);
      timerStart(timer);
      timerAlarm(timer, Tus, true, 0 ); // Interrupt every 1 second
      digitalWrite(led, 1);  
      Serial.printf("Started speed movement");
  }  

  void handleTimes(int time)
  {
    timerStart(stopper);
    timerAlarm(stopper, time * 1000, false, 0 ); // Interrupt every 1 second
    handleStart(STEP_PER_1MHZ);
    //Serial.printf("Started time %s",server.arg("time"));       
  }
  
  void motorsetup()
  {
    pinMode(MOTOR_STEP, OUTPUT);
    pinMode(MOTOR_DIR, OUTPUT);
    pinMode(MOTOR_EN, OUTPUT);
    digitalWrite(MOTOR_EN, 1);
    digitalWrite(MOTOR_DIR,dir);    

    timer = timerBegin(1000000); // Timer 0, clock divisor 80
 
    timerAttachInterrupt(timer, timerInterrupt); // Attach the interrupt handling function
  
    stopper = timerBegin(1000000); 
    timerStop(stopper);
    timerAttachInterrupt(stopper, stopperInterrupt); // Attach the interrupt handling function    
  }


void handleStop(int motor)
{
    digitalWrite(MOTOR_EN, 1); 
    Serial.print("Stop movement");  
    timerStop(timer);
    digitalWrite(led, 0);  
}
void handleStart(int motor, int Tus)
{
    digitalWrite(MOTOR_EN, 0);
    timerStart(timer);
    timerAlarm(timer, Tus, true, 0 ); // Interrupt every 1 second
    digitalWrite(led, 1);  
    Serial.print("Started speed movement");
}


void setup(void) {
  pinMode(led, OUTPUT);
  motorsetup();

  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
  {
    Serial.println("STA Failed to configure");
  }

  Serial.printf("MAC: %s\n", WiFi.macAddress());

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("telescope.local")) {
    Serial.println("MDNS responder started");
  }

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
  timeClient.begin();
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  Serial.print("Got epochTime from NTPClient: ");
  Serial.println(epochTime, 6);

  MotorsController motorsController(epochTime);
  restService = new RestService(80, motorsController);
  restService->alpacainit();
}


void loop(void) {
  restService->handleClient();
  delay(1);//allow the cpu to switch to other tasks
}
