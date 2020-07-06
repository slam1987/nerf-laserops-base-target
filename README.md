# nerf-laserops-base-target
Nerf LaserOps target written in C++ for Arduino microcontroller

Description:

The purpose of the project is to create a "team base" or target practice device for Nerf's LaserOps laser tag system.
Since there is nothing on the market for this, I have decided to create a prototype using the Arduino microcontroller.

The code uses libraries from https://github.com/cyborg5/IRLib2 for decoding raw infrared signals.

Usage:

The device will have preset "hit points" to determine how many shots it can take before being "destroyed".
