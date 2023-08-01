import mongoose from "mongoose";
import dotenv from "dotenv";

// COSMOS_CONNECTION_STRING="mongodb+srv://moniquitatxt:Becauseyolo2310@iot-ucontrol-server.nq3zzs9.mongodb.net/?retryWrites=true&w=majority"

dotenv.config();

const db = () => {
	mongoose
		.connect(process.env.COSMOS_CONNECTION_STRING, {
			//   these are options to ensure that the connection is done properly
			useNewUrlParser: true,
			useUnifiedTopology: true,
		})
		.then(() => {
			console.log("Successfully connected to MongoDB Atlas!");
		})
		.catch((error) => {
			console.log("Unable to connect to MongoDB Atlas!");
			console.error(error);
		});
};

export default db;
