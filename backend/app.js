import cors from 'cors';
import http from 'http';
import mqtt from 'mqtt';
import express from 'express';
import { WebSocketServer } from 'ws';
import { RealTimeData } from './db/models/SensorData.model.js';
import dbConnect from './db/DbConnect.js';
import processLongTermData from './db/dataProcess.js';

const PORT = 3000;
const MQTT_BROKER = 'mqtt://localhost';
const MQTT_TOPIC = 'sensor/data';

const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

// Koneksi ke MongoDB
dbConnect;

// Middleware
app.use(cors({ origin: '*' }));
app.use(express.json());

// Koneksi ke broker MQTT
const mqttClient = mqtt.connect(MQTT_BROKER);

mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  mqttClient.subscribe(MQTT_TOPIC, (err) => {
    if (err) {
      console.error(`Failed to subscribe to topic ${MQTT_TOPIC}:`, err);
    } else {
      console.log(`Subscribed to topic: ${MQTT_TOPIC}`);
    }
  });
});

mqttClient.on('error', (err) => {
  console.error('MQTT connection error:', err);
});

// Saat menerima pesan dari MQTT
mqttClient.on('message', async (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    // console.log(`Received data from ${topic}:`, data);

    // Simpan data ke MongoDB
    const newSensorData = new RealTimeData(data);
    await newSensorData.save();

    // Kirim data ke klien WebSocket
    broadcastToWebSocketClients(data);
  } catch (err) {
    console.error('Error processing MQTT message:', err);
  }
});

// Fungsi untuk mengirim data ke semua klien WebSocket
const broadcastToWebSocketClients = (data) => {
  wss.clients.forEach((client) => {
    if (client.readyState === client.OPEN) {
      client.send(JSON.stringify(data));
    }
  });
};

// WebSocket connection handling
wss.on('connection', (ws) => {
  console.log('New WebSocket connection');

  ws.on('message', async (message) => {
    try {
      const data = JSON.parse(message);
      console.log('Received from client:', data);

      if (data.type === 'switchUpdate') {
        console.log(`Updating switch status for sensor: ${data.sensor}, useServer: ${data.useServer}`);

        const payload = { useServer: data.useServer };

        // Jika `useServer` adalah false, tambahkan nilai sensor
        if (!data.useServer && data.sensorValue !== undefined) {
          payload.sensorValue = data.sensorValue;
          console.log(`Sensor value to send: ${data.sensorValue}`);
        }

        // Kirim pembaruan ke MQTT untuk sensor terkait
        mqttClient.publish(
          `sensor/update/${data.sensor}`,
          JSON.stringify(payload),
          (err) => {
            if (err) {
              console.error('Failed to publish switch update to MQTT:', err);
            } else {
              console.log(`Switch update for ${data.sensor} sent to MQTT`);
            }
          }
        );
      }
    } catch (err) {
      console.error('Error processing WebSocket message:', err);
    }
  });

  ws.on('error', (err) => {
    console.error('WebSocket client error:', err);
  });

  ws.on('close', () => {
    console.log('WebSocket connection closed');
  });
});

// Jalankan fungsi processLongTermData setiap menit
processLongTermData;

// Endpoint untuk cek status server
app.get('/', (req, res) => {
  res.send('WebSocket and MQTT server is running!');
});

// Menangani error pada WebSocket server
wss.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// Mulai server
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
