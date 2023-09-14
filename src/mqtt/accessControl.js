import mqtt from "mqtt";
import dotenv from "dotenv";
import AccessControlSpace from "../models/AccessControlSpace.js";
import AccessControlUser from "../models/AccessControlUser.js";
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

control.on("message", async (topic, message) => {
	console.log("Este es el de control de acceso");
	topic = topic.toString();
	const eCard = message.toString().trim();
	try {
		const accessControlUser = await AccessControlUser.findOne({ eCard });
		if (!accessControlUser) {
			control.publish(topic + " / Permiso", "El usuario no existe", { qos: 1 });
			throw new Error("Usuario no existe");
		}

		const accessControlSpace = await AccessControlSpace.findOne({ topic });
		if (!accessControlSpace.status) {
			control.publish(topic + " / Permiso", "El espacio está cerrado", {
				qos: 1,
			});
			throw new Error("El espacio está cerrado");
		}

		if (!accessControlSpace.allowedUsers.includes(accessControlUser._id)) {
			control.publish(
				topic + " / Permiso",
				"El usuario no tiene acceso a este espacio",
				{ qos: 1 }
			);

			throw new Error("El usuario no tiene acceso a este espacio");
		}
		// Get today's date without the time (just the date)
		const currentTime = new Date();

		// Find the user's history entry for today
		const userHistoryToday = accessControlSpace.userHistory.find(
			(entry) =>
				entry.userId.toString() === accessControlUser._id.toString() &&
				entry.entered.some(
					(openedTime) =>
						openedTime.getFullYear() === currentTime.getFullYear() &&
						openedTime.getMonth() === currentTime.getMonth() &&
						openedTime.getDate() === currentTime.getDate()
				)
		);

		if (!userHistoryToday) {
			// No entry for today, create a new one
			const newUserHistory = {
				userId: accessControlUser._id,
				entered: new Date(),
				status: true, // User is entering
			};
			accessControlSpace.userHistory.push(newUserHistory);
		} else {
			// Entry for today exists, update status and gotOut accordingly
			if (userHistoryToday.status === false) {
				// User is entering
				userHistoryToday.status = true;
				userHistoryToday.entered = new Date();
			} else {
				// User is getting out
				userHistoryToday.status = false;
				userHistoryToday.gotOut = new Date();
			}
		}

		await accessControlSpace.save(); //
		control.publish(
			topic + " / Permiso",
			"Se ha permitido el acceso al usuario",
			{ qos: 1 }
		);

		console.log(accessControlUser);
	} catch (error) {
		console.error("Hubo un error", error);
	}
});

control.on("error", (error) => {
	console.error("Conexión fallida", error);
});

control.on("reconnect", (error) => {
	console.error("Reconexión fallida", error);
});

export default control;
