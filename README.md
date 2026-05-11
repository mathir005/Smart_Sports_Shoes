# ESP32 Smart Sports Shoes

An IoT-enabled smart shoe system built with ESP32 that tracks steps, calculates health metrics (calories and fat burned), and features a safety "Danger Light" for night running.

## 🚀 Features
* **Step Tracking:** Uses a Piezoelectric sensor to detect physical steps.
* **Health Metrics:** Real-time calculation of Calories (kcal) and Fat (grams) burned.
* **Web Dashboard:** Mobile-responsive interface to view stats, start/stop sessions, and export data.
* **Safety Light:** Configurable blinking "Danger Light" for visibility during night activities.
* **Data Export:** Download your activity report as a CSV file directly from the web interface.
* **Persistence:** All configurations (thresholds, goals, pin assignments) are saved to the ESP32's NVS (Non-Volatile Storage).

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32 (Any standard DevKit)
* **Sensor:** Piezoelectric Sensor (Default Pin: GPIO 34)
* **Output:** LED for Danger Light (Default Pin: GPIO 2)
* **Connectivity:** Built-in WiFi (Access Point mode)

## 💻 Software Setup
1. Install the **Arduino IDE**.
2. Install the **ESP32 Board Manager**.
3. Install the following libraries via the Library Manager:
    * `ArduinoJson`
4. Open `SmartSportsShoes.ino` and upload it to your ESP32.

## 📱 Usage
1. Power on the ESP32.
2. Connect your phone/PC to the WiFi network: **"ESP32_SmartSportsShoes"** (Password: **12345678**).
3. Open a web browser and go to `http://192.168.4.1`.
4. Use the **Home** tab to track activity and the **Configuration** tab to calibrate the sensor.

## 👥 Contributors
* Mathiyarasu R
* Mellina S
* Karoline Mary
* Ezhil Arasan
