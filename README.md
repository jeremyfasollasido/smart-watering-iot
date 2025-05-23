# 🌿 Smart Watering IoT System

## Automate Your Garden's Hydration with Real-time Control

---

Are you tired of guessing when to water your plants? Do you want to ensure your garden thrives even when you're away? This **Smart Watering IoT System** provides an efficient and intelligent solution for automated plant irrigation, leveraging an ESP32 microcontroller, a soil moisture sensor, and an MQTT-based communication for real-time monitoring and control.

Gone are the days of over or under-watering. This system helps you maintain optimal moisture levels for your plants, promoting healthier growth and conserving water.

### ✨ Features

* **Real-time Soil Moisture Monitoring:** Get instant updates on your soil's hydration level (0-100%) directly on a web-based UI and an LCD screen.
* **Automated or Manual Watering Control:** The system can be expanded for automated watering based on moisture thresholds, or you can manually trigger the pump via a simple web switch.
* **MQTT Communication:** Utilizes the lightweight and efficient MQTT protocol for seamless, event-driven communication between the ESP32 and the web UI. This ensures reliable data transfer and immediate command execution.
* **Local Web UI:** Access your watering system from any device on your local network using a clean and intuitive web interface (HTML, CSS, JavaScript). No need for third-party cloud services like Blynk!
* **Compact & Cost-Effective:** Built with readily available and affordable components, making it an ideal project for DIY enthusiasts and smart home developers.

### 🛠️ Hardware Requirements

* **ESP32 Development Board:** (e.g., ESP32 DevKitC)
* **Capacitive Soil Moisture Sensor:** (e.g., v1.2)
* **5V Relay Module:** For controlling the water pump.
* **Small DC Water Pump:** (Optional, if you want to automate watering)
* **16x2 I2C LCD Display:** For local status display.
* **Breadboard & Jumper Wires**
* **5V Power Supply**

### 💻 Software Requirements

* **Arduino IDE:** For programming the ESP32.
* **Libraries for Arduino IDE:**
    * `LiquidCrystal_I2C`
    * `Wire`
    * `WiFi`
    * `PubSubClient` (for MQTT)
    * `ArduinoJson` (for parsing MQTT messages)
* **MQTT Broker:**
    * **Recommended:** A local [Mosquitto MQTT broker](https://mosquitto.org/) (e.g., on a Raspberry Pi or a PC).
    * **For testing:** A public broker like `broker.hivemq.com` (ensure to use WebSocket port like `8000`).
* **Web Server (Optional, for hosting UI):** A simple local web server (e.g., Python's `http.server`, Node.js `http-server`) to serve the HTML/CSS/JS files, though you can also open `index.html` directly in a browser for quick testing.

---

### 🚀 Getting Started

Follow these steps to set up your Smart Watering IoT System:

1.  **Clone the Repository:**
    ```bash
    git clone [https://github.com/YOUR_USERNAME/smart-watering-iot.git](https://github.com/YOUR_USERNAME/smart-watering-iot.git)
    cd smart-watering-iot
    ```

2.  **ESP32 Firmware Setup:**
    * Open the `smart_watering_esp32.ino` (or similar) file in the Arduino IDE.
    * **Install Libraries:** Go to `Sketch > Include Library > Manage Libraries...` and search for and install `LiquidCrystal_I2C`, `PubSubClient`, and `ArduinoJson`.
    * **Configure WiFi:** Update `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` in the code.
    * **Configure MQTT Broker:** Set `mqtt_server` and `mqtt_port` to your MQTT broker's address and port.
    * **Upload:** Connect your ESP32 board, select the correct board and port in Arduino IDE, and upload the sketch.

3.  **MQTT Broker Setup:**
    * If you're using a public broker like `broker.hivemq.com`, no setup is needed, but be aware of its public nature.
    * **For Local Broker (Recommended):** Install Mosquitto on your preferred device. Ensure it's configured to accept WebSocket connections (e.g., on port 8000 or 8083) if you're serving the web UI directly from your browser.

4.  **Web UI Setup:**
    * Navigate to the `web-ui` directory (or wherever you save your HTML, CSS, JS files).
    * **Important:** Open `script.js` and update `MQTT_BROKER_HOST` and `MQTT_BROKER_PORT` to match your MQTT broker's WebSocket address and port.
    * You can directly open `index.html` in your web browser. For a more robust setup, serve these files using a local web server (e.g., `python -m http.server 8000` in the `web-ui` directory).

5.  **Access the UI:**
    * Once your ESP32 is connected to Wi-Fi and your MQTT broker, and your web UI is loaded in your browser:
        * If opening `index.html` directly, the connection will establish.
        * If using a local web server, navigate to `http://localhost:8000` (or your server's address).

### 📐 Project Structure
.
├── smart_watering_esp32/       # Arduino IDE project for ESP32 firmware
│   └── smart_watering_esp32.ino
├── web-ui/                     # Web-based UI files
│   ├── index.html
│   ├── style.css
│   └── script.js
└── README.md

### 🤝 Contributing

Contributions are always welcome! If you have ideas for improvements, new features, or bug fixes, feel free to:

1.  Fork the repository.
2.  Create a new branch (`git checkout -b feature/your-feature`).
3.  Make your changes.
4.  Commit your changes (`git commit -m 'Add new feature'`).
5.  Push to the branch (`git push origin feature/your-feature`).
6.  Open a Pull Request.

