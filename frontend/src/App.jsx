import React, { useEffect, useState, useRef } from "react";
import { Badge } from "@/components/ui/badge";
import { Slider } from "@/components/ui/slider";
import { Switch } from "@/components/ui/switch";
import {
  Card,
  CardContent,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

import { CartesianGrid, Line, LineChart, XAxis, YAxis, Tooltip, ResponsiveContainer } from "recharts";
import { ChartContainer, ChartTooltipContent } from "@/components/ui/chart";

function App() {
  const [sensorData, setSensorData] = useState(null);
  const [isConnected, setIsConnected] = useState(false);
  const [sliderData, setSliderData] = useState({
    pH: 7,
    DO: 5,
    BOD: 3,
    COD: 10,
    TSS: 20,
    nitrat: 2,
    fosfat: 1,
    fecal_coliform: 50,
  });
  const [useServer, setUseServer] = useState({
    pH: true,
    DO: true,
    BOD: true,
    COD: true,
    TSS: true,
    nitrat: true,
    fosfat: true,
    fecal_coliform: true,
  });
  const [IPjData, setIPjData] = useState([]); // State for storing IPj data

  const IP_SERVER = "192.168.18.34:3000"; // Adjust your server's IP address
  const ws = useRef(null);

  // WebSocket logic to receive sensor data
  // WebSocket logic to receive sensor data
  useEffect(() => {
    ws.current = new WebSocket(`ws://${IP_SERVER}`); // Assign WebSocket instance to `ws.current`

    ws.current.onopen = () => {
      console.log("WebSocket connected");
      setIsConnected(true);
    };

    ws.current.onerror = (error) => {
      console.error("WebSocket error:", error);
      setIsConnected(false);
    };

    ws.current.onmessage = (event) => {
      try {
        const newData = JSON.parse(event.data);
        console.log("Received:", newData);
        setSensorData(newData);

        // Update IPjData with the new IPj value
        const activeData = Object.keys(sliderData).reduce((acc, key) => {
          acc[key] = useServer[key] ? newData[key] || sliderData[key] : sliderData[key];
          return acc;
        }, {});
        const IPj = calculateIPj(activeData);
        setIPjData((prevData) => {
          const newData = [...prevData, { time: new Date().toLocaleTimeString(), IPj }];

          // Ensure the IPjData array holds a maximum of 10 points
          if (newData.length > 10) {
            newData.shift(); // Remove the oldest data point to keep the chart size to 10 points
          }

          console.log("IPjData updated:", newData); // Log updated data to check
          return newData;
        });
      } catch (error) {
        console.error("Error parsing JSON:", error);
      }
    };

    ws.current.onclose = () => {
      console.log("WebSocket disconnected");
      setIsConnected(false);
    };

    return () => {
      ws.current.close(); // Close WebSocket on cleanup
    };
  }, []);

  const sendSwitchData = (key, value) => {
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      const message = {
        type: "switchUpdate",
        sensor: key,
        useServer: value,
      };

      // Jika useServer adalah false, kita kirimkan nilai sensor (sensorValue)
      if (!value) {
        message.sensorValue = sliderData[key]; // Kirim nilai sensor jika useServer false
      }

      // Kirim pesan ke WebSocket
      ws.current.send(JSON.stringify(message));
      console.log("Sent switch data:", message);
    } else {
      console.error("WebSocket is not open. Unable to send data.");
    }
  };

  const sendSliderValue = (sensor, value) => {
    if (ws.current && ws.current.readyState === WebSocket.OPEN) {
      const message = {
        type: "switchUpdate",
        sensor: sensor,
        useServer: false,  // Always false for slider value updates
        sensorValue: value,  // Send the new value of the sensor
      };

      // Send message to WebSocket server
      ws.current.send(JSON.stringify(message));
      console.log(`Sent slider value for ${sensor}:`, value);
    } else {
      console.error("WebSocket is not open. Unable to send slider value.");
    }
  };


  // Combine server and slider data
  const activeData = Object.keys(sliderData).reduce((acc, key) => {
    acc[key] = useServer[key] ? sensorData?.[key] || sliderData[key] : sliderData[key];
    return acc;
  }, {});

  // Calculate and determine water quality
  const calculateIPj = (data) => {
    const L = {
      pH: 7.0,
      DO: 6.0,
      BOD: 3.0,
      COD: 10.0,
      TSS: 100.0,
      nitrat: 10.0,
      fosfat: 3.0,
      fecal_coliform: 200.0,
    };
    const ratios = Object.keys(data).map(key => data[key] / L[key]);
    const maxRatio = Math.max(...ratios);
    const avgRatio = ratios.reduce((a, b) => a + b, 0) / ratios.length;
    return Math.pow(maxRatio, 2) + Math.pow(avgRatio, 2);
  };

  const determineWaterQuality = (IPj) => {
    if (IPj <= 1.0) return "baik";
    if (IPj <= 5.0) return "cemar ringan";
    if (IPj <= 10.0) return "cemar sedang";
    return "cemar berat";
  };

  const IPj = calculateIPj(activeData);
  const waterQuality = determineWaterQuality(IPj);

  let badgeColor = "default";
  let desk = "---";
  if (waterQuality === "baik") {
    badgeColor = "success";
    desk = "Kualitas air baik, layak untuk digunakan.";
  } else if (waterQuality === "cemar ringan") {
    badgeColor = "warning";
    desk = "Kualitas air cemar ringan, perlu perhatian.";
  } else if (waterQuality === "cemar sedang") {
    badgeColor = "mediumWarning";
    desk = "Kualitas air cemar sedang, perlu penanganan lebih lanjut.";
  } else if (waterQuality === "cemar berat") {
    badgeColor = "destructive";
    desk = "Kualitas air cemar berat, tidak layak untuk digunakan.";
  }

  return (
    <div className="container mx-auto px-4">
      <Card className="my-4 shadow-lg bg-white rounded-lg">
        <CardHeader className="pb-0">
          <CardTitle className="text-lg font-semibold text-gray-800 text-center">
            Kualitas Air
          </CardTitle>
        </CardHeader>
        <CardContent className="flex flex-col justify-center py-0">
          {/* Wrap the chart within ChartContainer */}
          <ChartContainer config={{ color: "hsl(var(--chart-1))" }}>
            <div className="w-full h-full">
              <ResponsiveContainer>
                <LineChart data={IPjData} margin={{ top: 10, right: 10, left: 0, bottom: 0 }}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis
                    dataKey="time"
                    tick={{ fontSize: 10 }}
                    tickLine={false}
                    axisLine={false}
                  />
                  <YAxis
                    tick={{ fontSize: 10 }}
                    tickLine={false}
                    axisLine={false}
                    width={30}
                  />
                  <Tooltip content={<ChartTooltipContent hideLabel />} />
                  <Line
                    type="monotone"
                    dataKey="IPj"
                    stroke="#8884d8"
                    strokeWidth={2}
                    dot={false}
                    isAnimationActive={false}
                  />
                </LineChart>
              </ResponsiveContainer>
            </div>
          </ChartContainer>
          <strong className="flex mx-auto text-sm mt-2">{IPj.toFixed(2)}</strong>
        </CardContent>
        <CardContent className="flex flex-col items-center">
          <Badge variant={badgeColor} className="my-2 text-sm">
            <strong>{waterQuality}</strong>
          </Badge>
          <p className="text-sm text-gray-600 text-center px-4">{desk}</p>
        </CardContent>
      </Card>



      {/* Slider layout for each sensor */}
      <div className="grid grid-cols-2 gap-4 my-4">
        {[
          { key: "pH", label: "pH", unit: "", min: 0, max: 14 },
          { key: "DO", label: "Oksigen Terlarut (DO)", unit: "mg/L", min: 0, max: 15 },
          { key: "BOD", label: "Kebutuhan Oksigen Biologi (BOD)", unit: "mg/L", min: 0, max: 50 },
          { key: "COD", label: "Kebutuhan Oksigen Kimiawi (COD)", unit: "mg/L", min: 0, max: 100 },
          { key: "TSS", label: "Padatan Tersuspensi Total (TSS)", unit: "mg/L", min: 0, max: 500 },
          { key: "nitrat", label: "Nitrat (NO3-N)", unit: "mg/L", min: 0, max: 10 },
          { key: "fosfat", label: "Total Fosfat (T-Phosphat)", unit: "mg/L", min: 0, max: 10 },
          { key: "fecal_coliform", label: "Fecal Coliform", unit: "MPN/100mL", min: 0, max: 1000 },
        ].map(({ key, label, unit, min, max }) => (
          <Card key={key} className={`relative ${useServer[key] ? "bg-gray-200" : "bg-white"}`}>
            <CardHeader>
              <CardTitle className="text-sm text-gray-700">{label}</CardTitle>
            </CardHeader>
            <CardContent className="flex flex-col items-center justify-center mb-12">
              {!useServer[key] ? (
                <>
                  <p className="text-center text-xl font-semibold">{sliderData[key]} {unit}</p>
                  <Slider
                    value={[sliderData[key]]}
                    onValueChange={(value) => {
                      setSliderData((prev) => {
                        const newSliderData = { ...prev, [key]: value[0] };  // Update slider value locally
                        if (!useServer[key]) {  // If useServer is false, send updated value to server
                          sendSliderValue(key, value[0]);  // Send the new value
                        }
                        return newSliderData;  // Return updated slider state
                      });
                    }}
                    min={min}
                    max={max}
                    step={0.1}  // Optional: Adjust the step size for slider movement
                    className="w-full mt-4"
                  />
                </>
              ) : (
                <p className="text-center text-xl font-semibold">
                  {sensorData ? `${sensorData[key]} ${unit}` : "Loading..."}
                </p>
              )}
            </CardContent>
            <CardFooter className="absolute bottom-2 left-2 flex items-center p-2">
              <Switch
                checked={!useServer[key]}
                onCheckedChange={(value) => {
                  setUseServer((prev) => ({ ...prev, [key]: !value }));
                  sendSwitchData(key, !value);
                }
                }
              />
            </CardFooter>
          </Card>
        ))}
      </div>


    </div>
  );
}

export default App;
