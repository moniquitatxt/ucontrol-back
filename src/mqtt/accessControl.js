import mqtt from "mqtt";
import dotenv from "dotenv";
import AccessControlSpace from "../models/AccessControlSpace.js";
dotenv.config();

const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const control = mqtt.connect(`mqtt://${ip}:1884`, {
	username: username,
	password: password,
});

control.on("connect", () => {
	console.log("Conectado al broker este es el de control de acceso");

	(async () => {
		try {
			const topics = await AccessControlSpace.getAllTopics();
			// Subscribe to all topics retrieved from the database
			topics.forEach((topic) => {
				control.subscribe(topic, (error) => {
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

control.on("message", (topic, message) => {
	console.log("Este es el de control de acceso");

	const topico = topic.toString();
	const mensaje = message.toString();
});

control.on("error", (error) => {
	console.error("Conexión fallida", error);
});

control.on("reconnect", (error) => {
	console.error("Reconexión fallida", error);
});

export default control;
