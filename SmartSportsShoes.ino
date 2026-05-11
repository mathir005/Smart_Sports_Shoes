#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// Preferences object for storing configuration
Preferences preferences;

// Web server on port 80
WebServer server(80);

// WiFi AP credentials
const char* ssid = "ESP32_SmartSportsShoes";
const char* password = "12345678";

// Configuration variables (with defaults)
int piezoPin = 34;           // ADC pin for piezo sensor
int thresholdValue = 100;    // Threshold to detect a step
int stepDelay = 500;         // Delay between steps in milliseconds
int dailyGoal = 10000;       // Daily step goal
float caloriesPerStep = 0.04; // Calories burned per step
float fatPerCalorie = 0.00013; // Fat burned per calorie (in kg)

// Danger Light Configuration
int ledPin = 2;              // GPIO pin for danger light LED
int ledOnDelay = 200;        // LED on time in milliseconds
int ledOffDelay = 800;       // LED off time in milliseconds
bool dangerLightEnabled = false; // Danger light state

// Runtime variables
int stepCount = 0;
unsigned long lastStepTime = 0;
bool isRunning = false;
int sessionSteps = 0;
unsigned long sessionStartTime = 0;

// Danger light runtime variables
unsigned long lastLedToggle = 0;
bool ledState = false;

// Function prototypes
void loadConfiguration();
void saveConfiguration();
void handleRoot();
void handleConfig();
void handleSaveConfig();
void handleGetStats();
void handleStart();
void handleStop();
void handleReset();
void handleDangerLightOn();
void handleDangerLightOff();
void handleExportCSV();
void checkPiezoSensor();
void updateDangerLight();
String getFormattedDateTime();
String getSessionDuration();

void setup() {
  Serial.begin(115200);
  
  // Initialize preferences
  preferences.begin("smart-shoes", false);
  
  // Load saved configuration
  loadConfiguration();
  
  // Setup piezo sensor pin
  pinMode(piezoPin, INPUT);
  
  // Setup LED pin for danger light
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Setup WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/save-config", HTTP_POST, handleSaveConfig);
  server.on("/stats", handleGetStats);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/reset", handleReset);
  server.on("/danger-light-on", handleDangerLightOn);
  server.on("/danger-light-off", handleDangerLightOff);
  server.on("/export-csv", handleExportCSV);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  // Check for steps only if running
  if (isRunning) {
    checkPiezoSensor();
  }
  
  // Update danger light blinking
  updateDangerLight();
}

void loadConfiguration() {
  piezoPin = preferences.getInt("piezoPin", 34);
  thresholdValue = preferences.getInt("threshold", 100);
  stepDelay = preferences.getInt("stepDelay", 500);
  dailyGoal = preferences.getInt("dailyGoal", 10000);
  caloriesPerStep = preferences.getFloat("calPerStep", 0.04);
  fatPerCalorie = preferences.getFloat("fatPerCal", 0.00013);
  
  // Load danger light configuration
  ledPin = preferences.getInt("ledPin", 2);
  ledOnDelay = preferences.getInt("ledOnDelay", 200);
  ledOffDelay = preferences.getInt("ledOffDelay", 800);
  
  Serial.println("Configuration loaded:");
  Serial.printf("Piezo Pin: %d\n", piezoPin);
  Serial.printf("Threshold: %d\n", thresholdValue);
  Serial.printf("Step Delay: %d ms\n", stepDelay);
  Serial.printf("Daily Goal: %d steps\n", dailyGoal);
  Serial.printf("LED Pin: %d\n", ledPin);
  Serial.printf("LED On Delay: %d ms\n", ledOnDelay);
  Serial.printf("LED Off Delay: %d ms\n", ledOffDelay);
}

