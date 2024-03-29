import { InfluxDB, Point } from "@influxdata/influxdb-client";
import mqtt from "mqtt";
import Device from "./models/Device.js";
import dotenv from "dotenv";
dotenv.config();

const url = process.env.INFLUX_URL;
const token = process.env.API_TOKEN;
const org = process.env.ORG_ID;
const bucket = process.env.BUCKET;
const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const client = mqtt.connect(`mqtt://${ip}:1884`, {
	username: username,
	password: password,
});

const influxDB = new InfluxDB({ url, token });

client.on("connect", () => {
	console.log("Conectado al broker");

	client.subscribe("Nuevo Dispositivo");
	client.subscribe("Topico extra");

	(async () => {
		try {
			const topics = await Device.getAllTopics();
			// Subscribe to all topics retrieved from the database
			topics.forEach((topic) => {
				client.subscribe(topic, (error) => {
					if (error) {
						console.error(`Error al suscribirse al tópico ${topic}:`, error);
					} else {
						console.log(`Suscrito con éxito a ${topic}`);
					}
				});
			});
		} catch (error) {
			console.error("Error al obtener tópicos", error);
		}
	})();
});

client.on("message", (topic, message) => {
	const writeApi = influxDB.getWriteApi(org, bucket);

	if (topic == "Nuevo Dispositivo") {
		console.log("Suscrito al nuevo dispositivo");
		client.subscribe(message.toString());
	} else if (topic == "Topico extra") {
		console.log("Suscrito al tópico extra");
		console.log(message.toString());
		client.subscribe(message.toString());
	} else {
		const topico = topic.toString();
		const mensaje = message.toString();
		console.log(mensaje);
		console.log(topico);
		const valor = parseFloat(mensaje);
		if (!isNaN(valor)) {
			const point = new Point(topico).floatField("value", valor);

			writeApi.writePoint(point);
			writeApi.close().then(() => {
				console.log(` Registrado ${point}`);
			});
		} else {
			console.log("No es un número");
		}
	}
});

client.on("error", (error) => {
	console.error("Conexión fallida", error);
});

client.on("reconnect", (error) => {
	console.error("Reconexión fallida", error);
});

export default client;
