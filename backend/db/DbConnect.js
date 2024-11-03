import mongoose from "mongoose";

const dbConnect = mongoose.connect('mongodb://localhost/sistem_digital', {
    useNewUrlParser: true,
    useUnifiedTopology: true,
})
    .then(() => console.log('Connected to MongoDB'))
    .catch(err => console.error('MongoDB connection error:', err));

export default dbConnect;