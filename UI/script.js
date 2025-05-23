document.addEventListener('DOMContentLoaded', () => {
    const moistureValueDisplay = document.getElementById('moistureValue');
    const gaugeFill = document.getElementById('gaugeFill');
    const motorSwitch = document.getElementById('motorSwitch');
    const motorStatusText = document.getElementById('motorStatus');
    const mqttStatusText = document.getElementById('mqttStatus');

    // --- MQTT Configuration ---
    // IMPORTANT: Use a WebSocket port for your MQTT broker (often 8083, 8084 for SSL)
    // For HiveMQ public broker:
    const MQTT_BROKER_HOST = "broker.hivemq.com";
    const MQTT_BROKER_PORT = 8000; // WebSocket port for HiveMQ
    const MQTT_CLIENT_ID = "web_ui_client_" + parseInt(Math.random() * 1000); // Unique client ID

    const MQTT_TOPIC_MOISTURE = "smartwatering/moisture";
    const MQTT_TOPIC_MOTOR_CONTROL = "smartwatering/motor/control";
    const MQTT_TOPIC_MOTOR_STATUS = "smartwatering/motor/status"; // To get current motor state from ESP32

    let client; // MQTT client instance

    // Function to update the gauge
    function updateMoistureGauge(value) {
        if (value < 0) value = 0;
        if (value > 100) value = 100;

        moistureValueDisplay.textContent = `${value}%`;
        gaugeFill.style.height = `${value}%`;
    }

    // Function to initialize MQTT client
    function connectMqtt() {
        client = new Paho.MQTT.Client(MQTT_BROKER_HOST, Number(MQTT_BROKER_PORT), MQTT_CLIENT_ID);

        client.onConnectionLost = onConnectionLost;
        client.onMessageArrived = onMessageArrived;

        mqttStatusText.textContent = 'MQTT Status: Connecting...';
        client.connect({
            onSuccess: onConnect,
            onFailure: onFailure,
            useSSL: false // Set to true if your broker uses SSL (e.g., port 8084)
        });
    }

    function onConnect() {
        console.log("MQTT Connected");
        mqttStatusText.textContent = 'MQTT Status: Connected';
        // Subscribe to topics
        client.subscribe(MQTT_TOPIC_MOISTURE);
        client.subscribe(MQTT_TOPIC_MOTOR_STATUS); // Subscribe to receive motor status updates from ESP32
        console.log(`Subscribed to: ${MQTT_TOPIC_MOISTURE}, ${MQTT_TOPIC_MOTOR_STATUS}`);
    }

    function onFailure(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("MQTT Connection Failed: " + responseObject.errorMessage);
            mqttStatusText.textContent = `MQTT Status: Failed (${responseObject.errorMessage})`;
            setTimeout(connectMqtt, 5000); // Try to reconnect after 5 seconds
        }
    }

    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("MQTT Connection Lost: " + responseObject.errorMessage);
            mqttStatusText.textContent = `MQTT Status: Lost (${responseObject.errorMessage})`;
            setTimeout(connectMqtt, 5000); // Try to reconnect after 5 seconds
        }
    }

    function onMessageArrived(message) {
        console.log("Message Arrived: " + message.destinationName + " - " + message.payloadString);
        try {
            const payload = JSON.parse(message.payloadString);

            if (message.destinationName === MQTT_TOPIC_MOISTURE) {
                if (typeof payload.moisture !== 'undefined') {
                    updateMoistureGauge(payload.moisture);
                }
            } else if (message.destinationName === MQTT_TOPIC_MOTOR_STATUS) {
                if (payload.status === 'on') {
                    motorStatusText.textContent = 'Motor is ON';
                    motorSwitch.checked = true;
                } else if (payload.status === 'off') {
                    motorStatusText.textContent = 'Motor is OFF';
                    motorSwitch.checked = false;
                }
            }
        } catch (e) {
            console.error("Error parsing MQTT message payload:", e);
        }
    }

    // Function to publish motor control command
    function publishMotorControl(state) {
        const action = state ? "on" : "off";
        const message = new Paho.MQTT.Message(JSON.stringify({ action: action }));
        message.destinationName = MQTT_TOPIC_MOTOR_CONTROL;
        try {
            client.send(message);
            console.log(`Published motor control: ${action}`);
        } catch (e) {
            console.error("Error sending MQTT message:", e);
            mqttStatusText.textContent = 'MQTT Status: Send Error';
            motorSwitch.checked = !state; // Revert switch if publish failed
        }
    }

    // Event listener for the motor switch
    motorSwitch.addEventListener('change', (event) => {
        publishMotorControl(event.target.checked);
    });

    // Initial MQTT connection
    connectMqtt();
});
