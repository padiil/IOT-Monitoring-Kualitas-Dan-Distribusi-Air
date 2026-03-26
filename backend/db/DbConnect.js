import mongoose from 'mongoose';
import ensureRealtimeTimeseries from './ensureRealtimeTimeseries.js';

const MONGODB_URI =
  process.env.MONGODB_URI || 'mongodb://127.0.0.1:27017/IOT_Monitoring_Kualitas_Dan_Distribusi_Air';

const dbConnect = mongoose
  .connect(MONGODB_URI)
  .then(async () => {
    console.log('Connected to MongoDB');
    await ensureRealtimeTimeseries(mongoose.connection);
  })
  .catch((err) => console.error('MongoDB connection error:', err));

export default dbConnect;
