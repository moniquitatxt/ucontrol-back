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

	client.subscribe("Control de acceso");
	client.subscribe("Nuevo Dispositivo");
	client.subscribe("Pato / Bombillo");

	client.subscribe(
		"Escuela de Ingeniería Civil / Oficina Profe Yolanda / Bombillo de la oficina / Switch"
	);
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
	}

	//writeApi.useDefaultTags({ region: "west" });
	if (topic == "Control de acceso") {
		console.log(message.toString());
	} else {
		const topico = topic.toString();
		const mensaje = message.toString();
		const valor = parseFloat(mensaje);

		const point = new Point(topico).floatField("value", valor);
		//console.log(` ${point}`);

		writeApi.writePoint(point);
		writeApi.close().then(() => {
			console.log(` Registrado ${point}`);
		});
	}
});

client.on("error", (error) => {
	console.error("Conexión fallida", error);
});

client.on("reconnect", (error) => {
	console.error("Reconexión fallida", error);
});

export default client;
