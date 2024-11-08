# ESP32-Based Control System for Equatorial Astrophotography Mount with Alpaca REST API

## Project Overview

This project develops a versatile control system for an equatorial astrophotography mount using an **ESP32 microcontroller**.
The system aims to provide precise tracking and control over Right Ascension (RA) and Declination (Dec) axes via stepper motors.
With remote communication capabilities enabled by Wi-Fi, the system allows seamless integration with popular astronomical software such as [N.I.N.A.](https://nighttime-imaging.eu/) and with them supports advanced functions like Slew with Plate Solving and Three-Point Polar Alignment.
Additionally, a GPS module is used to accurately determine location data, aligning the coordinate system for precise celestial tracking.

Leveraging the Alpaca REST API, the system ensures compatibility with both modern and legacy applications through bridging services such as [Device Hub](https://ascom-standards.org/FAQs/DevHub.htm), which translates Alpaca API calls to the ASCOM standard.
This setup allows users to operate the mount using older Windows applications that rely on the ASCOM protocol.

## RA and Dec Axes Control

The equatorial mount design relies on two primary axes:

- **Right Ascension (RA)**: Controls the east-west movement of the mount, enabling the tracking of celestial objects across the sky as the Earth rotates.
- **Declination (Dec)**: Controls the north-south movement, allowing adjustment of the telescope’s vertical position relative to the celestial equator.

Microstepping for both RA and Dec axes is achieved through the use of stepper motors driven by microstepping drivers.
The ESP32 controller manages these microsteps via low level **interrupt routines**, ensuring accurate and timely control for each step.
This level of control is essential for long-exposure astrophotography, where even minor tracking inaccuracies can lead to blurring in the final image.

## Current Implementation of Alpaca REST API

The Alpaca REST API serves as the primary communication layer for this system, facilitating remote control and interaction with astronomical software. Key features include:

- **Asynchronous Slewing**: Slewing commands are executed asynchronously, allowing real-time adjustments without blocking other operations.
- **Sidereal-Speed Tracking**: Supports continuous tracking of celestial objects at sidereal speed, ensuring precise alignment with the Earth's rotation.
- **Syncing Coordinates (EQ)**: Allows synchronization of the mount’s coordinates with the telescope’s actual position in equatorial mode.
- **Parking**: Supports a park function, enabling the mount to return to a predefined safe position when not in use, protecting equipment and ensuring easy restarts.

For more details on the Alpaca API, refer to the [Alpaca REST API specification](https://ascom-standards.org/api/).
Please note that not all endpoints are currently implemented;

## Planned Enhancements

In future iterations, additional endpoints and functionalities will be introduced, including:

- **GuidePulse Support**
