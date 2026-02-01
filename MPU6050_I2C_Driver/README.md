# MPU6050 Linux I2C Device Driver – BeagleBone Black

This repository contains the source code for a **custom Linux kernel I2C device driver** developed for the **MPU6050 6-axis Motion Tracking Sensor** (Accelerometer + Gyroscope + Temperature).

The driver was developed and tested on a **BeagleBone Black** based on the **TI Sitara AM3358BZCZ100** processor.

All source files related to the **kernel driver, user-space application, and Device Tree overlay** are available in this repository.

---

## Driver Overview

This driver is implemented as a **loadable kernel module** and integrates with the **Linux I2C subsystem**. It communicates with the MPU6050 over the **I2C-2 bus** and exposes sensor data to user space through a **character device interface**.

The driver performs:

- Device detection and verification  
- Sensor initialization and configuration  
- Multi-byte sensor data acquisition  
- Kernel-to-user data transfer  

---

## Hardware Platform

| Component | Details |
|----------|---------|
| Board | BeagleBone Black |
| Processor | TI Sitara AM3358BZCZ100 |
| Sensor | MPU6050 (Accelerometer + Gyroscope + Temperature) |
| Interface | I2C (I2C-2 Bus) |

---

## Software Environment

| Component | Version |
|----------|---------|
| OS | Debian GNU/Linux 11 (Bullseye) |
| Kernel | Linux 5.10.168-ti-r71 |
| Driver Type | Loadable Kernel Module |
| Language | C |

---

## I2C Driver Details

### Device Matching

The driver supports both legacy and Device Tree based systems:

- **I2C Device ID Table** for traditional matching  
- **OF Device ID Table** with compatible string:


