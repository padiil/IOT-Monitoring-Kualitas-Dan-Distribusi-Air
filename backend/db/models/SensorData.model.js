import mongoose from 'mongoose';

// Fungsi untuk mendapatkan waktu lokal dengan zona waktu yang sesuai
const getLocalDate = () => {
  return new Date(new Date().getTime() + (7 * 60 * 60 * 1000)); // UTC+7
};

// Schema untuk data sensor
const sensorDataSchema = new mongoose.Schema({
  timestamp: {
    type: Date,
    default: getLocalDate,
    required: true
  },
  pH: {
    type: Number,
    required: true,
    min: 0,
    max: 14
  },
  DO: {
    type: Number,
    required: true
  },
  BOD: {
    type: Number,
    required: true
  },
  COD: {
    type: Number,
    required: true
  },
  TSS: {
    type: Number,
    required: true
  },
  nitrat: {
    type: Number,
    required: true
  },
  fosfat: {
    type: Number,
    required: true
  },
  fecal_coliform: {
    type: Number,
    required: true
  },
  IPj: {
    type: Number,
    required: true
  }
}, {
  collection: 'sensors_sec'
});

// Model untuk data sensor
const SensorData = mongoose.model('SensorData', sensorDataSchema);

export default SensorData;
