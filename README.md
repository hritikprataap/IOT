# 🌱 IoT-Based Smart Irrigation System

An IoT-enabled smart irrigation system built using **ESP32, C++, soil moisture sensors, and Blynk IoT** to automate plant watering based on real-time soil conditions. The system supports remote monitoring, manual pump control, automatic irrigation, water-level protection, and sensor-based notifications.

## 📌 Project Overview

Traditional irrigation often requires manual monitoring and watering, which can lead to water wastage and inconsistent plant care.

This project addresses these challenges by using an ESP32 microcontroller to monitor soil moisture and control a water pump according to configurable moisture thresholds. A Blynk IoT dashboard enables users to monitor sensor readings and control irrigation remotely.

## ✨ Features

- **Automated Irrigation:** Activates the water pump when soil moisture falls below the configured threshold.
- **Real-Time Monitoring:** Displays soil moisture, water availability, and pump status on the Blynk dashboard.
- **Manual and Automatic Modes:** Supports automatic watering and user-initiated pump control.
- **Water-Level Protection:** Prevents pump operation when the water tank is empty or the low-water switch is triggered.
- **Smart Notifications:** Sends alerts when soil moisture is low or the water tank reaches a low level.
- **Pump Runtime Protection:** Stops the pump after a configured maximum continuous runtime.
- **Remote Control:** Allows users to monitor the system and issue pump commands through the Blynk IoT application.

## 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| ESP32 | Microcontroller and local control logic |
| C++ | Embedded firmware development |
| Arduino Framework | ESP32 programming environment |
| Blynk IoT | Remote monitoring, control, and notifications |
| Wi-Fi | Communication between ESP32 and Blynk Cloud |
| Soil Moisture Sensor | Measures soil moisture |
| Float Switch | Detects water availability in the tank |
| Relay Module | Switches the water pump |

## 🔌 Hardware Requirements

- ESP32 development board
- Capacitive soil moisture sensor or compatible analog sensor
- Water-level float switch
- Relay module compatible with the control circuit
- DC water pump
- Suitable external pump power supply
- Jumper wires and connecting components
- Computer with USB data cable

## 🧩 System Architecture

```text
       Soil Moisture Sensor
                |
                v
       +------------------+
       |       ESP32      |
       |                  |
       | Sensor Processing|
       | Threshold Logic  |
       | Safety Checks    |
       +------------------+
          |            |
          v            v
    Relay Module    Wi-Fi
          |            |
          v            v
      Water Pump   Blynk Cloud
                       |
                       v
                Mobile Dashboard
                - Moisture Level
                - Water Availability
                - Pump Status
                - Manual Control
                - Notifications
```

## 📍 Pin Configuration

The following GPIO assignments are used in the reference firmware. Verify them against your actual wiring before powering the system.

| Component | ESP32 GPIO |
|---|---|
| Soil moisture sensor analog output | GPIO 34 |
| Water-level float switch | GPIO 27 |
| Relay module input | GPIO 26 |

## ⚙️ How It Works

1. **Sensor Reading:** The ESP32 reads the analog output of the soil moisture sensor.
2. **Moisture Calculation:** The raw ADC reading is converted into an estimated moisture percentage using calibrated dry and wet reference values.
3. **Automatic Irrigation:** When the moisture level falls below the configured dry threshold, the controller requests irrigation.
4. **Pump Activation:** The relay switches the pump on if water is available and no safety lockout is active.
5. **Irrigation Stop:** The pump stops when the moisture reaches the configured wet threshold.
6. **Water-Level Protection:** If the tank is low, the controller switches off the pump and prevents further operation.
7. **Remote Monitoring:** The ESP32 sends sensor readings and pump status to Blynk over Wi-Fi.
8. **Notifications:** The system can generate alerts for low soil moisture and low tank water levels.

## 📱 Blynk IoT Configuration

Create a Blynk template and device, then configure the following virtual datastreams:

| Virtual Pin | Data | Description |
|---|---|---|
| V0 | Integer, 0–100 | Soil moisture percentage |
| V1 | Integer, 0–1 | Water availability |
| V2 | Integer, 0–1 | Operating mode: automatic/manual |
| V3 | Integer, 0–1 | Manual pump request |
| V4 | Integer, 0–1 | Actual pump status |

Create two Blynk events for notifications:

- `soil_dry` — soil moisture is below the configured threshold.
- `low_water` — the water tank is low.

Configure notification delivery in the Blynk console.

## 💻 Software Setup

### Prerequisites

- Visual Studio Code
- PlatformIO IDE extension
- ESP32 development board
- Blynk IoT account
- Wi-Fi connection for cloud features

### Installation

1. Clone this repository:

   ```bash
   git clone https://github.com/YOUR_USERNAME/iot-smart-irrigation-system.git
   cd iot-smart-irrigation-system
   ```

2. Open the project folder in VS Code.

3. Configure the PlatformIO project using an appropriate `platformio.ini` file.

4. Install the ESP32 Arduino framework and Blynk library.

5. Configure your Blynk template ID, device authentication token, and Wi-Fi credentials locally.

6. Verify GPIO assignments, relay polarity, and sensor calibration values.

7. Build and upload the firmware using PlatformIO.

   ```bash
   pio run
   pio run --target upload
   ```

8. Open the Serial Monitor to inspect sensor readings and pump status.

   ```bash
   pio device monitor
   ```

## 🔐 Configuration and Security

Do not commit real Wi-Fi passwords or Blynk authentication tokens to a public repository.

Keep local credentials in an untracked configuration file and provide a sanitized example configuration for other users.

Add local secret files to `.gitignore` and ensure no actual credentials are included in committed source files.

## 🧪 Testing

The following scenarios should be tested with the physical hardware:

- Verify sensor readings in dry and wet soil.
- Confirm correct moisture percentage calibration.
- Test automatic pump activation and deactivation.
- Verify manual pump control from Blynk.
- Confirm the pump stops when the tank is low.
- Verify the maximum continuous runtime protection.
- Test dashboard updates and notification delivery.
- Test behavior during Wi-Fi disconnection and reconnection.

## 🔒 Safety Considerations

- Never power the pump directly from an ESP32 GPIO.
- Use a properly rated external pump supply and relay or driver.
- Verify sensor voltage compatibility with the ESP32.
- Test relay behavior before connecting the pump.
- Use appropriate electrical isolation and enclosure protection around water.
- Validate runtime limits and tank-level protection before unattended operation.

## 🚀 Future Enhancements

- Store historical soil moisture readings.
- Add water consumption measurement.
- Introduce scheduled irrigation and plant-specific thresholds.
- Add multiple soil sensors for different plant zones.
- Improve offline-first operation and cloud reconnection handling.
- Develop water usage analytics and irrigation reports.

## 👨‍💻 Author

**Hritik Singh**

BE Engineering Student | Embedded Systems | IoT | C++ | Backend Development

---

*This project is intended for educational and demonstration purposes. Sensor calibration, hardware compatibility, and safety behavior should be validated on the actual setup before deployment.*
