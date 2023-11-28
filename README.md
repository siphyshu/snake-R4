# 🐍🕹️ snake-R4

A Snake game in Arduino UNO R4's LED matrix with a joystick controller

https://github.com/siphyshu/snake-R4/assets/52672162/3939b7a4-3024-4b81-9c07-6673afcf4b6d

## Requirements

1. An [Arduino UNO R4 WIFI](https://store.arduino.cc/products/uno-r4-wifi)
2. A [Dual-axis joystick controller](https://robu.in/product/joystick-module-ps2-breakout-sensor/)
3. 5 connecting wires

## Setup

Connect Arduino R4's 5V, GND, A0, A1, and D2 to the PS2 Joystick module's 5V, GND, URX, URY, and SW respectively.

<img src="https://github.com/siphyshu/snake-R4/assets/52672162/23e4b67c-cdf1-4ca2-9919-640c709266d0" width="600">

## Notes and TODO

These are my ideas for the project, PRs are welcomed if anybody wants to work on these.

* This can most likely be repurposed for any arduino connected with an LED Matrix, will have to see about that though
* See how a blinking head on the snake looks. Does it provide clearer visuals of the snake's direction?
* Adding an infinite AI playing mode could be fun and can be used as a screensaver playing in the background
