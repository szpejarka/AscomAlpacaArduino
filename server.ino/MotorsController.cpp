#include "MotorsController.h"
#include <cmath>
#include <NTPClient.h>

double MotorsController::ra = 0;                       //From 0 to 360 deg -> tracking axis
double MotorsController::dec = 90;                     // From -90 deg (south pool) to 90 deg (north pool)
const double MotorsController::longitude = 15.75;      // Hardcoded longitude


MotorsController::MotorsController(unsigned long epochTime) {
    int year = 1970 + epochTime / 31556926;
    int month = 1 + (epochTime % 31556926) / 2629743;
    int day = 1 + ((epochTime % 31556926) % 2629743) / 86400;
    double hour = (epochTime % 86400) / 3600.0;

    ra = calculateLST(year, month, day, hour, longitude);

    Serial.print("Right Ascension (RA) set to: ");
    Serial.print(getRA(), 6);
    Serial.println(" hours");
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