import express from "express";
import dotenv from "dotenv";
import { InfluxDB, Point } from "@influxdata/influxdb-client";
dotenv.config();

const url = process.env.INFLUX_URL;
const token = process.env.API_TOKEN;
const org = process.env.ORG_ID;
const bucket = process.env.BUCKET;

const influxDB = new InfluxDB({ url, token });
const queryApi = influxDB.getQueryApi(org);

const router = express.Router();

router.get("/temperature", async (req, res) => {
	try {
		// Write your Flux query here
		const fluxQuery = `
        from(bucket: "${bucket}")
          |> range(start: -10d) // Change the time range as needed
          |> filter(fn: (r) => r._measurement == "temperatura")`;

		// Execute the Flux query
		const queryResult = await queryApi.collectRows(fluxQuery);

		// Return the query result as JSON response
		res.json(queryResult);
	} catch (error) {
		console.error("Error executing Flux query:", error);
		res
			.status(500)
			.json({ error: "An error occurred while processing the request" });
	}
});

export default router;
