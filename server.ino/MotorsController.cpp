#include "MotorsController.h"
#include <cmath>
#include <NTPClient.h>
#include <Arduino.h>  // Required for Arduino functions and macros
#include <driver/timer.h> // ESP32 specific timer functions

double ra = 0;                       //From 0 to 360 deg -> tracking axis
double dec = 90;                     // From -90 deg (south pool) to 90 deg (north pool)
const double MotorsController::longitude = 15.75;      // Hardcoded longitude

const int STEPS_PER_DEGREE = 10240;
const double DEGREE_PER_ONE_STEP = 0.000097656;
const int TUS_1_DEG_PER_SECOND = 98;

// Motor RA
const int MOTOR_STEP_RA = 22;
const int MOTOR_DIR_RA = 23;
const int MOTOR_EN_RA = 21;
int step_tick_RA = 0;
hw_timer_t *timerRA = NULL;
int cyclesToDoRA = 0;
int ra_slew_dir = -1;
bool ra_is_slewing = false;

// Motor DEC
const int MOTOR_STEP_DEC = 33;
const int MOTOR_DIR_DEC = 32;
const int MOTOR_EN_DEC = 25;
int step_tick_DEC = 0;
hw_timer_t *timerDEC = NULL;
int cyclesToDoDEC = 0;
int dec_slew_dir = -1;
bool dec_is_slewing = false;

// Tracking RA
int step_tick_TRACK = 0;
hw_timer_t *timerTRACK = NULL;
bool isTRACK = false;
const int TUS_TRACKING = 233;//23375;

void handleStopDEC() {
    digitalWrite(MOTOR_EN_DEC, 1); 
    timerStop(timerDEC);
}

void handleStopRA() {
    digitalWrite(MOTOR_EN_RA, 1);  
    timerStop(timerRA);
}

void handleStopTRACK() {
    digitalWrite(MOTOR_EN_RA, 1); 
    isTRACK = false;
    timerStop(timerTRACK);
    //Serial.println("Tracking stopped"); 
}

void IRAM_ATTR timerInterruptRA() {
    digitalWrite(MOTOR_STEP_RA, step_tick_RA);
    ra = ra + (DEGREE_PER_ONE_STEP * ra_slew_dir);
    if(ra > 360){ ra = ra - 360; }
    else if(ra < 0){ ra = ra + 360; }
    cyclesToDoRA--;
    step_tick_RA = (step_tick_RA + 1) % 2;
    if(cyclesToDoRA <= 0){
        ra_is_slewing = false;
        handleStopRA();
    }
}

void IRAM_ATTR timerInterruptTRACK() {
    digitalWrite(MOTOR_STEP_RA, step_tick_TRACK);
    step_tick_TRACK = (step_tick_TRACK + 1) % 2;
}


void IRAM_ATTR timerInterruptDEC() {
    digitalWrite(MOTOR_STEP_DEC, step_tick_DEC);
    dec = dec + (DEGREE_PER_ONE_STEP * dec_slew_dir);
    cyclesToDoDEC--;
    step_tick_DEC = (step_tick_DEC + 1) % 2;
    if(cyclesToDoDEC <= 0){
        dec_is_slewing = false;
        handleStopDEC();
    }
}


void handleStartDEC(int Tus, int cycles) {
    digitalWrite(MOTOR_EN_DEC, 0);
    cyclesToDoDEC = cycles;
    //timerAlarmEnable(timerDEC);
    timerStart(timerDEC);
    timerAlarm(timerDEC, Tus, true, 0);
    Serial.println("DEC axis started");
}


void handleStartRA(int Tus, int cycles) {
    digitalWrite(MOTOR_EN_RA, 0);
    cyclesToDoRA = cycles;
    //timerAlarmEnable(timerRA);
    timerStart(timerRA);
    timerAlarm(timerRA, Tus, true, 0);
    Serial.println("RA axis started");
}

void handleStartTrack(){
  digitalWrite(MOTOR_EN_RA, 0);
  digitalWrite(MOTOR_DIR_RA, 0);
  timerStart(timerTRACK);
  timerAlarm(timerTRACK, TUS_TRACKING, true, 0);
  isTRACK = true;
}

void MotorsController::motorsetupRA(){
    pinMode(MOTOR_STEP_RA, OUTPUT);
    pinMode(MOTOR_DIR_RA, OUTPUT);
    pinMode(MOTOR_EN_RA, OUTPUT);
    digitalWrite(MOTOR_EN_RA, 1);  

    timerRA = timerBegin(1000000); // Timer 0, clock divisor 80
    timerAttachInterrupt(timerRA, timerInterruptRA); // Attach the interrupt handling function

    timerTRACK = timerBegin(1000000);
    timerAttachInterrupt(timerTRACK, timerInterruptTRACK);
  }

void MotorsController::motorsetupDEC(){
    pinMode(MOTOR_STEP_DEC, OUTPUT);
    pinMode(MOTOR_DIR_DEC, OUTPUT);
    pinMode(MOTOR_EN_DEC, OUTPUT);
    digitalWrite(MOTOR_EN_DEC, 1);    

    timerDEC = timerBegin(1000000); // Timer 0, clock divisor 80
    timerAttachInterrupt(timerDEC, timerInterruptDEC); // Attach the interrupt handling function
  }

