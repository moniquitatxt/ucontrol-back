import mqtt from "mqtt";
import dotenv from "dotenv";
import express from "express";
dotenv.config();

const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const instructionsClient = mqtt.connect(`mqtt://${ip}:1884`, {
	username: username,
	password: password,
});

const router = express.Router();

router.post("/sendInstruction", async (req, res) => {
	try {
		const { instruction, topic } = req.body;

		instructionsClient.publish(topic + " / Switch", instruction, {
			qos: 1,
		});
		res.status(201).json({
			success: true,
			message: "Instrucción enviada correctamente",
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

export default router;
