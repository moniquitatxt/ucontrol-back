import mqtt from "mqtt";
import dotenv from "dotenv";
dotenv.config();
import { InfluxDB, Point } from "@influxdata/influxdb-client";

const client = mqtt.connect("mqtt://192.168.0.101:1883");

const url = process.env.INFLUX_URL;
const token = process.env.API_TOKEN;
const org = process.env.ORG_ID;
const bucket = process.env.BUCKET;

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
