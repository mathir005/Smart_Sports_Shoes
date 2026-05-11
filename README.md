# ESP32 Smart Sports Shoes

An IoT-enabled smart shoe system built with ESP32 that tracks steps, calculates health metrics (calories and fat burned), and features a safety "Danger Light" for night running.

## 🚀 Features
* [cite_start]**Step Tracking:** Uses a Piezoelectric sensor to detect physical steps[cite: 3, 12, 26].
* [cite_start]**Health Metrics:** Real-time calculation of Calories (kcal) and Fat (grams) burned[cite: 5, 6, 40].
* [cite_start]**Web Dashboard:** Mobile-responsive interface to view stats, start/stop sessions, and export data[cite: 16, 58, 126].
* [cite_start]**Safety Light:** Configurable blinking "Danger Light" for visibility during night activities[cite: 8, 11, 240].
* [cite_start]**Data Export:** Download your activity report as a CSV file directly from the web interface[cite: 42, 56, 166].
* [cite_start]**Persistence:** All configurations (thresholds, goals, pin assignments) are saved to the ESP32's NVS (Non-Volatile Storage)[cite: 1, 13, 24].

## 🛠️ Hardware Requirements
* [cite_start]**Microcontroller:** ESP32 (Any standard DevKit) [cite: 1, 202]
* [cite_start]**Sensor:** Piezoelectric Sensor (Default Pin: GPIO 34) [cite: 3, 19]
* [cite_start]**Output:** LED for Danger Light (Default Pin: GPIO 2) [cite: 7, 20]
* [cite_start]**Connectivity:** Built-in WiFi (Access Point mode) [cite: 2, 15]

## 💻 Software Setup
1. Install the **Arduino IDE**.
2. Install the **ESP32 Board Manager**.
3. Install the following libraries via the Library Manager:
    * [cite_start]`ArduinoJson` [cite: 1]
4. Open `SmartSportsShoes.ino` and upload it to your ESP32.

## 📱 Usage
1. Power on the ESP32.
2. [cite_start]Connect your phone/PC to the WiFi network: **"ESP32_SmartSportsShoes"** (Password: **12345678**)[cite: 2].
3. [cite_start]Open a web browser and go to `http://192.168.4.1`[cite: 15, 16].
4. [cite_start]Use the **Home** tab to track activity and the **Configuration** tab to calibrate the sensor[cite: 126, 198].

## 👥 Contributors
* Mathiyarasu R [cite: 56]
* [cite_start]Mellina S [cite: 56]
* [cite_start]Karoline Mary [cite: 56]
* Ezhil Arasan [cite: 56]
