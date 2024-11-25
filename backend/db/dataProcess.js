import { RealTimeData, LongTermData } from './models/SensorData.model.js';

const seconds = 60;

// Helper function to format numbers to two decimal places
const formatToTwoDecimal = (num) => {
    return parseFloat(num.toFixed(2)); // Rounds to 2 decimal places
};

const calculateProcess = async () => {
    try {
        // Fetch the latest 60 records (assuming these are the last 60 seconds of data)
        const data = await RealTimeData.find({}).sort({ timestamp: -1 }).limit(seconds);

        if (data.length === 0) {
            console.log('No data found for the last minute.');
            return;
        }

        // Define the metrics to calculate
        const metrics = ['DO', 'IPj', 'BOD', 'pH', 'nitrat', 'TSS', 'COD', 'fosfat', 'fecal_coliform'];

        // Initialize containers to hold the calculated values
        let averageMetrics = {};
        let maxMetrics = {};
        let minMetrics = {};

        // Calculate average, max, and min for each metric
        metrics.forEach((metric) => {
            const values = data.map((item) => item[metric]);

            // Calculate the average for the metric
            averageMetrics[metric] = formatToTwoDecimal(values.reduce((sum, val) => sum + val, 0) / values.length);

            // Calculate the max and min for the metric
            maxMetrics[metric] = formatToTwoDecimal(Math.max(...values));
            minMetrics[metric] = formatToTwoDecimal(Math.min(...values));
        });

        // Save the calculated data to the LongTermData collection
        const longTermData = new LongTermData({
            date: new Date(), // current timestamp
            average_metrics: averageMetrics,
            max_metrics: maxMetrics,
            min_metrics: minMetrics
        });

        await longTermData.save();

        // Optionally, you can clear the processed data from RealTimeData after saving
        await RealTimeData.deleteMany({});
        console.log('Real-time data cleared successfully and Long-term data saved successfully');

    } catch (err) {
        console.error('Error fetching or processing data:', err);
    }
};

const processLongTermData = setInterval(() => {
    const now = new Date();
    const seconds = now.getSeconds();
    if (seconds === 0 || seconds === 60) {
      calculateProcess();
    }
  }, 1000);

export default processLongTermData;