void saveConfiguration() {
  preferences.putInt("piezoPin", piezoPin);
  preferences.putInt("threshold", thresholdValue);
  preferences.putInt("stepDelay", stepDelay);
  preferences.putInt("dailyGoal", dailyGoal);
  preferences.putFloat("calPerStep", caloriesPerStep);
  preferences.putFloat("fatPerCal", fatPerCalorie);
  
  // Save danger light configuration
  preferences.putInt("ledPin", ledPin);
  preferences.putInt("ledOnDelay", ledOnDelay);
  preferences.putInt("ledOffDelay", ledOffDelay);
  
  Serial.println("Configuration saved!");
}

void checkPiezoSensor() {
  int sensorValue = analogRead(piezoPin);
  unsigned long currentTime = millis();
  
  // Check if sensor exceeds threshold and enough time has passed since last step
  if (sensorValue > thresholdValue && 
      (currentTime - lastStepTime) > stepDelay) {
    stepCount += 2;       // Increment by 2 per physical step detected
    sessionSteps += 2;    // Increment by 2 per physical step detected
    lastStepTime = currentTime;
    
    Serial.printf("Step detected! Total: %d, Session: %d, Sensor: %d\n", 
                  stepCount, sessionSteps, sensorValue);
  }
}

String getFormattedDateTime() {
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  return String(days) + "d " + String(hours % 24) + "h " + 
         String(minutes % 60) + "m " + String(seconds % 60) + "s";
}

String getSessionDuration() {
  if (sessionStartTime == 0) {
    return "0m 0s";
  }
  
  unsigned long sessionDuration = millis() - sessionStartTime;
  unsigned long seconds = sessionDuration / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  if (hours > 0) {
    return String(hours) + "h " + String(minutes % 60) + "m " + String(seconds % 60) + "s";
  } else if (minutes > 0) {
    return String(minutes) + "m " + String(seconds % 60) + "s";
  } else {
    return String(seconds) + "s";
  }
}

