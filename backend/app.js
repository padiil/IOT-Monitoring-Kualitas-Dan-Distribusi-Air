import express from 'express';
import http from 'http';
import { WebSocketServer } from 'ws';
import mqtt from 'mqtt';
import cors from 'cors';
import SensorData from './db/models/SensorData.model.js';
import dbConnect from './db/DbConnect.js';

const PORT = 3000;
const TOPIC = 'sensor/data';

const app = express();
const server = http.createServer(app);
const wss = new WebSocketServer({ server });

// Koneksi ke MongoDB
dbConnect; // Pastikan koneksi dijalankan

// Koneksi ke broker MQTT
const mqttClient = mqtt.connect('mqtt://localhost');

mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  mqttClient.subscribe(TOPIC, (err) => {
    if (!err) {
      console.log(`Subscribed to topic: ${TOPIC}`);
    } else {
      console.error(`Failed to subscribe to topic ${TOPIC}:`, err);
    }
  });
});

mqttClient.on('error', (err) => {
  console.error('MQTT connection error:', err);
});

// Saat menerima pesan dari MQTT
mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    // console.log(`Received data from ${topic}:`, data);
    broadcastToWebSocketClients(data);
  } catch (err) {
    console.error('Error parsing MQTT message:', err);
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

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);
      console.log('Received from client:', data);

      if (data.type === 'switchUpdate') {
        console.log(`Updating switch status for sensor: ${data.sensor}, useServer: ${data.useServer}`);

        // Jika useServer adalah false, kita kirimkan nilai sensor yang baru
        const payload = {
          useServer: data.useServer,
        };

        // Jika useServer false, kita kirimkan nilai sensor yang diubah
        if (!data.useServer && data.sensorValue !== undefined) {
          payload.sensorValue = data.sensorValue;
          console.log(`Sensor value to send: ${data.sensorValue}`);
        }

        // Kirim pembaruan ke MQTT untuk sensor yang relevan
        mqttClient.publish(
          `sensor/update/${data.sensor}`,  // Topik untuk ESP32
          JSON.stringify(payload),  // Mengirim data payload ke MQTT
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
      console.error('Error parsing message from client:', err);
    }
  });

  ws.on('error', (err) => {
    console.error('WebSocket client error:', err);
  });
});


// Middleware
app.use(cors({ origin: '*' }));
app.use(express.json());

// Endpoint untuk cek status server
app.get('/', (req, res) => {
  res.send('WebSocket server is running!');
});

// Endpoint untuk menyimpan data sensor
app.post('/sensor-data', async (req, res) => {
  try {
    const sensorData = new SensorData(req.body);
    await sensorData.save();
    res.status(201).send('Data saved successfully!');
  } catch (err) {
    res.status(500).send('Error saving data: ' + err.message);
  }
});

// Menangani error pada WebSocket server
wss.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// Mulai server
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