MotorsController::MotorsController(unsigned long epochTime) {
    int year = 1970 + epochTime / 31556926;
    int month = 1 + (epochTime % 31556926) / 2629743;
    int day = 1 + ((epochTime % 31556926) % 2629743) / 86400;
    double hour = (epochTime % 86400) / 3600.0;

    Serial.print("Year : ");
    Serial.println(year);
    Serial.print("Month : ");
    Serial.println(month);
    Serial.print("Day : ");
    Serial.println(day);
    Serial.print("Hour : ");
    Serial.println(hour);

    ra = calculateLST(year, month, day, hour, longitude);

    Serial.print("Right Ascension (RA) set to: ");
    Serial.print(getRA(), 6);
    Serial.println(" hours");

    motorsetupDEC();
    motorsetupRA();
    
}


void MotorsController::setRA(double raValueH) {
    ra = raValueH*15;
}


void MotorsController::setDEC(double decValueDeg) {
    dec = decValueDeg;
}


double MotorsController::getRA() {
    return ra/15;
}

double MotorsController::getDEC() {
    return dec;
}

bool MotorsController::isSlewing(){
    return (ra_is_slewing || dec_is_slewing);
}

bool MotorsController::abortSlewing(){
    handleStopRA();
    handleStopDEC();
    ra_is_slewing = false;
    dec_is_slewing = false;
    cyclesToDoDEC = 0;
    cyclesToDoRA = 0;
    return true;
}


  void MotorsController::startTracking(){
      Serial.println("Tracking started");
      handleStartTrack();
  }

  void MotorsController::stopTracking(){
    Serial.println("Tracking stopped");
      handleStopTRACK();
  }

  bool MotorsController::isTracking(){
      return isTRACK;
  }


void MotorsController::slewToCoordinates(double decValueDeg, double raValueH){
  //setRA(raValueH);
  //setDEC(decValueDeg);

  double raValueDec = raValueH*15;
  double deltaRA = raValueDec - ra;
  if(deltaRA > 180){ deltaRA = deltaRA - 360; }
  else if (deltaRA < -180){ deltaRA = deltaRA + 360; }

  double deltaDEC = decValueDeg - dec;
  if(deltaDEC > 90){ deltaDEC = deltaDEC - 180; }
  else if(deltaDEC < -90){ deltaDEC = deltaDEC + 180; }

  if (deltaRA > 0) { digitalWrite(MOTOR_DIR_RA, 1); }
  else { digitalWrite(MOTOR_DIR_RA, 0); }

  if (deltaDEC > 0) { digitalWrite(MOTOR_DIR_DEC, 1); }
  else { digitalWrite(MOTOR_DIR_DEC, 0); }

  int stepsDEC = static_cast<int>(std::abs(deltaDEC * 10240));
  int stepsRA = static_cast<int>(std::abs(deltaRA * 10240));

  if(deltaDEC > 0){ dec_slew_dir = 1; }
  else{ dec_slew_dir = -1; }

  if(deltaRA > 0){ ra_slew_dir = 1; }
  else{ ra_slew_dir = -1; }
  

  Serial.print("Delta DEC ");
  Serial.println(deltaDEC);
  Serial.print("Steps DEC ");
  Serial.println(stepsDEC);
  Serial.print("Delta RA ");
  Serial.println(deltaRA);
  Serial.print("Steps RA ");
  Serial.println(stepsRA);

  
  if(stepsDEC > 0){
      handleStartDEC(TUS_1_DEG_PER_SECOND/2, stepsDEC);
      Serial.println("Slewing DEC axis started");
      dec_is_slewing = true;
  }
  if(stepsRA > 0){
      handleStartRA(TUS_1_DEG_PER_SECOND/2, stepsRA);
      Serial.println("Slewing RA axis started");
      ra_is_slewing = true;
  }
}

double MotorsController::calculateJulianDate(int year, int month, int day, double ut) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    double A = std::floor(year / 100.0);
    double B = 2 - A + std::floor(A / 4.0);
    double JD = std::floor(365.25 * (year + 4716)) + std::floor(30.6001 * (month + 1)) + day + B - 1524.5 + (ut / 24.0);
    return JD;
}

double MotorsController::calculateGST(double JD) {
    double JC = (JD - 2451545.0) / 36525.0;
    double GMST = 280.46061837 + 360.98564736629 * (JD - 2451545.0) + 0.000387933 * JC * JC - (JC * JC * JC) / 38710000.0;
    GMST = fmod(GMST, 360.0);
    if (GMST < 0) {
        GMST += 360.0;
    }
    return GMST;
}

double MotorsController::calculateLST(int year, int month, int day, double ut, double longitude) {
    double JD = calculateJulianDate(year, month, day, ut);
    double GST = calculateGST(JD);
    double LST = GST + longitude;
    LST = fmod(LST, 360.0);  // Wrap LST to 0-360 degrees
    if (LST < 0) {
        LST += 360.0;
    }
    return LST;  // Convert LST to hours
}