void handleExportCSV() {
  // Calculate statistics
  float calories = sessionSteps * caloriesPerStep;
  float fatBurned = calories * fatPerCalorie * 1000; // in grams
  float progressPercent = (float)stepCount / dailyGoal * 100;
  String duration = getSessionDuration();
  String uptime = getFormattedDateTime();
  
  // Build CSV content
  String csv = "Smart Sports Shoes Activity Report\n";
  csv += "Generated: " + uptime + " (uptime)\n";
  csv += "\n";
  csv += "Category,Parameter,Value,Unit\n";
  csv += "Session Statistics,Session Steps," + String(sessionSteps) + ",steps\n";
  csv += "Session Statistics,Session Duration," + duration + ",\n";
  csv += "Session Statistics,Calories Burned," + String(calories, 2) + ",kcal\n";
  csv += "Session Statistics,Fat Burned," + String(fatBurned, 2) + ",grams\n";
  csv += "Session Statistics,Status," + String(isRunning ? "Running" : "Stopped") + ",\n";
  csv += "\n";
  csv += "Total Statistics,Total Steps," + String(stepCount) + ",steps\n";
  csv += "Total Statistics,Daily Goal," + String(dailyGoal) + ",steps\n";
  csv += "Total Statistics,Progress," + String(progressPercent, 1) + ",%\n";
  csv += "Total Statistics,Goal Status," + String(stepCount >= dailyGoal ? "Achieved" : "In Progress") + ",\n";
  csv += "\n";
  csv += "Hardware Configuration,Piezo Pin,GPIO " + String(piezoPin) + ",\n";
  csv += "Hardware Configuration,Threshold Value," + String(thresholdValue) + ",ADC units\n";
  csv += "Hardware Configuration,Step Delay," + String(stepDelay) + ",ms\n";
  csv += "Hardware Configuration,LED Pin,GPIO " + String(ledPin) + ",\n";
  csv += "Hardware Configuration,LED On Delay," + String(ledOnDelay) + ",ms\n";
  csv += "Hardware Configuration,LED Off Delay," + String(ledOffDelay) + ",ms\n";
  csv += "\n";
  csv += "Calculation Settings,Calories Per Step," + String(caloriesPerStep, 3) + ",kcal\n";
  csv += "Calculation Settings,Fat Per Calorie," + String(fatPerCalorie, 5) + ",kg\n";
  csv += "\n";
  csv += "Danger Light,Status," + String(dangerLightEnabled ? "ON (Blinking)" : "OFF") + ",\n";
  csv += "\n";
  csv += "Project Information,Created By,Mathiyarasu R; Mellina S; Karoline Mary; Ezhil Arasan,\n";
  csv += "Project Information,Device,ESP32 Smart Sports Shoes,\n";
  
  // Send CSV file
  server.sendHeader("Content-Disposition", "attachment; filename=smart_sports_shoes_report.csv");
  server.send(200, "text/csv", csv);
  
  Serial.println("CSV report exported!");
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Sports Shoes - Home</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 32px;
            margin-bottom: 10px;
        }
        
        .nav {
            display: flex;
            background: #f8f9fa;
            border-bottom: 2px solid #e9ecef;
        }
        
        .nav a {
            flex: 1;
            padding: 15px;
            text-align: center;
            text-decoration: none;
            font-size: 16px;
            font-weight: 600;
            color: #6c757d;
            transition: all 0.3s;
            border-bottom: 3px solid transparent;
        }
        
        .nav a.active {
            background: white;
            color: #667eea;
            border-bottom: 3px solid #667eea;
        }
        
        .nav a:hover {
            background: #e9ecef;
        }
        
        .content {
            padding: 30px;
        }
        
        .stat-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border-radius: 15px;
            padding: 30px;
            margin-bottom: 20px;
            text-align: center;
            color: white;
        }
        
        .stat-label {
            font-size: 14px;
            margin-bottom: 10px;
            text-transform: uppercase;
            letter-spacing: 1px;
            opacity: 0.9;
        }
        
        .stat-value {
            font-size: 56px;
            font-weight: bold;
            margin-bottom: 5px;
        }
        
        .stat-unit {
            font-size: 18px;
            opacity: 0.8;
        }
        
        .progress-section {
            background: #f8f9fa;
            border-radius: 15px;
            padding: 25px;
            margin-bottom: 20px;
        }
        
        .progress-header {
            display: flex;
            justify-content: space-between;
            margin-bottom: 15px;
        }
        
        .progress-label {
            font-size: 16px;
            font-weight: 600;
            color: #495057;
        }
        
        .progress-percentage {
            font-size: 16px;
            font-weight: bold;
            color: #667eea;
        }
        
        .progress-container {
            background: #e9ecef;
            border-radius: 10px;
            height: 30px;
            overflow: hidden;
        }
        
        .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            transition: width 0.5s ease;
            display: flex;
            align-items: center;
            justify-content: flex-end;
            padding-right: 10px;
            color: white;
            font-weight: bold;
            font-size: 14px;
        }
        
        .controls {
            display: flex;
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .btn {
            flex: 1;
            padding: 18px 30px;
            border: none;
            border-radius: 10px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .btn-start {
            background: #28a745;
            color: white;
        }
        
        .btn-start:hover:not(:disabled) {
            background: #218838;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(40, 167, 69, 0.3);
        }
        
        .btn-stop {
            background: #dc3545;
            color: white;
        }
        
        .btn-stop:hover:not(:disabled) {
            background: #c82333;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(220, 53, 69, 0.3);
        }
        
        .btn-reset {
            background: #6c757d;
            color: white;
            width: 100%;
        }
        
        .btn-reset:hover {
            background: #5a6268;
            transform: translateY(-2px);
        }
        
        .btn-export {
            background: #17a2b8;
            color: white;
            width: 100%;
            margin-bottom: 20px;
        }
        
        .btn-export:hover {
            background: #138496;
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(23, 162, 184, 0.3);
        }
        
        .btn:disabled {
            opacity: 0.5;
            cursor: not-allowed;
        }
        
        .status {
            text-align: center;
            padding: 15px;
            border-radius: 10px;
            margin-bottom: 20px;
            font-weight: bold;
            font-size: 16px;
        }
        
        .status.running {
            background: #d4edda;
            color: #155724;
            border: 2px solid #28a745;
        }
        
        .status.stopped {
            background: #f8d7da;
            color: #721c24;
            border: 2px solid #dc3545;
        }
        
        .session-stats {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .mini-stat {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
            border: 2px solid #e9ecef;
        }
        
        .mini-stat-label {
            font-size: 12px;
            color: #6c757d;
            margin-bottom: 8px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        .mini-stat-value {
            font-size: 28px;
            font-weight: bold;
            color: #667eea;
        }
        
        .mini-stat-unit {
            font-size: 14px;
            color: #6c757d;
            margin-top: 5px;
        }
        
        .footer {
            background: #f8f9fa;
            padding: 20px;
            text-align: center;
            border-top: 2px solid #e9ecef;
            color: #6c757d;
            font-size: 14px;
        }
        
        .footer strong {
            color: #495057;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }
        
        .status.running::before {
            content: "🏃 ";
            animation: pulse 1.5s infinite;
        }
        
        .status.stopped::before {
            content: "⏸️ ";
        }
        
        .danger-light-section {
            background: #f8f9fa;
            border-radius: 15px;
            padding: 20px;
            margin-bottom: 20px;
            border: 2px solid #e9ecef;
        }
        
        .danger-light-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 15px;
            font-size: 18px;
            font-weight: bold;
            color: #495057;
        }
        
        .toggle-switch {
            width: 60px;
            height: 30px;
            background: #dc3545;
            border-radius: 15px;
            position: relative;
            cursor: pointer;
            transition: background 0.3s;
        }
        
        .toggle-switch.active {
            background: #28a745;
        }
        
        .toggle-slider {
            width: 26px;
            height: 26px;
            background: white;
            border-radius: 50%;
            position: absolute;
            top: 2px;
            left: 2px;
            transition: left 0.3s;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        
        .toggle-switch.active .toggle-slider {
            left: 32px;
        }
        
        .danger-light-status {
            text-align: center;
            font-size: 16px;
            font-weight: bold;
            padding: 10px;
            border-radius: 8px;
            background: #f8d7da;
            color: #721c24;
        }
        
        .danger-light-status.on {
            background: #d4edda;
            color: #155724;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>👟 Smart Sports Shoes</h1>
            <p>Track Your Running Activity</p>
        </div>
        
        <div class="nav">
            <a href="/" class="active">Home</a>
            <a href="/config">Configuration</a>
        </div>
        
        <div class="content">
            <div id="status" class="status stopped">
                Status: Stopped
            </div>
            
            <div class="stat-card">
                <div class="stat-label">Total Steps</div>
                <div class="stat-value" id="stepCount">0</div>
                <div class="stat-unit">steps</div>
            </div>
            
            <div class="progress-section">
                <div class="progress-header">
                    <div class="progress-label">Daily Goal Progress</div>
                    <div class="progress-percentage" id="progressPercent">0%</div>
                </div>
                <div class="progress-container">
                    <div class="progress-bar" id="progressBar" style="width: 0%">
                        <span id="progressText"></span>
                    </div>
                </div>
            </div>
            
            <div class="session-stats">
                <div class="mini-stat">
                    <div class="mini-stat-label">Session Steps</div>
                    <div class="mini-stat-value" id="sessionSteps">0</div>
                    <div class="mini-stat-unit">steps</div>
                </div>
                <div class="mini-stat">
                    <div class="mini-stat-label">Calories Burned</div>
                    <div class="mini-stat-value" id="calories">0</div>
                    <div class="mini-stat-unit">kcal</div>
                </div>
            </div>
            
            <div class="mini-stat" style="margin-bottom: 20px;">
                <div class="mini-stat-label">Fat Burned</div>
                <div class="mini-stat-value" id="fat">0</div>
                <div class="mini-stat-unit">grams</div>
            </div>
            
            <div class="danger-light-section">
                <div class="danger-light-header">
                    <span>🚨 Danger Light</span>
                    <div class="toggle-switch" id="dangerToggle" onclick="toggleDangerLight()">
                        <div class="toggle-slider"></div>
                    </div>
                </div>
                <div class="danger-light-status" id="dangerStatus">OFF</div>
            </div>
            
            <div class="controls">
                <button class="btn btn-start" id="startBtn" onclick="startTracking()">Start</button>
                <button class="btn btn-stop" id="stopBtn" onclick="stopTracking()" disabled>Stop</button>
            </div>
            
            <button class="btn btn-export" onclick="exportCSV()">📊 Export Report (CSV)</button>
            
            <button class="btn btn-reset" onclick="resetCounter()">Reset Counter</button>
        </div>
        
        <div class="footer">
            <strong>Project Made By:</strong><br>
            Mathiyarasu R, Mellina S, Karoline Mary, Ezhil Arasan
        </div>
    </div>

    <script>
        let isRunning = false;
        let dangerLightOn = false;
        
        // Update stats every second
        setInterval(updateStats, 1000);
        
        // Initial update
        updateStats();
        
        function updateStats() {
            fetch('/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('stepCount').textContent = data.stepCount;
                    document.getElementById('sessionSteps').textContent = data.sessionSteps;
                    
                    // Calculate calories and fat
                    let calories = (data.sessionSteps * data.caloriesPerStep).toFixed(2);
                    let fat = (calories * data.fatPerCalorie * 1000).toFixed(2); // Convert to grams
                    
                    document.getElementById('calories').textContent = calories;
                    document.getElementById('fat').textContent = fat;
                    
                    // Update progress bar
                    let progress = Math.min((data.stepCount / data.dailyGoal) * 100, 100);
                    document.getElementById('progressBar').style.width = progress + '%';
                    document.getElementById('progressPercent').textContent = progress.toFixed(1) + '%';
                    
                    if (progress >= 100) {
                        document.getElementById('progressText').textContent = '🎉 Goal Achieved!';
                    } else {
                        document.getElementById('progressText').textContent = '';
                    }
                    
                    // Update status
                    isRunning = data.isRunning;
                    dangerLightOn = data.dangerLightEnabled;
                    updateButtonStates();
                    updateDangerLightUI();
                })
                .catch(error => console.error('Error:', error));
        }
        
        function updateDangerLightUI() {
            const toggle = document.getElementById('dangerToggle');
            const status = document.getElementById('dangerStatus');
            
            if (dangerLightOn) {
                toggle.classList.add('active');
                status.classList.add('on');
                status.textContent = 'ON - Blinking';
            } else {
                toggle.classList.remove('active');
                status.classList.remove('on');
                status.textContent = 'OFF';
            }
        }
        
        function toggleDangerLight() {
            if (dangerLightOn) {
                fetch('/danger-light-off')
                    .then(response => response.text())
                    .then(data => {
                        console.log(data);
                        updateStats();
                    })
                    .catch(error => console.error('Error:', error));
            } else {
                fetch('/danger-light-on')
                    .then(response => response.text())
                    .then(data => {
                        console.log(data);
                        updateStats();
                    })
                    .catch(error => console.error('Error:', error));
            }
        }
        
        function updateButtonStates() {
            let statusDiv = document.getElementById('status');
            let startBtn = document.getElementById('startBtn');
            let stopBtn = document.getElementById('stopBtn');
            
            if (isRunning) {
                statusDiv.className = 'status running';
                statusDiv.textContent = 'Status: Running';
                startBtn.disabled = true;
                stopBtn.disabled = false;
            } else {
                statusDiv.className = 'status stopped';
                statusDiv.textContent = 'Status: Stopped';
                startBtn.disabled = false;
                stopBtn.disabled = true;
            }
        }
        
        function startTracking() {
            fetch('/start')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                    updateStats();
                })
                .catch(error => console.error('Error:', error));
        }
        
        function stopTracking() {
            fetch('/stop')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                    updateStats();
                })
                .catch(error => console.error('Error:', error));
        }
        
        function resetCounter() {
            if (confirm('Are you sure you want to reset the step counter?')) {
                fetch('/reset')
                    .then(response => response.text())
                    .then(data => {
                        console.log(data);
                        updateStats();
                    })
                    .catch(error => console.error('Error:', error));
            }
        }
        
        function exportCSV() {
            // Create a temporary link and trigger download
            window.location.href = '/export-csv';
        }
    </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleConfig() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Sports Shoes - Configuration</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 32px;
            margin-bottom: 10px;
        }
        
        .nav {
            display: flex;
            background: #f8f9fa;
            border-bottom: 2px solid #e9ecef;
        }
        
        .nav a {
            flex: 1;
            padding: 15px;
            text-align: center;
            text-decoration: none;
            font-size: 16px;
            font-weight: 600;
            color: #6c757d;
            transition: all 0.3s;
            border-bottom: 3px solid transparent;
        }
        
        .nav a.active {
            background: white;
            color: #667eea;
            border-bottom: 3px solid #667eea;
        }
        
        .nav a:hover {
            background: #e9ecef;
        }
        
        .content {
            padding: 30px;
        }
        
        .section {
            margin-bottom: 30px;
        }
        
        .section-title {
            font-size: 20px;
            font-weight: bold;
            color: #495057;
            margin-bottom: 20px;
            padding-bottom: 10px;
            border-bottom: 2px solid #e9ecef;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .form-group label {
            display: block;
            margin-bottom: 8px;
            font-weight: 600;
            color: #495057;
        }
        
        .form-group input {
            width: 100%;
            padding: 12px;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        
        .form-group input:focus {
            outline: none;
            border-color: #667eea;
        }
        
        .form-group small {
            display: block;
            margin-top: 5px;
            color: #6c757d;
            font-size: 13px;
        }
        
        .save-btn {
            width: 100%;
            padding: 18px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.3s;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .save-btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .alert {
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
            font-weight: 500;
            display: none;
        }
        
        .alert-success {
            background: #d4edda;
            color: #155724;
            border: 2px solid #28a745;
        }
        
        .footer {
            background: #f8f9fa;
            padding: 20px;
            text-align: center;
            border-top: 2px solid #e9ecef;
            color: #6c757d;
            font-size: 14px;
        }
        
        .footer strong {
            color: #495057;
        }
        
        .info-box {
            background: #e7f3ff;
            border-left: 4px solid #2196F3;
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 5px;
        }
        
        .info-box p {
            margin: 5px 0;
            color: #0c5460;
            font-size: 14px;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>⚙️ Configuration</h1>
            <p>Customize Your Smart Sports Shoes</p>
        </div>
        
        <div class="nav">
            <a href="/">Home</a>
            <a href="/config" class="active">Configuration</a>
        </div>
        
        <div class="content">
            <div id="alert" class="alert alert-success">
                ✅ Configuration saved successfully!
            </div>
            
            <div class="info-box">
                <p><strong>ℹ️ Note:</strong> Changes will be saved to ESP32 memory and persist after restart.</p>
            </div>
            
            <form id="configForm" onsubmit="saveConfig(event)">
                <div class="section">
                    <div class="section-title">Hardware Settings</div>
                    
                    <div class="form-group">
                        <label for="piezoPin">Piezo Sensor Pin</label>
                        <input type="number" id="piezoPin" name="piezoPin" min="0" max="39" required>
                        <small>GPIO pin connected to piezo sensor (0-39 for ESP32)</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="threshold">Threshold Value</label>
                        <input type="number" id="threshold" name="threshold" min="1" max="4095" required>
                        <small>Minimum sensor value to count as a step (1-4095)</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="stepDelay">Step Delay (ms)</label>
                        <input type="number" id="stepDelay" name="stepDelay" min="100" max="5000" required>
                        <small>Minimum time between steps in milliseconds (prevents double counting)</small>
                    </div>
                </div>
                
                <div class="section">
                    <div class="section-title">Goal & Calculation Settings</div>
                    
                    <div class="form-group">
                        <label for="dailyGoal">Daily Step Goal</label>
                        <input type="number" id="dailyGoal" name="dailyGoal" min="100" max="100000" required>
                        <small>Your daily step target</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="caloriesPerStep">Calories Per Step</label>
                        <input type="number" step="0.001" id="caloriesPerStep" name="caloriesPerStep" min="0.01" max="1" required>
                        <small>Calories burned per step (default: 0.04 kcal)</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="fatPerCalorie">Fat Per Calorie (kg)</label>
                        <input type="number" step="0.00001" id="fatPerCalorie" name="fatPerCalorie" min="0.0001" max="0.001" required>
                        <small>Fat burned per calorie in kg (default: 0.00013 kg)</small>
                    </div>
                </div>
                
                <div class="section">
                    <div class="section-title">Danger Light Settings</div>
                    
                    <div class="form-group">
                        <label for="ledPin">LED Pin</label>
                        <input type="number" id="ledPin" name="ledPin" min="0" max="39" required>
                        <small>GPIO pin connected to danger light LED (default: 2)</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="ledOnDelay">LED On Delay (ms)</label>
                        <input type="number" id="ledOnDelay" name="ledOnDelay" min="50" max="5000" required>
                        <small>Time LED stays ON during blink cycle (default: 200 ms)</small>
                    </div>
                    
                    <div class="form-group">
                        <label for="ledOffDelay">LED Off Delay (ms)</label>
                        <input type="number" id="ledOffDelay" name="ledOffDelay" min="50" max="5000" required>
                        <small>Time LED stays OFF during blink cycle (default: 800 ms)</small>
                    </div>
                </div>
                
                <button type="submit" class="save-btn">💾 Save Configuration</button>
            </form>
        </div>
        
        <div class="footer">
            <strong>Project Made By:</strong><br>
            Mathiyarasu R, Mellina S, Karoline Mary, Ezhil Arasan
        </div>
    </div>

    <script>
        // Load current configuration
        window.onload = function() {
            fetch('/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('piezoPin').value = data.piezoPin;
                    document.getElementById('threshold').value = data.thresholdValue;
                    document.getElementById('stepDelay').value = data.stepDelay;
                    document.getElementById('dailyGoal').value = data.dailyGoal;
                    document.getElementById('caloriesPerStep').value = data.caloriesPerStep;
                    document.getElementById('fatPerCalorie').value = data.fatPerCalorie;
                    document.getElementById('ledPin').value = data.ledPin;
                    document.getElementById('ledOnDelay').value = data.ledOnDelay;
                    document.getElementById('ledOffDelay').value = data.ledOffDelay;
                })
                .catch(error => console.error('Error:', error));
        };
        
        function saveConfig(event) {
            event.preventDefault();
            
            const formData = new FormData(event.target);
            const params = new URLSearchParams();
            
            for (let [key, value] of formData.entries()) {
                params.append(key, value);
            }
            
            fetch('/save-config', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: params
            })
            .then(response => response.text())
            .then(data => {
                // Show success message
                const alert = document.getElementById('alert');
                alert.style.display = 'block';
                setTimeout(() => {
                    alert.style.display = 'none';
                }, 3000);
                
                console.log(data);
            })
            .catch(error => console.error('Error:', error));
        }
    </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleSaveConfig() {
  if (server.hasArg("piezoPin")) {
    piezoPin = server.arg("piezoPin").toInt();
    pinMode(piezoPin, INPUT);
  }
  if (server.hasArg("threshold")) {
    thresholdValue = server.arg("threshold").toInt();
  }
  if (server.hasArg("stepDelay")) {
    stepDelay = server.arg("stepDelay").toInt();
  }
  if (server.hasArg("dailyGoal")) {
    dailyGoal = server.arg("dailyGoal").toInt();
  }
  if (server.hasArg("caloriesPerStep")) {
    caloriesPerStep = server.arg("caloriesPerStep").toFloat();
  }
  if (server.hasArg("fatPerCalorie")) {
    fatPerCalorie = server.arg("fatPerCalorie").toFloat();
  }
  
  // Save danger light configuration
  if (server.hasArg("ledPin")) {
    ledPin = server.arg("ledPin").toInt();
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
  }
  if (server.hasArg("ledOnDelay")) {
    ledOnDelay = server.arg("ledOnDelay").toInt();
  }
  if (server.hasArg("ledOffDelay")) {
    ledOffDelay = server.arg("ledOffDelay").toInt();
  }
  
  // Save to preferences
  saveConfiguration();
  
  server.send(200, "text/plain", "Configuration saved successfully!");
}

void handleGetStats() {
  StaticJsonDocument<512> doc;
  
  doc["stepCount"] = stepCount;
  doc["sessionSteps"] = sessionSteps;
  doc["isRunning"] = isRunning;
  doc["piezoPin"] = piezoPin;
  doc["thresholdValue"] = thresholdValue;
  doc["stepDelay"] = stepDelay;
  doc["dailyGoal"] = dailyGoal;
  doc["caloriesPerStep"] = caloriesPerStep;
  doc["fatPerCalorie"] = fatPerCalorie;
  doc["ledPin"] = ledPin;
  doc["ledOnDelay"] = ledOnDelay;
  doc["ledOffDelay"] = ledOffDelay;
  doc["dangerLightEnabled"] = dangerLightEnabled;
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handleStart() {
  isRunning = true;
  sessionSteps = 0;
  sessionStartTime = millis();
  
  Serial.println("Tracking started!");
  server.send(200, "text/plain", "Tracking started!");
}

void handleStop() {
  isRunning = false;
  
  Serial.println("Tracking stopped!");
  Serial.printf("Session steps: %d\n", sessionSteps);
  Serial.printf("Calories burned: %.2f\n", sessionSteps * caloriesPerStep);
  Serial.printf("Fat burned: %.2f grams\n", sessionSteps * caloriesPerStep * fatPerCalorie * 1000);
  
  server.send(200, "text/plain", "Tracking stopped!");
}

void handleReset() {
  stepCount = 0;
  sessionSteps = 0;
  
  Serial.println("Step counter reset!");
  server.send(200, "text/plain", "Counter reset!");
}

void handleDangerLightOn() {
  dangerLightEnabled = true;
  ledState = false;
  lastLedToggle = millis();
  
  Serial.println("Danger light enabled!");
  server.send(200, "text/plain", "Danger light ON!");
}

void handleDangerLightOff() {
  dangerLightEnabled = false;
  digitalWrite(ledPin, LOW);
  ledState = false;
  
  Serial.println("Danger light disabled!");
  server.send(200, "text/plain", "Danger light OFF!");
}

void updateDangerLight() {
  if (!dangerLightEnabled) {
    return;
  }
  
  unsigned long currentTime = millis();
  
  if (ledState) {
    // LED is currently ON, check if it's time to turn OFF
    if (currentTime - lastLedToggle >= ledOnDelay) {
      digitalWrite(ledPin, LOW);
      ledState = false;
      lastLedToggle = currentTime;
    }
  } else {
    // LED is currently OFF, check if it's time to turn ON
    if (currentTime - lastLedToggle >= ledOffDelay) {
      digitalWrite(ledPin, HIGH);
      ledState = true;
      lastLedToggle = currentTime;
    }
  }
}
