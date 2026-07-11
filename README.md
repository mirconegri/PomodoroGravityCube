# 🍅 Pomodoro Gravity Cube

[![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

A buttonless Arduino-based Pomodoro timer housed in a handcrafted wooden shell. The device reads its own orientation via an accelerometer — flipping it to a different face starts a different countdown. No buttons, no touchscreen, no app required.

Built as an alternative to software timers that compete for screen real estate and attention. A physical object on a desk, with no screen of its own, is a more effective focus anchor than yet another browser tab or phone notification.

## Table of Contents

- [Preview](#preview)
- [Live Simulation](#live-simulation)
- [Features](#features)
- [Tech Stack and Hardware](#tech-stack-and-hardware)
- [Design Decisions](#design-decisions)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Configuration and Environment](#configuration-and-environment)
- [Contributing](#contributing)
- [License](#license)

## Preview

| 3D Model Overview | Panels and Exploded View |
|:---:|:---:|
| <img src="assets/side.png" alt="3D Overview" width="400"/> | <img src="assets/front.png" alt="Exploded View" width="400"/> |
| **Inner Core** | **Top-Down Layout** |
| <img src="assets/back.png" alt="Inner Core" width="400"/> | <img src="assets/up.png" alt="Top View" width="400"/> |

### Cube States

| State | Trigger | LED |
|:---|:---|:---:|
| Timer Active | Face 1/2/3/4 pointing up — sets 5, 15, 25, or 45 min | Green |
| Warning | 30 seconds remaining | Blinking Yellow |
| Time's Up | Countdown reached 00:00 | Blinking Red |
| Paused | Screen face pointing straight up (Z-axis) | Blue |
| Idle | No face detected yet | Off |

| Timer Active | Warning | Time's Up | Paused | Idle |
|:---:|:---:|:---:|:---:|:---:|
| ![Green](assets/verde.png) | ![Yellow](assets/giallo.png) | ![Red](assets/rosso.png) | ![Blue](assets/blu.png) | ![Off](assets/spento.png) |

## Live Simulation

No hardware required to test the firmware. Run the exact sketch in your browser via Wokwi:

**[Run the Virtual Prototype on Wokwi](https://wokwi.com/projects/466522637641408513)**

In the simulation: click the switch to power on, then click the blue MPU6050 sensor block to change the X/Y/Z axis values and observe the OLED and LED react in real time.

## Features

- Buttonless operation — physical orientation is the only input, read directly from the MPU6050 accelerometer
- Four preset countdowns mapped to faces: **5, 15, 25, and 45 minutes**
- Pause face (Z-axis up) freezes the countdown without resetting — flip back to resume exactly where it left off
- OLED content rotates automatically to match the orientation of the active face
- RGB LED feedback: green (running), blinking yellow (last 30s), blinking red (done), blue (paused), off (idle)
- Delta-time countdown via `millis()` — accurate regardless of main loop execution speed

## Tech Stack and Hardware

**Firmware**

- Language: C++ (Arduino framework, `.ino`)
- Libraries: `Wire.h`, `Adafruit_MPU6050`, `Adafruit_Sensor`, `Adafruit_GFX`, `Adafruit_SSD1306`

**Hardware**

| Component | Spec |
|---|---|
| Microcontroller | Arduino Nano / Pro Micro |
| Motion Sensor | MPU6050 — 6-axis accelerometer and gyroscope, I2C |
| Display | SSD1306 OLED, 128×64, I2C |
| Visual Feedback | RGB LED (Common Cathode) + 3× 330Ω resistors |
| Power Management | TP4056 LiPo charging module |
| Power Source | 3.7V LiPo battery |
| Enclosure | Custom handcrafted wooden shell |

## Design Decisions

**Why no buttons?** Physical buttons require deliberate action — you press them because you decided to. Orientation-based input is lower friction for mode switching and naturally maps to a cube's faces in a way that's instantly learnable without documentation. In practice, flipping the cube to start a new session creates a small physical ritual that reinforces the focus intent.

**Why `millis()` instead of a blocking `delay()`?** `delay()` halts execution, which would make the device unresponsive to orientation changes mid-countdown. The delta-time pattern (`millis()` minus last-tick) allows the main loop to keep polling the accelerometer every cycle while still tracking elapsed time accurately.

**Why an acceleration threshold of `7.0`?** This value was determined empirically during testing on the specific enclosure. The cube needs to be held fairly steady to register a face — incidental tilts while moving it across a desk don't trigger a timer switch. If you rebuild this with a different enclosure or mount the sensor differently, you may need to retune this constant.

## Project Structure

```
PomodoroGravityCube/
├── src/
│   └── cubo_pomodoro.ino   # Firmware — orientation, timer logic, OLED and LED output
├── assets/
│   ├── back.png
│   ├── blu.png
│   ├── front.png
│   ├── giallo.png
│   ├── rosso.png
│   ├── side.png
│   ├── spento.png
│   ├── up.png
│   └── verde.png
├── README.md
└── LICENSE
```

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (1.8+ or 2.x) or PlatformIO
- Hardware components listed above, wired to the microcontroller
- The following libraries installed via the Arduino Library Manager:
  - `Adafruit MPU6050`
  - `Adafruit Unified Sensor`
  - `Adafruit GFX Library`
  - `Adafruit SSD1306`

### Installation

```bash
git clone https://github.com/mirconegri/PomodoroGravityCube.git
cd PomodoroGravityCube
```

Wire the MPU6050 and SSD1306 to the microcontroller's I2C bus (SDA/SCL). Wire the RGB LED to three PWM-capable digital pins through 330Ω resistors. Refer to the diagrams in `assets/` for the breadboard layout.

Open `src/cubo_pomodoro.ino` in the Arduino IDE, select your board and port, and upload.

## Usage

Once powered on:

- **Flip to Face 1, 2, 3, or 4** → starts a 5, 15, 25, or 45-minute countdown
- **Flip to Z-axis up (screen face up)** → pauses the current timer without losing progress
- **Flip back to the same face** → resumes from where it paused
- **Flip to a different face mid-session** → starts a fresh countdown for the new duration
- **When the display shows "FINITO!"** → flip to any face to reset and start a new session

## Configuration and Environment

No environment variables or configuration files. All tunable values are constants in `src/cubo_pomodoro.ino`:

| Constant | Current Value | Purpose |
|---|---|---|
| `pinRosso`, `pinVerde`, `pinBlu` | `5`, `6`, `9` | RGB LED output pins |
| Per-face durations | 5 / 15 / 25 / 45 min | Set in the `switch (facciaAttuale)` block |
| Acceleration threshold | `7.0` | Sensitivity for face detection — tune if mounting differs |

## Contributing

Contributions welcome — firmware improvements, alternative enclosure designs, or wiring variants:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Commit your changes with a clear message
4. Open a Pull Request

For bugs or ideas, open an [Issue](https://github.com/mirconegri/PomodoroGravityCube/issues).

### Author

**Mirco Negri** — Computer Science @ UniTrento

[![Portfolio](https://img.shields.io/badge/Portfolio-00599C?style=for-the-badge&logo=globe&logoColor=white)](https://mirconegri.github.io/Portfolio/)
[![GitHub](https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white)](https://github.com/mirconegri)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/mirco-negri-263810225)
[![Gmail](https://img.shields.io/badge/Gmail-D14836?style=for-the-badge&logo=gmail&logoColor=white)](mailto:mirconegri06@gmail.com)

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
