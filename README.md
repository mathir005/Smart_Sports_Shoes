# 🏃 Smart Sports Shoe Activity Tracker

![StepPulse](https://img.shields.io/badge/StepPulse-v2.0-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32-orange)
![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![License](https://img.shields.io/badge/License-MIT-brightgreen)
![Build](https://img.shields.io/badge/Arduino-IDE-ready-red)

A **smart sports shoe activity tracker** powered by ESP32 that counts your steps using a piezo vibration sensor, tracks calories & fat burned, and displays everything on a sleek **live web dashboard** — all without any external app or cloud dependency!

---

## 📌 Table of Contents

- [🌟 Features](#-features)
- [🏗️ Architecture](#-architecture)
- [🛠️ Hardware Requirements](#-hardware-requirements)
- [⚡ Quick Start](#-quick-start)
- [📊 Dashboard Overview](#-dashboard-overview)
- [⚙️ Configuration](#-configuration)
- [🌐 Web API Reference](#-web-api-reference)
- [📂 Project Structure](#-project-structure)
- [👤 Authors](#-authors)
- [📝 License](#-license)

---

## 🌟 Features

| Feature | Description |
|---------|-------------|
| **Piezo Step Detection** | Counts steps using a piezo vibration sensor attached to the shoe — every footstep is captured |
| **Live Web Dashboard** | Real-time stats accessible from any device via WiFi — no app needed |
| **Calorie & Fat Tracking** | Calculates calories burned and fat loss based on configurable per-step metrics |
| **Daily Goal Progress** | Set a daily step goal and track progress with a live progress bar |
| **Danger Light** | Flashing LED safety light for night running — toggle on/off from the dashboard |
| **CSV Export** | One-click export of full activity report with session stats, totals, and hardware config |
| **Persistent Config** | All settings saved to ESP32 flash memory — survives power cycles |
| **Configurable Thresholds** | Adjust piezo sensitivity, step delay, and calculation parameters via web UI |
| **Zero Dependencies** | No cloud services, no apps, no accounts — pure local IoT |
| **Mobile Friendly** | Responsive web UI works on phones, tablets, and desktops |

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────┐
│              ESP32 (in shoe)                │
│                                             │
│  ┌──────────┐    ┌──────────────────────┐   │
│  │  Piezo   │───▶│  Step Detection      │   │
│  │  Sensor  │    │  (ADC threshold)     │   │
│  └──────────┘    └──────────┬───────────┘   │
│                             │               │
│  ┌──────────┐    ┌──────────▼───────────┐   │
│  │  LED     │◀───│  Danger Light        │   │
│  │ (GPIO2)  │    │  (blink toggle)      │   │
│  └──────────┘    └──────────────────────┘   │
│                                             │
│  ┌──────────────────────────────────────┐   │
│  │          WiFi Access Point           │   │
│  │     ESP32_SmartSportsShoes:12345678  │   │
│  └───────────────┬──────────────────────┘   │
└───────────────────┼─────────────────────────┘
                    │ HTTP / JSON
                    ▼
        ┌───────────────────────────┐
        │   Web Browser Dashboard   │
        │   http://192.168.4.1      │
        │                           │
        │  ┌─ Step Count (live)     │
        │  ├─ Calories Burned       │
        │  ├─ Fat Burned (grams)    │
        │  ├─ Daily Goal Progress % │
        │  ├─ Session Stats         │
        │  ├─ Start/Stop Tracking   │
        │  ├─ Danger Light Toggle   │
        │  ├─ CSV Export            │
        │  └─ Configuration Editor  │
        └───────────────────────────┘
```

---

## 🛠️ Hardware Requirements

| Component | GPIO Pin | Notes |
|-----------|----------|-------|
| **ESP32 Dev Module** | — | Any ESP32 with ADC support |
| **Piezo Disc Sensor** | GPIO34 (default) | Analog output, detects vibration |
| **LED (Danger Light)** | GPIO2 (default) | Standard LED or LED strip |
| **Resistor (10kΩ)** | — | For piezo voltage divider (if needed) |
| **Battery** | — | LiPo or USB power |

### Wiring Diagram

```
ESP32                  Components
──────                 ──────────
GPIO34 (ADC) ────────► Piezo Sensor (+)
GND          ────────► Piezo Sensor (-)
GPIO2        ────────► LED Anode (+)
                      LED Cathode (-) ──► GND (via resistor)
3.3V         ────────► (optional pull-up for piezo)
```

> **Note:** The piezo sensor may require a voltage divider circuit (10kΩ + 20kΩ resistors) to bring the analog voltage within the ESP32's 0–3.3V ADC range.

---

## ⚡ Quick Start

### Step 1: Open in Arduino IDE

1. Open `SmartSportsShoes.ino` in Arduino IDE
2. Select board: **ESP32 Dev Module**
3. Select the correct COM port

### Step 2: Upload Firmware

```
Sketch → Upload
```

### Step 3: Connect to WiFi

1. On your phone/laptop, scan for WiFi networks
2. Connect to: **`ESP32_SmartSportsShoes`**
3. Password: **`12345678`**
4. Open browser: **http://192.168.4.1**

### Step 4: Start Tracking!

1. Mount the piezo sensor inside your shoe (sole area)
2. Click **Start** on the dashboard
3. Walk or run — steps are counted in real-time
4. View calories, fat burned, and progress toward your goal

---

## 📊 Dashboard Overview

### Main Page (/)

| Section | Description |
|---------|-------------|
| **Status Indicator** | Shows Running/Stopped with animated icon |
| **Total Steps** | Large display of cumulative step count |
| **Progress Bar** | Visual progress toward daily goal (0–100%) |
| **Session Steps** | Steps in current active session |
| **Calories Burned** | Estimated kcal for current session |
| **Fat Burned** | Estimated grams of fat burned in session |
| **Danger Light** | Toggle switch to enable/disable blinking LED |
| **Controls** | Start / Stop / Reset / Export CSV buttons |

### Configuration Page (/config)

| Setting | Default | Range | Description |
|---------|---------|-------|-------------|
| Piezo Sensor Pin | GPIO34 | 0–39 | ADC pin for vibration sensor |
| Threshold Value | 100 | 1–4095 | Minimum ADC value to register a step |
| Step Delay | 500ms | 100–5000ms | Cooldown between steps (prevents double-count) |
| Daily Goal | 10,000 | 100–100,000 | Target steps per day |
| Calories/Step | 0.04 | 0.01–1.0 | Energy burned per step (kcal) |
| Fat/Calorie | 0.00013 | 0.0001–0.001 | Fat burned per kcal (kg) |
| LED Pin | GPIO2 | 0–39 | GPIO for danger light |
| LED On Delay | 200ms | 50–5000ms | Blink ON duration |
| LED Off Delay | 800ms | 50–5000ms | Blink OFF duration |

> 📝 All settings are **auto-saved to ESP32 flash** and persist after power-off.

---

## 🌐 Web API Reference

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Main dashboard HTML |
| `/config` | GET | Configuration page HTML |
| `/stats` | GET | JSON: all runtime stats & config |
| `/start` | GET | Start step tracking |
| `/stop` | GET | Stop step tracking |
| `/reset` | GET | Reset step counters |
| `/danger-light-on` | GET | Enable danger LED blinking |
| `/danger-light-off` | GET | Disable danger LED |
| `/export-csv` | GET | Download CSV activity report |
| `/save-config` | POST | Save configuration to flash |

### JSON Response Format (`/stats`)

```json
{
  "stepCount": 4523,
  "sessionSteps": 1234,
  "isRunning": true,
  "piezoPin": 34,
  "thresholdValue": 100,
  "stepDelay": 500,
  "dailyGoal": 10000,
  "caloriesPerStep": 0.04,
  "fatPerCalorie": 0.00013,
  "ledPin": 2,
  "ledOnDelay": 200,
  "ledOffDelay": 800,
  "dangerLightEnabled": false
}
```

---

## 📂 Project Structure

```
SmartSportsShoes+2/
└── SmartSportsShoes.ino    # Single-file ESP32 firmware (1295 lines)
    ├── WiFi Access Point    # Soft AP mode
    ├── Web Server           # HTTP server on port 80
    ├── Step Detection       # Piezo ADC reading + threshold
    ├── Dashboard UI         # HTML/CSS/JS (embedded in firmware)
    ├── Configuration        # Persistent via Preferences (SPIFFS)
    ├── Danger Light         # LED blink control
    └── CSV Export           # On-demand activity report
```

---

## 💡 Design Decisions

### Why Piezo Sensor?
- **Non-invasive** — no modification to the shoe structure
- **Self-powered** — generates voltage from mechanical stress
- **Cheap & reliable** — simple threshold-based detection
- **Lightweight** — adds minimal weight to the shoe

### Why WiFi Access Point?
- **No router dependency** — works anywhere, even outdoors
- **Zero setup** — connects directly from any device
- **Privacy-first** — all data stays local, no cloud involved
- **Instant access** — dashboard loads in under 1 second

### Why Single-File Firmware?
- **Easy to flash** — just open and upload in Arduino IDE
- **Portability** — copy the `.ino` to any ESP32
- **Maintenance** — all logic in one place, easy to debug

---

## 🔧 Calibration Tips

1. **Threshold Tuning:** Walk normally and observe the Serial Monitor (115200 baud). Adjust `thresholdValue` so steps register reliably without false triggers.
2. **Step Delay:** If steps are double-counted, increase `stepDelay`. If fast steps are missed, decrease it.
3. **Piezo Placement:** Mount at the **heel** or **ball of the foot** for maximum vibration detection.
4. **Calories/Fat:** Adjust `caloriesPerStep` and `fatPerCalorie` based on user weight and stride analysis.

---

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| Can't find WiFi network | Check ESP32 power supply; monitor Serial output |
| Steps not counting | Lower threshold value or check piezo wiring |
| Steps double-counted | Increase step delay (try 600–800ms) |
| Dashboard not loading | Re-flash firmware; ensure SPIFFS is initialized |
| LED not blinking | Verify LED polarity and GPIO pin connection |
| CSV export fails | Try a different browser; check serial for errors |

---

## 👤 Authors

| Name | Role |
|------|------|
| **Mathiyarasu R** | Lead Developer, Hardware Integration |
| **Mellina S** | Web UI Design, Dashboard Development |
| **Karoline Mary** | Testing, Calibration, Documentation |
| **Ezhil Arasan** | Project Coordination, Data Analysis |

---

## 📝 License

**MIT License** — Free for personal, educational, and commercial use.

---

## ⭐ If You Like This Project

Please consider giving it a ⭐ on GitHub! For bugs, feature requests, or collaboration, open an issue on the repository.

---

> **StepPulse** — *Every step counts. Smartly.*
