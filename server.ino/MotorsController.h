#ifndef MOTORSCONTROLLLER_H
#define MOTORSCONTROLLLER_H

class MotorsController {
  public:
    MotorsController(unsigned long epochTime);
    void setRA(double raValueH);
    void setDEC(double decValueDeg);
    double getRA();
    double getDEC();
    void slewToCoordinates(double decValueDeg, double raValueH);
    bool isSlewing();
    bool abortSlewing();

  private:
    static const double longitude;

    double calculateJulianDate(int year, int month, int day, double ut);
    double calculateGST(double JD);
    double calculateLST(int year, int month, int day, double ut, double longitude);
    void motorsetupRA();
    void motorsetupDEC();
};

#endif