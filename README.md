# nerf-laserops-base-target
Nerf LaserOps target written in C++ for Arduino microcontroller

Description:

The purpose of the project is to create a "team base" or target practice device for Nerf's LaserOps laser tag system.
Since there is nothing on the market for this, I have decided to create a prototype using the Arduino microcontroller.

The code uses libraries from https://github.com/cyborg5/IRLib2 for decoding raw infrared signals.

Features:
- Support for both the ALPHAPOINT and DELTABURST systems.
- Preset "hit points" to determine how many shots the device can take before being "destroyed".
- Reset button can be used to reset hit points once the device is in the "destroyed" state.
- Supports multiple teams using the "Team Switch" button.
