#ifndef MOTORSCONTROLLLER_H
#define MOTORSCONTROLLLER_H

class MotorsController {
  public:
    MotorsController(unsigned long epochTime);
    static void setRA(double raValueH);
    static void setDEC(double decValueDeg);
    static double getRA();
    static double getDEC();

  private:
    static double ra;
    static double dec;
    static const double longitude;

    double calculateJulianDate(int year, int month, int day, double ut);
    double calculateGST(double JD);
    double calculateLST(int year, int month, int day, double ut, double longitude);
};

#endif