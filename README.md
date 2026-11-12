# 🌱 Smart Plant Watering System

An Arduino-based Smart Plant Watering System designed to monitor soil moisture and automate plant watering. The project combines Arduino hardware with a web-based dashboard for plant monitoring, watering control, and analytics.

## 📌 Project Overview

The Smart Plant Watering System helps maintain optimal soil moisture by monitoring the moisture level and controlling a water pump through a relay.

It also provides a web dashboard where users can monitor soil moisture, check pump status, simulate soil conditions, and view watering analytics.

## ✨ Features

- 🌱 Soil moisture monitoring.
- 💧 Automatic watering based on moisture thresholds.
- ⚡ Relay-based water pump control.
- 📊 Web dashboard with moisture analytics.
- 🔌 Arduino connectivity using Web Serial API.
- 🛑 Sensor fault detection and safety stop.
- 📱 Responsive dashboard interface.
- 🔔 Plant care reminders.
- 🧪 Simulation mode for testing without hardware.

## 🛠️ Technologies Used

**Hardware:**
- Arduino
- Soil Moisture Sensor
- Relay Module
- Water Pump

**Software:**
- Arduino IDE
- HTML
- CSS
- JavaScript
- Chart.js
- Web Serial API

## 📂 Project Structure

```text
Smart-Plant-Watering-System/
│
├── project_arduino.ino
├── dashboard.html
└── README.md
```

## ⚙️ Working Principle

1. The soil moisture sensor measures the moisture level in the soil.
2. Arduino processes the sensor readings.
3. When the soil becomes dry, the system activates the water pump through the relay.
4. The pump stops when the required moisture condition is achieved.
5. The web dashboard displays moisture readings, pump status, and system analytics.

## 🚀 How to Run

### 1. Arduino Setup
- Open `project_arduino.ino` in Arduino IDE.
- Connect the Arduino board to your computer.
- Select the correct board and COM port.
- Upload the code.

### 2. Dashboard Setup
- Open `dashboard.html` in Google Chrome or Microsoft Edge.
- Click on **Connect Arduino**.
- Select the correct serial port.
- The dashboard can now receive Arduino data.

**Note:** Web Serial API requires a supported desktop browser. If opening the HTML file directly does not work, run it through localhost using VS Code Live Server.

## 📊 Dashboard Modules

- Dashboard
- My Plants
- Watering Control
- Analytics
- Reminders

## 🎯 Project Objective

To develop an affordable and efficient smart irrigation solution that reduces unnecessary water usage, minimizes manual effort, and supports healthier plant growth.

## 🔮 Future Scope

- Mobile application integration.
- IoT-based remote monitoring.
- Cloud data storage.
- Weather-based watering recommendations.
- AI-based plant health monitoring.
- Multiple plant monitoring.

## 👩‍💻 Author

**Urvashi Sharma**

B.Tech – Computer Science and Engineering

##
