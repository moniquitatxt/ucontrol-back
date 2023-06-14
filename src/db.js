import { MongoClient } from "mongodb";
import dotenv from "dotenv";
dotenv.config();

const connectionString = process.env.COSMOS_CONNECTION_STRING || "";
const client = new MongoClient(connectionString);

let conn;
try {
	conn = await client.connect();
} catch (e) {
	console.error(e);
}

let db = conn.db("ucontrol");

export default db;
