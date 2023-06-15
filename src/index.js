import express from "express";
import cors from "cors";
import "express-async-errors";
import dotenv from "dotenv";
import db from "./models/index.js";
import { MongoClient } from "mongodb";

dotenv.config();

const connectionString = process.env.COSMOS_CONNECTION_STRING || "";

const PORT = process.env.PORT || 3000;
const app = express();

app.use(cors());
app.use(express.json());

const Role = db.role;
db.mongoose
	.connect(connectionString)
	.then(() => {
		console.log("Successfully connect to MongoDB.");
		initial();
	})
	.catch((err) => {
		console.error("Connection error", err);
		process.exit();
	});

const initial = () => {
	const roleUser = new Role({
		name: "user",
	});

	roleUser
		.save()
		.then(() => {
			console.log("added 'user' to roles collection");
		})
		.catch((error) => {
			console.error("error", error);
		});
};

app.listen(PORT, () => {
	console.log(`Server is running on port: ${PORT}`);
});
