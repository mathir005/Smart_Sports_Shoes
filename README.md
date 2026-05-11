# ESP32 Smart Sports Shoes

An IoT-enabled smart shoe system built with ESP32 that tracks steps, calculates health metrics (calories and fat burned), and features a safety "Danger Light" for night running.

---

## 🚀 Features
* **Step Tracking:** Uses a Piezoelectric sensor to detect physical steps.
* **Health Metrics:** Real-time calculation of Calories (kcal) and Fat (grams) burned.
* **Web Dashboard:** Mobile-responsive interface to view stats, control sessions, and export data.
* **Safety Light:** Configurable blinking "Danger Light" for visibility during night activities.
* **Data Export:** Download your activity report as a CSV file directly from the web interface.
* **Persistence:** All configurations (thresholds, goals, pin assignments) are saved to the ESP32's NVS (Non-Volatile Storage).

---

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32 (Any standard DevKit)
* **Sensor:** Piezoelectric Sensor
* **Output:** LED (Danger Light)
* **Connectivity:** Built-in WiFi (Access Point mode)

### 🔌 Connection Diagram
| Component | ESP32 Pin (GPIO) | Description |
| :--- | :--- | :--- |
| **Piezo Sensor (+)** | **GPIO 34** | Analog input to detect foot strikes |
| **Piezo Sensor (-)** | **GND** | Ground connection |
| **LED (+)** | **GPIO 2** | Danger light output |
| **LED (-)** | **GND** | Ground (use a 220Ω resistor) |

---

## 💻 Software Setup
1. Install the **Arduino IDE**.
2. Install the **ESP32 Board Manager**.
3. Install the following libraries via the Library Manager:
    * `ArduinoJson`
4. Open `SmartSportsShoes.ino` and upload it to your ESP32.

---

## 📖 User Manual

### 1. Initial Connection
* Power on the ESP32.
* Scan for WiFi networks on your phone/PC and connect to: **"ESP32_SmartSportsShoes"**.
* Use the password: **12345678**.
* Open your browser and go to: `http://192.168.4.1`.

### 2. Calibrating the Sensor
* Go to the **Configuration** tab.
* Adjust the **Threshold Value** (Default: 100). If it misses steps, lower it; if it double-counts, increase it.
* Adjust the **Step Delay** to match your running pace (Default: 500ms).
* Click **Save Configuration** to store settings permanently.

### 3. Tracking Your Run
* On the **Home** tab, click **Start** to begin a session.
* The dashboard will update every second with your step count, calories, and fat burned.
* Toggle the **Danger Light** switch if you are running at night.
* Click **Stop** when finished to see your session summary.

### 4. Data Management
* Use the **Export Report (CSV)** button to download your activity history for your records.
* Use **Reset Counter** to clear total steps for a new day.

---

## 👥 Contributors
* Mathiyarasu R
* Mellina S
* Karoline Mary
* Ezhil Arasan
