import mongoose from "mongoose";
import dotenv from "dotenv";

dotenv.config();

const db = () => {
	mongoose
		.connect(process.env.COSMOS_CONNECTION_STRING, {
			//   these are options to ensure that the connection is done properly
			useNewUrlParser: true,
			useUnifiedTopology: true,
		})
		.then(() => {
			console.log("Se ha conectado exitosamente a la base de datos");
		})
		.catch((error) => {
			console.log("No se pudo conectar a la base de datos");
			console.error(error);
		});
};

export default db;
