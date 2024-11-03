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

// Saat menerima pesan dari MQTT
mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    console.log(`Received data from ${topic}:`, data);
    broadcastToWebSocketClients(data);
  } catch (err) {
    console.error('Error parsing MQTT message:', err);
  }
});

// Fungsi untuk mengirim data ke semua klien WebSocket
const broadcastToWebSocketClients = (data) => {
  wss.clients.forEach(client => {
    if (client.readyState === client.OPEN) {
      client.send(JSON.stringify(data));
    }
  });
};

app.use(cors({ origin: '*' }));
app.use(express.json());

// Koneksi ke MongoDB
dbConnect;

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

// Menangani error pada WebSocket
wss.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// Mulai server
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
