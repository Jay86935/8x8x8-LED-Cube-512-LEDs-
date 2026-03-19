# 8×8×8 LED Cube (512 LEDs)

This project implements an **8×8×8 LED Cube consisting of 512 LEDs** controlled using an **Arduino Uno and 74HC595 shift registers**. The cube creates dynamic 3D lighting patterns and animations through multiplexing techniques and custom firmware.

The system demonstrates concepts of **digital electronics, shift-register control, and 3D LED multiplexing**.

---

## Features

* 512 LEDs arranged in a **3D 8×8×8 cube structure**
* Controlled using **74HC595 shift registers**
* Custom **3D lighting animations and patterns**
* Efficient **multiplexing technique** for controlling multiple LEDs with limited microcontroller pins
* Stable brightness through optimized refresh timing

---

## Hardware Components

* Arduino Uno
* 512 Blue LEDs
* 74HC595 Shift Registers
* Resistors
* Transistors (for layer control)
* Power supply
* Custom soldered LED cube structure

---

## Working Principle

The LED cube is organized into **8 layers with 64 LEDs per layer**.

Instead of controlling all LEDs individually, the system uses **multiplexing**:

1. The **74HC595 shift registers** control the columns of the cube.
2. Each layer is activated one at a time using transistor switching.
3. The Arduino rapidly cycles through the layers while updating column data.
4. Due to **persistence of vision**, the human eye perceives the cube as fully illuminated with complex 3D patterns.

Custom Arduino code generates different animations by controlling which LEDs turn ON or OFF in each refresh cycle.

---

## Applications

* Interactive LED displays
* Electronics and embedded systems learning
* Art installations and visual effects
* Demonstration of multiplexing and shift-register control

---

## Technologies Used

* Arduino (C/C++)
* 74HC595 Shift Register Interface
* LED Multiplexing
* Embedded Systems Programming

---

## Project Demo

Images and videos of the LED cube animations.

![LED Cube](images/led_cube.jpg)

---

## Repository Structure

8x8x8-LED-Cube
│
├── code
│   └── led_cube.ino
│
├── circuit
│   └── schematic.png
│
├── images
│   └── led_cube.jpg
│
└── README.md
