import React, { useEffect, useState } from 'react';
import { Badge } from "@/components/ui/badge";
import {
  Card,
  CardContent,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

function App() {
  const [sensorData, setSensorData] = useState(null);
  const [isConnected, setIsConnected] = useState(false);
  const IP_SERVER = '192.168.100.187:3000';

  useEffect(() => {
    const ws = new WebSocket(`ws://${IP_SERVER}`);

    ws.onopen = () => {
      console.log('WebSocket connected');
      setIsConnected(true);
    };

    ws.onerror = (error) => {
      console.error('WebSocket error:', error);
      setIsConnected(false);
    };

    ws.onmessage = (event) => {
      try {
        const newData = JSON.parse(event.data);
        console.log('Received:', newData); // Log untuk memeriksa data yang diterima
        setSensorData(newData);
      } catch (error) {
        console.error('Error parsing JSON:', error);
      }
    };

    ws.onclose = () => {
      console.log('WebSocket disconnected');
      setIsConnected(false);
    };

    return () => {
      ws.close();
    };
  }, []);

  // Tentukan warna badge berdasarkan status kualitas air
  let badgeColor = 'default';
  let desk = '---';
  if (sensorData) {
    if (sensorData.waterQuality === 'baik') {
      badgeColor = 'success';
      desk = 'Kualitas air baik, layak untuk digunakan.';
    } else if (sensorData.waterQuality === 'cemar ringan') {
      badgeColor = 'warning';
      desk = 'Kualitas air cemar ringan, perlu perhatian.';
    } else if (sensorData.waterQuality === 'cemar sedang') {
      badgeColor = 'warning';
      desk = 'Kualitas air cemar sedang, perlu penanganan lebih lanjut.';
    } else if (sensorData.waterQuality === 'cemar berat') {
      badgeColor = 'destructive';
      desk = 'Kualitas air cemar berat, tidak layak untuk digunakan.';
    }
  }

  return (
    <div className="container mx-auto px-4">
      <Card className="my-4">
        <div className="flex justify-between items-center">
          <CardHeader>
            <CardTitle>Kualitas Air</CardTitle>
          </CardHeader>
          <CardContent className="flex justify-center items-center h-full py-0">
            <Badge variant={badgeColor}>
              <strong>
                {sensorData ? sensorData.waterQuality : 'Loading...'}
              </strong>
            </Badge>
          </CardContent>
        </div>
        <CardFooter>
          <p>
            {sensorData ? desk : 'Loading...'}
          </p>
        </CardFooter>
      </Card>

      <div className="grid grid-cols-2 gap-4 my-4">
        {[
          { key: 'pH', label: 'pH', unit: '' },
          { key: 'DO', label: 'Oksigen Terlarut (DO)', unit: 'mg/L' },
          { key: 'BOD', label: 'Kebutuhan Oksigen Biologi (BOD)', unit: 'mg/L' },
          { key: 'COD', label: 'Kebutuhan Oksigen Kimiawi (COD)', unit: 'mg/L' },
          { key: 'TSS', label: 'Padatan Tersuspensi Total (TSS)', unit: 'mg/L' },
          { key: 'nitrat', label: 'Nitrat (NO3-N)', unit: 'mg/L' },
          { key: 'fosfat', label: 'Total Fosfat (T-Phosphat)', unit: 'mg/L' },
          { key: 'fecal_coliform', label: 'Fecal Coliform', unit: 'MPN/100mL' }
        ].map(({ key, label, unit }) => (
          <Card key={key}>
            <CardHeader>
              <CardTitle>{label}</CardTitle>
            </CardHeader>
            <CardContent>
              <p>
                {sensorData ? `${sensorData[key]} ${unit}` : 'Loading...'}
              </p>
            </CardContent>
          </Card>
        ))}
      </div>
    </div>
  );
}

export default App;
