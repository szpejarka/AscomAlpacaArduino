#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>



const char* ssid = "rock-1";
const char* password = "grudka1972";
// Set your Static IP address
IPAddress local_IP(192, 168, 55, 200);
// Set your Gateway IP address
IPAddress gateway(192, 168, 55, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
WebServer server(80);

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
/*
hw_timer_t *timer2 = NULL;
hw_timer_t *stopper2 = NULL;
int step2_tick = 1;
void IRAM_ATTR timer2Interrupt() {
 digitalWrite(MOTOR2_STEP, step2_tick);
 step2_tick = (step2_tick + 1) % 2;
}
void IRAM_ATTR stopper2Interrupt() {
   timerStop(timer2);
   digitalWrite(led, 0);  
}
enum MotorNo {M1,M2};
class Motor
{


  int MOTOR_STEP = 33;
  int MOTOR_DIR = 32;
  int MOTOR_EN = 25;

  int STEP_PER_1MHZ = 23373;

  int *step_tick;
  int speed = 12800;
  int dir = 0;
  MotorNo no;

  hw_timer_t **timer = NULL;
  hw_timer_t **stopper = NULL;

  public:
  Motor(MotorNo ano,int pinStep, int pinDir, int pinEn, hw_timer_t **atimer, hw_timer_t **astopper, int *astep_tick)
  {
    no = ano;
    MOTOR_STEP = pinStep;
    MOTOR_DIR = pinDir;
    MOTOR_EN = pinEn;
    timer = atimer;
    stopper = astopper;
    step_tick = astep_tick;
  }
  */
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
    Serial.printf("Started time %s",server.arg("time"));       
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
/*
  private:
  void IRAM_ATTR timerInterrupt() {
  digitalWrite(MOTOR_STEP, step_tick);
  step_tick = (step_tick + 1) % 2;
  }
  void IRAM_ATTR stopper1Interrupt() {
    timerStop(*timer);
    digitalWrite(led, 0);   
  }  


}


Motor Motor1 = new(MOTOR1_STEP, MOTOR1_DIR, MOTOR1_EN, &timer1, &stopper1, &step1_tick);
Motor Motor2 = new(MOTOR2_STEP, MOTOR2_DIR, MOTOR2_EN, &timer2, &stopper2, &step2_tick);

Motor Motors[2];

Motors[0] = Motor1;
Motors[1] = Motor2;
*/

void handleRoot(String action) {
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

String handleRest() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
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
  void handleNotFound() {
  String message = handleRest();
  handleRoot(message);
  digitalWrite(led, 0);
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

void alpacainit()
{
  server.on("/api/v1/telescope/0/interfaceversion", []() {

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
  server.on("/api/v1/telescope/0/connected", []() {
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
  server.on("/api/v1/telescope/0/cansync", []() {
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
  server.on("/api/v1/telescope/0/cansettracking", []() {
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
  server.on("/api/v1/telescope/0/canslew", []() {
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
  server.on("/api/v1/telescope/0/canslewasync", []() {
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
  server.on("/api/v1/telescope/0/canmoveaxis", []() {
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
  server.on("/api/v1/telescope/0/declination", []() {
      handleRest();

      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": 0.0 \
        }");
  
    });       
  server.on("/api/v1/telescope/0/rightascension", []() {
      handleRest();

      server.send(200, "application/json", 
        "{ \
          \"ClientTransactionID\": 0, \
          \"ServerTransactionID\": 0, \
          \"ErrorNumber\": 0, \
          \"ErrorMessage\": \"\", \
          \"Value\": 0.0 \
        }");
  
    }); 
  server.on("/api/v1/telescope/0/tracking", []() {
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

  server.on("/", []() {
      handleRoot("Ready");
  });

  server.on("/inline", []() {
    handleRoot("this works as well");
  });
  server.on("/start", []() {
    handleStart(STEP_PER_1MHZ);
    handleRoot("Started");
  });
  server.on("/stop", []() {
    handleStop();
    handleRoot("Stopped");
  });
  server.on("/starth", []() {
    handleStart(STEP_PER_1MHZ / 500);
    handleRoot("Started HighSpeed");
  });
  server.on("/dir", []() {
    dir = (dir + 1) % 2;   
    digitalWrite(MOTOR_DIR,dir);
    Serial.print("Change dir");
    handleRoot("changed dir");
  });
  server.on("/form", []() {
    String timestr = server.arg("time");
    double time = timestr.toDouble();

    timerStart(stopper);
    timerAlarm(stopper, time * 1000, false, 0 ); // Interrupt every 1 second
    handleStart(STEP_PER_1MHZ);
    Serial.printf("Started time %s",server.arg("time"));    

    String msg = String("Set time ")+timestr;
    handleRoot(msg);
  });

  alpacainit();

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


  // timer = timerBegin(1000000); // Timer 0, clock divisor 80

  // timerAttachInterrupt(timer, &timerInterrupt); // Attach the interrupt handling function
 
  // stopper = timerBegin(1000000); 
  // timerStop(stopper);
  // timerAttachInterrupt(stopper, &stopperInterrupt); // Attach the interrupt handling function

  // analogWriteFrequency(MOTOR_STEP, speed);
  // analogWriteResolution(MOTOR_STEP,4);
  // analogWrite(MOTOR_STEP,8);
}


void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
  //digitalWrite(MOTOR_STEP,step_tick);

  // step_tick = !step_tick;
}
