import mqtt from "mqtt";
import dotenv from "dotenv";
dotenv.config();
import { InfluxDB, Point } from "@influxdata/influxdb-client";

const url = process.env.INFLUX_URL;
const token = process.env.API_TOKEN;
const org = process.env.ORG_ID;
const bucket = process.env.BUCKET;
const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const client = mqtt.connect(`mqtt://${ip}:1883`, {
	username: username,
	password: password,
});

const influxDB = new InfluxDB({ url, token });

client.on("connect", () => {
	client.subscribe("temperatura");

	console.log("Conectado al broker");
});

client.on("message", (topic, message) => {
	const writeApi = influxDB.getWriteApi(org, bucket);

	writeApi.useDefaultTags({ region: "west" });

	const topico = topic.toString();
	const mensaje = message.toString();
	const valor = parseFloat(mensaje);

	const point = new Point(topico)
		.tag("sensor_id", "TLM01")
		.floatField("value", valor);
	console.log(` ${point}`);

	writeApi.writePoint(point);
	writeApi.close().then(() => {
		console.log("WRITE FINISHED");
	});
});

export default client;
