import express from "express";
import cors from "cors";
import "express-async-errors";
import db from "./db.js";
import authRoute from "./routes/auth.js";
import userRoute from "./routes/users.js";
import spaceRoute from "./routes/spaces.js";
import permissionRoute from "./routes/permissions.js";
import deviceRoute from "./routes/devices.js";
import accessControlRoute from "./routes/accessControlSpace.js";
import queriesRoute from "./influxdb/queries.js";
import instructionsRoute from "./routes/instructions.js";
import client from "./broker.js";
import control from "./mqtt/accessControl.js";
import conditions from "./mqtt/conditions.js";
import User from "./models/user.js";
import Permission from "./models/Permissions.js";
import Space from "./models/Space.js";
import Device from "./models/Device.js";
import nodemailer from "nodemailer";
import cron from "node-cron";
import { getAllSubspaces } from "./routes/permissions.js";
import dotenv from "dotenv";
import { InfluxDB, Point } from "@influxdata/influxdb-client";
dotenv.config();
const url = process.env.INFLUX_URL;
const token = process.env.API_TOKEN;
const org = process.env.ORG_ID;
const bucket = process.env.BUCKET;

const influxDB = new InfluxDB({ url, token });
const queryApi = influxDB.getQueryApi(org);
const PORT = process.env.PORT || 9000;
const app = express();

db();
app.use(cors());
app.use(express.json());

app.use("/api", authRoute);
app.use("/api", userRoute);
app.use("/api", deviceRoute);
app.use("/api", spaceRoute);
app.use("/api", permissionRoute);
app.use("/api", queriesRoute);
app.use("/api", accessControlRoute);
app.use("/api", instructionsRoute);

const sendWeeklyEmail = async (email) => {
	const transporter = nodemailer.createTransport({
		service: "Gmail",
		auth: {
			user: "ucontrol.iotsystem@gmail.com",
			pass: "jhszraxaqmsmrdpl",
		},
	});

	const user = await User.findOne({ email });

	if (!user) {
		console.log("User not found");
		return;
	}
	const userId = user._id.toString();

	// Find space IDs for which the user has permissions
	const permissionSpaces = await Permission.find({ userId }).distinct(
		"spaceId"
	);

	// Retrieve all subspaces recursively for each space
	let allSubspaces = [];
	for (const spaceId of permissionSpaces) {
		const subspaces = await getAllSubspaces(spaceId);
		allSubspaces = allSubspaces.concat(subspaces);
	}

	const spaces = await Space.find({
		$or: [{ _id: { $in: allSubspaces } }, { _id: { $in: permissionSpaces } }],
	});

	const devices = [];

	// Fetch devices for each space
	for (const spaceData of spaces) {
		const deviceIds = spaceData.devices.map((deviceId) => deviceId.toString());
		const spaceDevices = await Device.find({
			_id: { $in: deviceIds },
		});
		devices.push(...spaceDevices);
	}

	// Extrae los temas de los dispositivos
	const deviceInfo = devices.map((device) => ({
		topic: device.topic,
		type: device.type,
	}));
	if (deviceInfo.length > 0) {
		const emailContentArray = [];

		for (const device of deviceInfo) {
			const { topic, type } = device;
			if (type !== "controlAcceso") {
				if (type === "tempHum") {
					const fluxQuery = `
       		from(bucket: "${bucket}")
        	|> range(start: -7d) // Obtener datos de la última semana
      		|> filter(fn: (r) => r._measurement == "${topic} / Temperatura")`;

					const fluxQueryHum = `
			  from(bucket: "${bucket}")
		   |> range(start: -7d) // Obtener datos de la última semana
			 |> filter(fn: (r) => r._measurement == "${topic} / Humedad")`;
					// Ejecuta la consulta Flux
					const queryResult = await queryApi.collectRows(fluxQuery);
					const queryResultHum = await queryApi.collectRows(fluxQueryHum);
					if (queryResult.length === 0 && queryResultHum.length === 0) {
						// If both temperature and humidity queries returned no data
						emailContentArray.push(
							`Resultados para el tópico ${topic}: no se recolectaron datos para este tópico en esta semana`
						);
					} else {
						const formattedResults = queryResult.map((row) => {
							const time = new Date(row._time).toLocaleString("es-ES", {
								timeZone: "UTC",
							}); // Format time
							return `${time}: ${row._value}°C`;
						});

						const formattedResultsHum = queryResultHum.map((row) => {
							const time = new Date(row._time).toLocaleString("es-ES", {
								timeZone: "UTC",
							}); // Format time
							return `${time}: ${row._value}%`;
						});

						// Convierte el resultado en una cadena y agrégalo al contenido del correo
						emailContentArray.push(
							`Resultados para el tópico ${topic} (Temperatura):\n\n${formattedResults.join(
								"\n"
							)}`
						);

						emailContentArray.push(
							`Resultados para el tópico ${topic} (Humedad):\n\n${formattedResultsHum.join(
								"\n"
							)}`
						);
					}
				} else {
					const fluxQuery = `
				   from(bucket: "${bucket}")
				|> range(start: -7d) // Obtener datos de la última semana
				  |> filter(fn: (r) => r._measurement == "${topic}")`;

					// Ejecuta la consulta Flux
					const queryResult = await queryApi.collectRows(fluxQuery);
					if (queryResult.length === 0) {
						// If both temperature and humidity queries returned no data
						emailContentArray.push(
							`Resultados para el tópico ${topic}: no se recolectaron datos para este tópico en esta semana`
						);
					} else {
						const formattedResults = queryResult.map((row) => {
							const time = new Date(row._time).toLocaleString("es-ES", {
								timeZone: "America/Caracas",
							}); // Format time

							if (
								type === "movimiento" ||
								type === "vibraciones" ||
								type === "agua"
							) {
								const presence =
									row._value === 0 ? "No hubo presencia" : "Hubo presencia";
								return `${time}: ${presence}`;
							}

							if (type === "luz") {
								const presence = row._value === 0 ? "Apagada" : "Encendida";
								return `${time}: ${presence}`;
							}
							if (type === "aire") {
								const presence = row._value === 0 ? "Apagada" : "Encendida";
								return `${time}: ${presence}`;
							}
							if (type === "hum") {
								return `${time}: ${row._value}%`;
							}
						});

						emailContentArray.push(
							`Resultados para el tópico ${topic} \n\n${formattedResults.join(
								"\n"
							)}`
						);
					}
				}
			}
		}
		// Crea el contenido del correo con la lista de resultados
		const emailContent = emailContentArray.join("\n\n");

		const mailOptions = {
			from: "ucontrol.iotsystem@gmail.com",
			to: email,
			subject: "Reporte semanal",
			text: emailContent,
		};

		transporter.sendMail(mailOptions, (error, info) => {
			if (error) {
				console.log("Error sending email: " + error);
			} else {
				console.log("Email sent: " + info.response);
			}
		});
	}
};

// Schedule the email to be sent every Monday at 9:00 AM
cron.schedule("28 18 * * 1", async () => {
	// Get a list of user emails you want to send the email to
	try {
		// Fetch only email addresses from the database
		const users = await User.find({}, "email");

		// Extract emails and send the weekly email to each user
		users.forEach((user) => {
			sendWeeklyEmail(user.email);
		});
	} catch (error) {
		console.error("Error obteniendo los usuarios", error);
	}
});

app.listen(PORT, () => {
	console.log(`Servidor en el puerto: ${PORT}`);
});
