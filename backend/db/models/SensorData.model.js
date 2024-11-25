import mongoose from 'mongoose';

// Fungsi untuk mendapatkan waktu lokal dengan zona waktu UTC+7
const getLocalDate = () => {
  return new Date(new Date().getTime() + (7 * 60 * 60 * 1000)); // UTC+7
};

// Schema untuk data sensor (Realtime)
const realTimeDataSchema = new mongoose.Schema({
  timestamp: {
    type: Date,
    default: getLocalDate,
    required: true,
    index: { expires: 60 }  // TTL index dalam detik
  },
  pH: { type: Number, required: true, min: 0, max: 14 },
  DO: { type: Number, required: true },
  BOD: { type: Number, required: true },
  COD: { type: Number, required: true },
  TSS: { type: Number, required: true },
  nitrat: { type: Number, required: true },
  fosfat: { type: Number, required: true },
  fecal_coliform: { type: Number, required: true },
  IPj: { type: Number, required: true }
}, {
  collection: 'realtime_data' // Nama collection
});

// Schema untuk data long-term (Data harian)
const longTermDataSchema = new mongoose.Schema({
  date: {
    type: Date,
    required: true, // Tanggal (tanpa waktu) untuk mewakili hari
    unique: true // Pastikan hanya ada satu dokumen per hari
  },
  average_metrics: {
    pH: { type: Number, required: true },
    DO: { type: Number, required: true },
    BOD: { type: Number, required: true },
    COD: { type: Number, required: true },
    TSS: { type: Number, required: true },
    nitrat: { type: Number, required: true },
    fosfat: { type: Number, required: true },
    fecal_coliform: { type: Number, required: true },
    IPj: { type: Number, required: true }
  },
  max_metrics: {
    pH: { type: Number, required: true },
    DO: { type: Number, required: true },
    BOD: { type: Number, required: true },
    COD: { type: Number, required: true },
    TSS: { type: Number, required: true },
    nitrat: { type: Number, required: true },
    fosfat: { type: Number, required: true },
    fecal_coliform: { type: Number, required: true },
    IPj: { type: Number, required: true }
  },
  min_metrics: {
    pH: { type: Number, required: true },
    DO: { type: Number, required: true },
    BOD: { type: Number, required: true },
    COD: { type: Number, required: true },
    TSS: { type: Number, required: true },
    nitrat: { type: Number, required: true },
    fosfat: { type: Number, required: true },
    fecal_coliform: { type: Number, required: true },
    IPj: { type: Number, required: true }
  }
}, {
  collection: 'longterm_data' // Nama collection untuk data long-term
});

export const RealTimeData = mongoose.model('RealTimeData', realTimeDataSchema);
export const LongTermData = mongoose.model('LongTermData', longTermDataSchema);
