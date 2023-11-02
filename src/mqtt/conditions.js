import mqtt from "mqtt";
import dotenv from "dotenv";
import Device from "../models/Device.js";
import nodemailer from "nodemailer";
dotenv.config();

const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const conditions = mqtt.connect(`mqtt://${ip}:1884`, {
	username: username,
	password: password,
});

conditions.on("connect", () => {
	console.log("Escuchando las condiciones");

	(async () => {
		try {
			const topics = await Device.getAllTopics();
			// Subscribe to all topics retrieved from the database
			topics.forEach((topic) => {
				conditions.subscribe(topic, (error) => {
					if (error) {
						console.error(`Error al suscribirse al tópico ${topic}:`, error);
					} else {
						console.log(`Escuchando las condiciones de: ${topic}`);
					}
				});
			});
		} catch (error) {
			console.error("Error al obtener tópicos", error);
		}
	})();
});

function checkCondition(value, operator, conditionValue) {
	switch (operator) {
		case "=":
			return value === conditionValue;
		case ">":
			return value > conditionValue;
		case "<":
			return value < conditionValue;
		case ">=":
			return value >= conditionValue;
		case "<=":
			return value <= conditionValue;
		default:
			return false;
	}
}

async function performAction(sentDate, topic, instruction) {
	const nowDate = new Date();
	const difference = (nowDate - sentDate) / (1000 * 60 * 60);
	if (difference > 1) {
		console.log(
			"Se envió al tópico " +
				topic +
				" la siguiente instrucción: " +
				instruction
		);
		const transporter = nodemailer.createTransport({
			service: "Gmail",
			auth: {
				user: "ucontrol.iotsystem@gmail.com",
				pass: "jhszraxaqmsmrdpl",
			},
		});

		const mailOptions = {
			from: "ucontrol.iotsystem@gmail.com",
			to: spaceCreator.email, // Correo del creador del espacio
			subject: "Acción enviada al dispositivo",
			html: `
				<html>
					<head>
						<style>
							body {
								font-family: Arial, sans-serif;
								background-color: #f4f4f4;
							}
							.container {
								max-width: 600px;
								margin: 0 auto;
								padding: 20px;
								background-color: #ffffff;
							}
						</style>
					</head>
					<body>
						<div class="container">
							<h2>Hola ,</h2>
							<p>.</p>
							<!-- Add more content here -->
						</div>
					</body>
				</html>
			`,
		};

		const info = await transporter.sendMail(mailOptions);
	}

	conditions.publish(topic + " / Switch", instruction, {
		qos: 1,
	});
}

conditions.on("message", async (topic, message) => {
	topic = topic.toString();
	try {
		const device = await Device.findOne({
			"conditions.listenerDevice": topic,
		});

		const secondDevice = await Device.findOne({
			"conditions.secondTopic": topic,
		});

		if (!device && !secondDevice) {
			console.log("No hay dispositivos escuchando este topico: " + topic);
			return;
		}

		const mensaje = message.toString();
		const valor = parseFloat(mensaje);
		let valorCondicion = "";
		if (device) {
			valorCondicion = parseFloat(device.conditions.conditionValue);

			if (checkCondition(valor, device.conditions.condition, valorCondicion)) {
				performAction(
					device.conditions.sentInstruction,
					device.topic,
					device.conditions.instruction
				);
			} else {
				console.log("La condición no se cumple");
			}
		} else {
			valorCondicion = parseFloat(secondDevice.conditions.secondConditionValue);
			if (
				checkCondition(valor, secondDevice.conditions.condition, valorCondicion)
			) {
				performAction(
					secondDevice.conditions.sentInstruction,
					secondDevice.topic,
					secondDevice.conditions.instruction
				);
			} else {
				console.log("La condición no se cumple");
			}
		}
	} catch (error) {
		console.error("Error processing message:", error);
	}
});

export default conditions;
